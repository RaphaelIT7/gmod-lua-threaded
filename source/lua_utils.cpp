#include <GarrysMod/Lua/Interface.h>
#include "CLuaGameCallback.h"
#include "lua_threaded.h"

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
CGlobalVars* gpGlobal;
IVEngineServer* engine;

int interfaces_count = 0;
std::unordered_map<double, ILuaThread*> interfaces;

int shared_table_reference = -1;
CThreadFastMutex shared_table_mutex;
std::unordered_map<ILuaValue*, ILuaValue*, ILuaValueHash> shared_table;

void PushFile(GarrysMod::Lua::ILuaBase* LUA, LUA_File* file)
{
	Push_File(LUA, file->filename, file->fileMode, file->path);
}

void PushValue(GarrysMod::Lua::ILuaBase* LUA, ILuaValue* value)
{
	switch (value->type)
	{
		case GarrysMod::Lua::Type::NUMBER:
			LUA->PushNumber(value->number);
			break;
		case GarrysMod::Lua::Type::BOOL:
			LUA->PushBool(value->number == 1);
			break;
		case GarrysMod::Lua::Type::STRING:
			LUA->PushString(value->string);
			break;
		case GarrysMod::Lua::Type::ENTITY:
			/*
			LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
				LUA->GetField(-1, "Entity");
					LUA->PushNumber(value->number);
					LUA->Call(1, 1);
					ILuaObject* entity = ILUA->GetObject(-1);
			LUA->Pop(2);
			*/
			break;
		case GarrysMod::Lua::Type::VECTOR:
			Push_Vector(LUA, Vector(value->x, value->y, value->z));
			break;
		case GarrysMod::Lua::Type::ANGLE:
			Push_Angle(LUA, QAngle(value->x, value->y, value->z));
			break;
		case GarrysMod::Lua::Type::File:
			if (ThreadInMainThread()) // We cannot push a File from a our module to GMod.
			{
				LUA->PushNil();
				break;
			}

			PushFile(LUA, (LUA_File*)value->data);
			break;
		case GarrysMod::Lua::Type::Table:
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
	for (auto& [key, val] : value->tbl)
	{
		SafeDelete(val);
	}

	if (value->data != nullptr)
		delete value->data;

	delete value;
}

void FillValue(GarrysMod::Lua::ILuaBase* LUA, ILuaValue* val, int iStackPos, int type)
{
	if (type == GarrysMod::Lua::Type::Number)
	{
		val->type = type;
		val->number = LUA->GetNumber(iStackPos);
	} else if (type == GarrysMod::Lua::Type::Bool)
	{
		val->type = type;
		val->number = LUA->GetBool(iStackPos) ? 1 : 0;
	} else if (type == GarrysMod::Lua::Type::String)
	{
		val->type = type;
		val->string = LUA->GetString(iStackPos);
	} else if (type == GarrysMod::Lua::Type::Entity)
	{
		//val->type = type;
		//val->number = ((CBaseEntity*)ILUA->GetObject(3)->GetEntity())->edict()->m_EdictIndex;
	} else if (type == GarrysMod::Lua::Type::Vector)
	{
		val->type = type;
		if (ThreadInMainThread())
		{
			const Vector& vec = LUA->GetVector(iStackPos);
			val->x = vec.x;
			val->y = vec.y;
			val->z = vec.z;
		} else {
			LUA_Vector* vec = Vector_Get(LUA, iStackPos);
			val->x = vec->x;
			val->y = vec->y;
			val->z = vec->z;
		}
	} else if (type == GarrysMod::Lua::Type::Angle)
	{
		val->type = type;
		if (ThreadInMainThread())
		{
			const QAngle& ang = LUA->GetAngle(iStackPos);
			val->x = ang.x;
			val->y = ang.y;
			val->z = ang.z;
		} else {
			LUA_Angle* ang = Angle_Get(LUA, iStackPos);
			val->x = ang->x;
			val->y = ang->y;
			val->z = ang->z;
		}
	} else if (type == GarrysMod::Lua::Type::Table)
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
	} else if (type == GarrysMod::Lua::Type::File)
	{
		if (ThreadInMainThread()) // We cannot push a File from GMod to our module.
		{
			return;
		}

		LUA_File* file = File_Get(LUA, iStackPos);
		LUA_File* copy = new LUA_File;
		copy->fileMode = file->fileMode;
		copy->filename = file->filename;
		copy->path = file->path;
		//copy->handle = file->handle // Should we really share the handle?
		val->type = type;
		val->data = copy;
	}
}

