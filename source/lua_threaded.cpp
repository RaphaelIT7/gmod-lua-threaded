#include "detours.h"
#include "lua_threaded.h"
#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include <lua.h>
#include "CLuaGameCallback.h"
//#include <player.h>

int interfaces_count = 0;
std::unordered_map<double, ILuaThread*> interfaces;

CThreadFastMutex shared_table_mutex;
std::unordered_map<std::string, ILuaValue*> shared_table;

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

void RunString(ILuaInterface* LUA, const char* str)
{
	int result = func_luaL_loadstring(LUA->GetState(), str);
	if (result != 0)
	{
		const char* err = LUA->GetString(-1);
		LUA->Pop();

		Msg("[ERROR] ILuaInterface:RunString: %s\n", err);
		return;
	}

	LUA->PCall(0, LUA_MULTRET, 0);
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
		RunString(thread->IFace, str);
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
	return func_LuaPanic(state);;
}

int AdvancedLuaErrorReporter(lua_State* state)
{
	return func_AdvancedLuaErrorReporter(state);
}

ILuaInterface* CreateInterface()
{
	ILuaInterface* IFace = func_CreateLuaInterface(true);

	IFace->Init(new CLuaGameCallback(), true); //We should call it but we do everything manually. NOTE: We don't "cache" all strings. Gmod pushes all hooks in the Init

	//lua_State* state = func_luaL_newstate();

	//func_lua_atpanic(state, LuaPanic);
	
	// lua_pushcclosure(state, AdvancedLuaErrorReporter, 0);

	//IFace->SetState(state); // Set the State

	//func_luaL_openlibs(state);

	return IFace;
}

void ShutdowInterface(ILuaThread* thread)
{
	for (ILuaAction* action : thread->actions)
	{
		delete action;
	}

	func_CloseLuaInterface(thread->IFace);

	delete thread;
}

unsigned LuaThread(void* data)
{
	ILuaThread* thread_data = (ILuaThread*)data;
	ILuaInterface* IFace = CreateInterface();
	thread_data->IFace = IFace;
	InitLuaThreaded(IFace, thread_data->id);
	InitMetaTable(IFace);

	while(thread_data->run)
	{
		thread_data->mutex.Lock();

		for (ILuaAction* action : thread_data->actions)
		{
			if (strcmp(action->type, "run") == 0)
			{
				RunString(IFace, action->data);
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

	ShutdowInterface(thread_data);

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
		ShutdowInterface(interfaces[id]);
	}

	return 0;
}

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

LUA_FUNCTION(LuaThread_GetTable)
{
    ILuaInterface* ILUA = (ILuaInterface*)LUA;

    LUA->CreateTable();
    shared_table_mutex.Lock();
    for (auto& [key, value] : shared_table)
    {
		PushValue(LUA, value);
		LUA->SetField(-2, key.c_str());
    }
    shared_table_mutex.Unlock();

    return 1;
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

LUA_FUNCTION(LuaThread_SetValue)
{
	std::string key = (std::string)LUA->CheckString(1);
	int type = LUA->GetType(2);

	if (type == Type::Nil)
	{
		shared_table_mutex.Lock();
		if (shared_table.find(key) == shared_table.end())
			return 0;

		ILuaValue* val = shared_table[key];
		if (val)
		{
			shared_table.erase(key);
			SafeDelete(val);
		}
		shared_table_mutex.Unlock();

		return 0;
	}

	ILuaValue* val = GetOrCreate(key);
	val->type = type;

	FillValue(LUA, val, 2, type);

	shared_table_mutex.Lock();
	shared_table[key] = val;
	shared_table_mutex.Unlock();

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
			Add_Func(LUA, LuaThread_GetTable, "GetTable");
			Add_Func(LUA, LuaThread_SetValue, "SetValue");

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
			ShutdowInterface(thread);
		}
	}

	for (auto& [key, val] : shared_table)
	{
		PushValue(LUA, val);
		LUA->SetField(-2, key.c_str());
	}

	return 0;
}