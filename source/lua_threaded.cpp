#include "detours.h"
#include "lua_threaded.h"
#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include <unordered_map>
#include <lua.h>
#include <vector>
#include "CLuaGameCallback.h"
//#include <player.h>

struct ILuaValue
{
	int type;

	int number;
	const char* string;
	Vector vec;
	QAngle ang;
};

struct ILuaAction
{
	const char* type;
	const char* data;
};

struct ILuaThread
{
	ILuaInterface* IFace;
	CThreadFastMutex mutex;

	std::vector<ILuaAction*> actions;
	std::unordered_map<std::string, ILuaValue*> shared_table;

	bool run = true;
	bool threaded = false;
	
	int id = -1;
};

int interfaces_count = 0;
std::unordered_map<double, ILuaThread*> interfaces;

static int32_t metatype = GarrysMod::Lua::Type::NONE;
static const char metaname[] = "ILuaInterface";
static const char invalid_error[] = "invalid ILuaInterface";
static const char table_name[] = "ILuaInterface_object";

struct LUA_ILuaInterface
{
	ILuaInterface* IFace;
	int ID;
};

inline void CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, metatype))
		luaL_typerror(LUA->GetState(), index, metaname);
}

inline LUA_ILuaInterface *GetUserdata(ILuaBase *LUA, int index)
{
	return LUA->GetUserType<LUA_ILuaInterface>(index, metatype);
}

static ILuaInterface* Get(ILuaBase* LUA, int index)
{
	CheckType(LUA, index);

	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if(udata == nullptr)
		LUA->ArgError(index, invalid_error);

	return udata->IFace;
}

void Push(ILuaBase* LUA, ILuaInterface* Interface, int ID)
{
	if(Interface == nullptr)
	{
		LUA->PushNil();
		return;
	}

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(Interface);
	LUA->GetTable(-2);
	if(LUA->IsType(-1, metatype))
	{
		LUA->Remove(-2);
		return;
	}

	LUA->Pop(1);

	LUA_ILuaInterface *udata = LUA->NewUserType<LUA_ILuaInterface>(metatype);
	udata->IFace = Interface;
	udata->ID = ID;

	LUA->PushMetaTable(metatype);
	LUA->SetMetaTable(-2);

	LUA->CreateTable();
	LUA->SetFEnv(-2);

	LUA->PushUserdata(Interface);
	LUA->Push(-2);
	LUA->SetTable(-4);
	LUA->Remove(-2);
}

static void Destroy(ILuaBase *LUA, int index)
{
	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if (udata == nullptr)
		return;

	ILuaInterface* IFace = udata->IFace;

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(IFace);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

static ILuaThread* GetValidThread(ILuaBase* LUA, double index)
{
	if (ThreadInMainThread())
	{
		LUA_ILuaInterface* IData = GetUserdata(LUA, index);

		ILuaThread* thread = interfaces[IData->ID];
		if (!thread)
		{
			LUA->ThrowError("Invalid ILuaInterface!");
		}

		return thread;
	} else {
		LUA->PushSpecial(SPECIAL_GLOB);
			LUA->GetField(-1, "__InterfaceID");
			double id = LUA->GetNumber(0);
		LUA->Pop(2);

		return interfaces[id];
	}
}

/*
	ILuaInterface functions
*/
LUA_FUNCTION_STATIC(gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(tostring)
{
	LUA->PushFormattedString("%s: %p", metaname, Get(LUA, 1));
	return 1;
}

LUA_FUNCTION_STATIC(eq)
{
	LUA->PushBool(Get(LUA, 1) == Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(index)
{
	LUA->GetMetaTable(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	if(!LUA->IsType(-1, GarrysMod::Lua::Type::NIL))
		return 1;

	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	return 1;
}

LUA_FUNCTION_STATIC(newindex)
{
	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);
	return 0;
}

LUA_FUNCTION(ILuaInterface_RunString)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* str = LUA->CheckString(2);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "run";
		action->data = str;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		func_luaL_loadstring(thread->IFace->GetState(), str);
		thread->IFace->PCall(0, LUA_MULTRET, 0);
	}

	return 0;
}

LUA_FUNCTION(ILuaInterface_InitClasses)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "initclasses";

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		func_InitLuaClasses(thread->IFace);
	}

	return 0;
}

LUA_FUNCTION(ILuaInterface_InitLibraries)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "initlibraries";

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		func_InitLuaLibraries(thread->IFace);
	}

	return 0;
}

