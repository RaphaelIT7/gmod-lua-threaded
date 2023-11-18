#include "detours.h"
#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include <unordered_map>
#include <lua.h>

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

	bool run = true;
	bool threaded = false;
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

inline void CheckType(GarrysMod::Lua::ILuaBase* LUA, int32_t index)
{
	if(!LUA->IsType(index, metatype))
		luaL_typerror(LUA->GetState(), index, metaname);
}

inline LUA_ILuaInterface *GetUserdata(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	return LUA->GetUserType<LUA_ILuaInterface>(index, metatype);
}

static ILuaInterface* Get(GarrysMod::Lua::ILuaBase* LUA, int32_t index)
{
	CheckType(LUA, index);
	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if(udata == nullptr)
		LUA->ArgError(index, invalid_error);

	return udata->IFace;
}

void Push(GarrysMod::Lua::ILuaBase* LUA, ILuaInterface* Interface, int ID)
{
	if(Interface == nullptr)
	{
		LUA->PushNil();
		return;
	}

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
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

static void Destroy(GarrysMod::Lua::ILuaBase *LUA, int32_t index)
{
	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if (udata == nullptr)
		return;

	ILuaInterface* IFace = udata->IFace;

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
	LUA->PushUserdata(IFace);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

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

/*
	ILuaInterface functions
*/
LUA_FUNCTION(ILuaInterface_RunString)
{
	LUA_ILuaInterface* IData = GetUserdata(LUA, 1);
	const char* str = LUA->CheckString(2);

	ILuaThread* thread = interfaces[IData->ID];
	if (!thread)
	{
		LUA->ThrowError("Invalid ILuaInterface!");
	}

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
	ILuaInterface* IFace = Get(LUA, 1);

	func_InitLuaClasses(IFace);

	return 0;
}

LUA_FUNCTION(ILuaInterface_InitLibraries)
{
	ILuaInterface* IFace = Get(LUA, 1);

	func_InitLuaLibraries(IFace);

	return 0;
}

/*
	Module Table
*/
LUA_FUNCTION(LuaThread_GetInterface)
{
	int id = LUA->CheckNumber(1);

	if (id < 0 || id > interfaces_count) { return 0; }

	ILuaInterface* IFace = interfaces[id]->IFace;
	if (!IFace) { return 0; }

	Push(LUA, IFace, id);

	return 1;
}

LUA_FUNCTION(LuaPanic)
{
	func_LuaPanic(LUA->GetState());

	return 0;
}

LUA_FUNCTION(AdvancedLuaErrorReporter)
{
	func_AdvancedLuaErrorReporter(LUA->GetState());

	return 0;
}

ILuaInterface* CreateInterface(ILuaThread* data)
{
	ILuaInterface* IFace = func_CreateLuaInterface(true);

	data->IFace = IFace;

	//IFace->Init(); We should call it but we do everything manually. NOTE: We don't "cache" all strings. Gmod pushes all hooks in the Init

	lua_State* state = func_luaL_newstate();

	func_lua_atpanic(state, LuaPanic);
	
	// lua_pushcclosure(state, AdvancedLuaErrorReporter, 0);

	IFace->SetState(state); // Set the State

	return IFace;
}

unsigned LuaThread(void* data)
{
	ILuaThread* thread_data = (ILuaThread*)data;
	ILuaInterface* IFace = CreateInterface(thread_data);

	while(thread_data->run)
	{
		thread_data->mutex.Lock();

		for (ILuaAction* action : thread_data->actions)
		{
			if (strcmp(action->type, "run"))
			{
				func_luaL_loadstring(IFace->GetState(), action->data);
				IFace->PCall(0, LUA_MULTRET, 0);
			}

			delete action;
		}

		thread_data->actions.clear();

		thread_data->mutex.Unlock();
	}

	func_CloseLuaInterface(IFace);

	return 0;
}

LUA_FUNCTION(LuaThread_CreateInterface)
{
	bool not_threaded = LUA->GetBool(1);

	ILuaThread* thread = new ILuaThread;
	thread->threaded = !not_threaded;

	if (thread->threaded) {
		CreateSimpleThread(LuaThread, nullptr);
	} else {
		ILuaInterface* IFace = CreateInterface(thread);
	}

	return 0;
}

LUA_FUNCTION(LuaThread_CloseInterface)
{
	ILuaInterface* ILUA = (ILuaInterface*)LUA;
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

void Add_Func(GarrysMod::Lua::ILuaBase* LUA, CFunc Func, const char* Name) {
	LUA->PushCFunction(Func);
	LUA->SetField(-2, Name);
}

GMOD_MODULE_OPEN()
{
	Symbols_Init();

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, LuaThread_GetInterface, "GetInterface");
			Add_Func(LUA, LuaThread_CreateInterface, "CreateInterface");
			Add_Func(LUA, LuaThread_CloseInterface, "CloseInterface");

			LUA->SetField(-2, "LuaThreaded");
	LUA->Pop(2);

	LUA->CreateTable();
	LUA->SetField( GarrysMod::Lua::INDEX_REGISTRY, table_name );

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

	// NOTE: Currently broken?
	//LUA->PushCFunction(ILuaInterface_InitLibraries);
	//LUA->SetField(-2, "InitLibraries");

	LUA->Pop(1);

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