void Add_Func(GarrysMod::Lua::ILuaBase* LUA, GarrysMod::Lua::CFunc Func, const char* Name) {
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

std::unordered_map<void*, std::unordered_map<std::string, int>> references;
void SafeOurFunnies(void* lua)
{
	GarrysMod::Lua::ILuaInterface* IFace = (GarrysMod::Lua::ILuaInterface*)lua;
	std::unordered_map<std::string, int> refs;

	IFace->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		IFace->GetField(-1, "debug");
		if (IFace->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			IFace->GetField(-1, "setlocal");
			refs["setlocal"] = IFace->ReferenceCreate();

			IFace->GetField(-1, "setupvalue");
			refs["setupvalue"] = IFace->ReferenceCreate();

			IFace->GetField(-1, "upvalueid");
			refs["upvalueid"] = IFace->ReferenceCreate();

			IFace->GetField(-1, "upvaluejoin");
			refs["upvaluejoin"] = IFace->ReferenceCreate();
		} else {
			Msg("[LuaThreaded] debug table doesn't seem to exist for ILuaInterface?\n");
		}
	IFace->Pop(2);

	references[lua] = refs;
}

GarrysMod::Lua::ILuaInterface* CreateInterface()
{
#ifdef SYSTEM_WINDOWS
	GarrysMod::Lua::ILuaInterface* IFace = Win_CreateInterface();
#else
	GarrysMod::Lua::ILuaInterface* IFace = func_CreateLuaInterface(true);
#endif
	// new CLuaGameCallback()
	AddStackCheckCallback(IFace, SafeOurFunnies);
	IFace->Init(GMOD->gamecallback, true); // We should call it but we do everything manually. NOTE: We don't "cache" all strings. Gmod pushes all hooks in the Init
	RemoveStackCheckCallback(IFace);

	if (references.find(IFace) != references.end())
	{
		IFace->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			IFace->GetField(-1, "debug");
			if (IFace->IsType(-1, GarrysMod::Lua::Type::Table))
			{
				std::unordered_map<std::string, int> refs = references[IFace];
				for (auto&[key, value] : refs)
				{
					IFace->PushString(key.c_str());
					IFace->ReferencePush(value);
					IFace->SetTable(-3);
				}
			} else {
				Msg("[LuaThreaded] debug table doesn't seem to exist for ILuaInterface?\n");
			}
		IFace->Pop(2);
	} else {
		Msg("[LuaThreaded] Our Callback wasn't called?\n");
	}

#ifndef SYSTEM_WINDOWS
	//lua_State* state = func_luaL_newstate();

	if (IFace->GetState() == nullptr)
	{
		Msg("Invalid Lua state?!?\n");
	}

	func_lua_atpanic(IFace->GetState(), LuaPanic); // NOTE for myself: NEVER remove THIS line. NEVER. It'll cause unknown crashes that will never give you any useful information!
#endif

	// lua_pushcclosure(state, AdvancedLuaErrorReporter, 0);

	//IFace->SetState(state); // Set the State

	//func_luaL_openlibs(state);


	// Push VERSION, VERSIONSTR, BRANCH, SERVER and CLIENT. Gmod does this inside CLuaManager::Startup();
	IFace->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB); // (Windows) ToDo: Why do u crash here :<
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
	val->type = GarrysMod::Lua::Type::Number;
	val->number = value;

	return val;
}

ILuaValue* CreateValue(const char* value)
{
	ILuaValue* val = new ILuaValue;
	val->type = GarrysMod::Lua::Type::String;
	val->string = value;

	return val;
}

bool EqualValue(ILuaValue* val1, ILuaValue* val2)
{
	if (val1->type != val2->type)
		return false;

	bool same = false;
	switch(val1->type)
	{
		case GarrysMod::Lua::Type::NUMBER:
			same = val1->number == val2->number;
			break;
		case GarrysMod::Lua::Type::BOOL:
			same = val1->number == val2->number;
			break;
		case GarrysMod::Lua::Type::STRING:
			same = strcmp(val1->string, val2->string) == 0;
			break;
		case GarrysMod::Lua::Type::ENTITY: // ToDo
			break;
		case GarrysMod::Lua::Type::VECTOR:
			same = val1->x == val2->x && val1->y == val2->y && val1->z == val2->z;
			break;
		case GarrysMod::Lua::Type::ANGLE:
			same = val1->x == val2->x && val1->y == val2->y && val1->z == val2->z;
			break;
		case GarrysMod::Lua::Type::File: // ToDo
			break;
		case GarrysMod::Lua::Type::Table: // ToDo
			break;
		default:
			break;
	}

	return same;
}