LUA_FUNCTION(ILuaInterface_GetTable)
{
	ILuaInterface* ILUA = (ILuaInterface*)LUA;
	ILuaThread* thread = GetValidThread(LUA, 1);

	LUA->CreateTable();
	for (auto& [key, value] : thread->shared_table)
	{
		if (value->type == Type::NUMBER)
		{
			LUA->PushNumber(value->number);
		} else if (value->type == Type::Bool)
		{
			LUA->PushBool(value->number == 1);
		} else if (value->type == Type::String)
		{
			LUA->PushString(value->string);
		} else if (value->type == Type::Entity)
		{
			LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
				LUA->GetField(-1, "Entity");
					LUA->PushNumber(value->number);
					LUA->Call(1, 1);
					ILuaObject* entity = ILUA->GetObject(-1);
			LUA->Pop(2);

			entity->Push();
		} else if (value->type == Type::Vector)
		{
			LUA->PushVector(value->vec);
		} else if (value->type == Type::Angle)
		{
			LUA->PushAngle(value->ang);
		} else {
			continue;
		}

		LUA->SetField(-2, key.c_str());
	}

	return 1;
}

ILuaValue* GetOrCreate(ILuaThread* thread, const char* key)
{
	ILuaValue* val = thread->shared_table[key];

	if (val)
		return val;

	return new ILuaValue;
}

LUA_FUNCTION(ILuaInterface_SetValue)
{
	//ILuaInterface* ILUA = (ILuaInterface*)LUA;
	ILuaThread* thread = GetValidThread(LUA, 1);

	const char* key;
	int type;
	if (ThreadInMainThread()) {
		key = LUA->CheckString(2);
		type = LUA->GetType(3);
	} else {
		key = LUA->CheckString(1);
		type = LUA->GetType(2);
	}

	if (type == Type::Nil)
	{
		ILuaValue* val = thread->shared_table[key];
		if (val)
		{
			thread->shared_table.erase(key);
			delete val;
		}

		return 0;
	}

	ILuaValue* val = GetOrCreate(thread, key);
	val->type = type;
	if (type == Type::Number)
	{
		val->type = type;
		val->number = LUA->GetNumber(3);
	} else if (type == Type::Bool)
	{
		val->type = type;
		val->number = LUA->GetBool(3) ? 1 : 0;
	} else if (type == Type::String)
	{
		val->type = type;
		val->string = LUA->GetString();
	} else if (type == Type::Entity)
	{
		//val->type = type;
		//val->number = ((CBaseEntity*)ILUA->GetObject(3)->GetEntity())->edict()->m_EdictIndex;
	} else if (type == Type::Vector)
	{
		val->type = type;
		val->vec = LUA->GetVector(3);
	} else if (type == Type::Angle)
	{
		val->type = type;
		val->ang = LUA->GetAngle(3);
	}

	thread->shared_table[key] = val;

	return 0;
}

/*
	Module Table
*/
LUA_FUNCTION(LuaThread_GetAllInterfaces)
{
	LUA->CreateTable();

	for (auto& [id, thread]: interfaces) {
		Push(LUA, thread->IFace, id);
		LUA->SetField(-2, std::to_string(id).c_str());
	}

	return 1;
}

LUA_FUNCTION(LuaThread_GetInterface)
{
	double id = LUA->CheckNumber(1);

	ILuaThread* thread = interfaces[id];
	if (!thread) { return 0; }

	Push(LUA, thread->IFace, id);

	return 1;
}

int LuaPanic(lua_State* state)
{
	func_LuaPanic(state);

	return 0;
}

int AdvancedLuaErrorReporter(lua_State* state)
{
	func_AdvancedLuaErrorReporter(state);

	return 0;
}

ILuaInterface* CreateInterface()
{
	ILuaInterface* IFace = func_CreateLuaInterface(true);

	IFace->Init(new CLuaGameCallback(), true); //We should call it but we do everything manually. NOTE: We don't "cache" all strings. Gmod pushes all hooks in the Init

	//lua_State* state = func_luaL_newstate();

	//func_lua_atpanic(state, LuaPanic);
	
	// lua_pushcclosure(state, AdvancedLuaErrorReporter, 0);

	//IFace->SetState(state); // Set the State

	//InitMetaTable(IFace);

	//func_luaL_openlibs(state);

	return IFace;
}

