#include <GarrysMod/Lua/Interface.h>
#include "CLuaGameCallback.h"
#include "lua_utils.h"

int interfaces_count = 0;
std::unordered_map<double, ILuaThread*> interfaces;

int shared_table_reference = -1;
CThreadFastMutex shared_table_mutex;
std::unordered_map<std::string, ILuaValue*> shared_table;

void PushValue(ILuaBase* LUA, ILuaValue* value)
{
	switch (value->type)
	{
		case Type::NUMBER:
			LUA->PushNumber(value->number);
			break;
		case Type::BOOL:
			LUA->PushBool(value->number == 1);
			break;
		case Type::STRING:
			LUA->PushString(value->string);
			break;
		case Type::ENTITY:
			/*
			LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
				LUA->GetField(-1, "Entity");
					LUA->PushNumber(value->number);
					LUA->Call(1, 1);
					ILuaObject* entity = ILUA->GetObject(-1);
			LUA->Pop(2);
			*/
			break;
		case Type::VECTOR:
			LUA->PushVector(value->vec);
			break;
		case Type::ANGLE:
			LUA->PushAngle(value->ang);
			break;
		case Type::Table:
			LUA->CreateTable();
			for (auto& [key, val] : value->tbl)
			{
				PushValue(LUA, val);
				LUA->SetField(-2, key.c_str());
			}
			break;
		default:
			LUA->PushNil();
			break;
	}
}

void SafeDelete(ILuaValue* value)
{
	if (value->type == Type::Table)
	{
		for (auto& [key, val] : value->tbl)
		{
			SafeDelete(val);
		}
	}

	delete value;
}

ILuaValue* GetOrCreate(std::string key)
{
	auto it = shared_table.find(key);
	if (it != shared_table.end())
	{
		ILuaValue* val = it->second;

		if (val) {
			if (val->type == Type::Table)
			{
				for (auto& [key, val] : val->tbl)
				{
					SafeDelete(val);
				}
			}

			return val;
		}
	}

	return new ILuaValue;
}

void FillValue(ILuaBase* LUA, ILuaValue* val, int iStackPos, int type)
{
	if (type == Type::Number)
	{
		val->type = type;
		val->number = LUA->GetNumber(iStackPos);
	} else if (type == Type::Bool)
	{
		val->type = type;
		val->number = LUA->GetBool(iStackPos) ? 1 : 0;
	} else if (type == Type::String)
	{
		val->type = type;
		val->string = LUA->GetString(iStackPos);
	} else if (type == Type::Entity)
	{
		//val->type = type;
		//val->number = ((CBaseEntity*)ILUA->GetObject(3)->GetEntity())->edict()->m_EdictIndex;
	} else if (type == Type::Vector)
	{
		val->type = type;
		val->vec = LUA->GetVector(iStackPos);
	} else if (type == Type::Angle)
	{
		val->type = type;
		val->ang = LUA->GetAngle(iStackPos);
	} else if (type == Type::Table)
	{
		val->type = type;
		std::unordered_map<std::string, ILuaValue*> tbl;

		LUA->PushNil();
		while (LUA->Next(-2)) {
			LUA->Push(-2);

			const char* key = LUA->GetString(-1);
			int val_type = LUA->GetType(-2);

			ILuaValue* new_val = new ILuaValue;

			FillValue(LUA, new_val, -2, val_type);
			tbl[(std::string)key] = new_val;

			LUA->Pop(2);
		}
		LUA->Pop();

		val->tbl = tbl;
	}
}

void Add_Func(GarrysMod::Lua::ILuaBase* LUA, CFunc Func, const char* Name) {
	LUA->PushCFunction(Func);
	LUA->SetField(-2, Name);
}

ILuaThread* FindThread(int id)
{
	auto it = interfaces.find(id);
	if (it != interfaces.end())
	{
		return it->second;
	}

	return nullptr;
}

LUA_FUNCTION(LuaPanic)
{
	ConDMsg("[LuaThread] Handling Lua Panic!\n");

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "__InterfaceID");
		int id = LUA->GetNumber(-1);

	LUA->Pop(2);

	ILuaThread* thread = FindThread(id);
	longjmp(thread->jumpBuffer, 1);

	return 0;
}

ILuaInterface* CreateInterface()
{
	ILuaInterface* IFace = func_CreateLuaInterface(true);

	IFace->Init(new CLuaGameCallback(), true); //We should call it but we do everything manually. NOTE: We don't "cache" all strings. Gmod pushes all hooks in the Init

	//lua_State* state = func_luaL_newstate();

	func_lua_atpanic(IFace->GetState(), LuaPanic);
	
	// lua_pushcclosure(state, AdvancedLuaErrorReporter, 0);

	//IFace->SetState(state); // Set the State

	//func_luaL_openlibs(state);

	return IFace;
}

void ShutdownInterface(ILuaThread* thread)
{
	for (ILuaAction* action : thread->actions)
	{
		delete action;
	}

	func_CloseLuaInterface(thread->IFace);

	delete thread;
}