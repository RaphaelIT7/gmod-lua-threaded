#include <GarrysMod/Lua/Interface.h>
#include <lua_ILuaInterface.h>
#include "CLuaGameCallback.h"

#ifdef SYSTEM_WINDOWS
#include <GarrysMod/Lua/LuaShared.h>

static SourceSDK::FactoryLoader luashared_loader("lua_shared");
GarrysMod::Lua::ILuaInterface* Win_CreateInterface() {
	GarrysMod::Lua::ILuaShared* LuaShared = (GarrysMod::Lua::ILuaShared*)luashared_loader.GetFactory()(GMOD_LUASHARED_INTERFACE, nullptr);
	if (LuaShared == nullptr) {
		Msg("failed to get ILuaShared!\n");
		return nullptr;
	}

	return LuaShared->CreateLuaInterface(GarrysMod::Lua::State::SERVER, true);
}
#endif

IFileSystem* filesystem;

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
				PushValue(LUA, key);
				PushValue(LUA, val);
				LUA->SetTable(-3);
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
				for (auto& [key2, val2] : val->tbl)
				{
					SafeDelete(val2);
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
		std::unordered_map<ILuaValue*, ILuaValue*> tbl;

		LUA->Push(iStackPos);
		LUA->PushNil();
		while (LUA->Next(-2)) {
			LUA->Push(-2);

			ILuaValue* key = new ILuaValue;
			FillValue(LUA, key, -1, LUA->GetType(-1));

			ILuaValue* new_val = new ILuaValue;
			FillValue(LUA, new_val, -2, LUA->GetType(-2));

			tbl[key] = new_val;

			LUA->Pop(2);
		}
		LUA->Pop(1);

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

	ILuaThread* thread = GetValidThread(LUA, 0);
	longjmp(thread->jumpBuffer, 1);

	return 0;
}

ILuaInterface* CreateInterface()
{
#ifdef SYSTEM_WINDOWS
	ILuaInterface* IFace = Win_CreateInterface();
#else
	ILuaInterface* IFace = func_CreateLuaInterface(true);
#endif

	IFace->Init(new CLuaGameCallback(), true); // We should call it but we do everything manually. NOTE: We don't "cache" all strings. Gmod pushes all hooks in the Init

#ifndef SYSTEM_WINDOWS
	//lua_State* state = func_luaL_newstate();

	if (IFace->GetState() == nullptr)
	{
		Msg("Invalid Lua state?!?\n");
	}

	func_lua_atpanic(IFace->GetState(), LuaPanic);
#endif

	// lua_pushcclosure(state, AdvancedLuaErrorReporter, 0);

	//IFace->SetState(state); // Set the State

	//func_luaL_openlibs(state);


	// Push VERSION, VERSIONSTR, BRANCH, SERVER and CLIENT. Gmod does this inside CLuaManager::Startup();
	IFace->PushSpecial(SPECIAL_GLOB); // (Windows) ToDo: Why do u crash here :<
		IFace->PushNumber(GMOD->version);
		IFace->SetField(-2, "VERSION");

		IFace->PushString(GMOD->versionstr);
		IFace->SetField(-2, "VERSIONSTR");

		IFace->PushString(GMOD->branch);
		IFace->SetField(-2, "BRANCH");

		IFace->PushBool(true);
		IFace->SetField(-2, "SERVER");

		IFace->PushBool(false);
		IFace->SetField(-2, "CLIENT");
	IFace->Pop(1);

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

std::string ToPath(std::string path)
{
    size_t lastSeparatorPos = path.find_last_of("/\\");

    if (lastSeparatorPos != std::string::npos) {
        return path.substr(0, lastSeparatorPos + 1);
    }

    return path;
}

ILuaValue* CreateValue(int value)
{
	ILuaValue* val = new ILuaValue;
	val->type = Type::Number;
	val->number = value;

	return val;
}

ILuaValue* CreateValue(const char* value)
{
	ILuaValue* val = new ILuaValue;
	val->type = Type::String;
	val->string = value;

	return val;
}