unsigned LuaThread(void* data)
{
	ILuaThread* thread_data = (ILuaThread*)data;
	ILuaInterface* IFace = CreateInterface();
	thread_data->IFace = IFace;
	InitLuaThreaded(IFace, thread_data->id);

	while(thread_data->run)
	{
		thread_data->mutex.Lock();

		for (ILuaAction* action : thread_data->actions)
		{
			if (strcmp(action->type, "run") == 0)
			{
				func_luaL_loadstring(IFace->GetState(), action->data);
				IFace->PCall(0, LUA_MULTRET, 0);
			} else if (strcmp(action->type, "initclasses") == 0)
			{
				func_InitLuaClasses(IFace);
			} else if (strcmp(action->type, "initlibraries") == 0)
			{
				func_InitLuaLibraries(IFace);
			}

			delete action;
		}

		thread_data->actions.clear();

		thread_data->mutex.Unlock();

		ThreadSleep(1); // Sleep 1 ms
	}

	func_CloseLuaInterface(IFace);

	return 0;
}

LUA_FUNCTION(LuaThread_CreateInterface)
{
	bool not_threaded = LUA->GetBool(1);

	ILuaThread* thread = new ILuaThread;
	thread->threaded = !not_threaded;

	interfaces_count += 1;
	interfaces[interfaces_count] = thread;

	thread->id = interfaces_count;

	if (thread->threaded) {
		CreateSimpleThread(LuaThread, thread);
	} else {
		thread->IFace = CreateInterface();
	}

	return 0;
}

LUA_FUNCTION(LuaThread_CloseInterface)
{
	LUA_ILuaInterface* obj = GetUserdata(LUA, 0);

	double id = obj->ID;
	if (!interfaces[id]) 
	{
		LUA->ThrowError("Invalid ILuaInterface!");
	}

	if (interfaces[obj->ID]->threaded) {
		interfaces[id]->run = false;
	} else {
		func_CloseLuaInterface(obj->IFace);
		interfaces[id] = nullptr;
	}

	return 0;
}

int LuaThread_Msg(lua_State* L)
{
	const char* msg = func_lua_tostring(L, 1, NULL);

	Msg("[LuaThreaded] %s\n", msg);

	return 0;
}

void Add_Func(GarrysMod::Lua::ILuaBase* LUA, CFunc Func, const char* Name) {
	LUA->PushCFunction(Func);
	LUA->SetField(-2, Name);
}

void InitLuaThreaded(ILuaInterface* LUA, int id)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->PushNumber(id);
		LUA->SetField(-2, "__InterfaceID");

		LUA->CreateTable();
			Add_Func(LUA, LuaThread_GetAllInterfaces, "GetAllInterfaces");
			Add_Func(LUA, LuaThread_GetInterface, "GetInterface");
			Add_Func(LUA, LuaThread_CreateInterface, "CreateInterface");
			Add_Func(LUA, LuaThread_CloseInterface, "CloseInterface");
			Add_Func(LUA, LuaThread_Msg, "Msg");

			if (!ThreadInMainThread()) 
			{
				Add_Func(LUA, ILuaInterface_GetTable, "GetTable");
				Add_Func(LUA, ILuaInterface_SetValue, "SetValue");
			}

			LUA->SetField(-2, "LuaThreaded");
	LUA->Pop(2);
}

void InitMetaTable(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	metatype = LUA->CreateMetaTable(metaname);

	LUA->PushCFunction(gc);
	LUA->SetField(-2, "__gc");

	LUA->PushCFunction(tostring);
	LUA->SetField(-2, "__tostring");

	LUA->PushCFunction(eq);
	LUA->SetField(-2, "__eq");

	LUA->PushCFunction(index);
	LUA->SetField(-2, "__index");

	LUA->PushCFunction(newindex);
	LUA->SetField(-2, "__newindex");

	LUA->PushCFunction(ILuaInterface_RunString);
	LUA->SetField(-2, "RunString");

	LUA->PushCFunction(ILuaInterface_InitClasses);
	LUA->SetField(-2, "InitClasses");

	LUA->PushCFunction(ILuaInterface_InitLibraries);
	LUA->SetField(-2, "InitLibraries");

	LUA->PushCFunction(ILuaInterface_GetTable);
	LUA->SetField(-2, "GetTable");

	LUA->PushCFunction(ILuaInterface_SetValue);
	LUA->SetField(-2, "SetValue");

	LUA->Pop(1);
}

GMOD_MODULE_OPEN()
{
	Symbols_Init();

	InitLuaThreaded((ILuaInterface*)LUA);

	InitMetaTable((ILuaInterface*)LUA);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	LUA->PushNil();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, metaname);

	for (auto& [id, thread]: interfaces) {
		if (thread->threaded) {
			thread->run = false;
		} else {
			func_CloseLuaInterface(thread->IFace);
		}
	}

	return 0;
}