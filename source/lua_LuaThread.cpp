#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include "CLuaGameCallback.h"
#include "lua_threaded.h"

LUA_FUNCTION(LuaThread_GetAllInterfaces)
{
	LUA->CreateTable();

	for (auto& [id, thread]: interfaces) {
		ILuaInterface_Push(LUA, thread->IFace, id);
		LUA->SetField(-2, std::to_string(id).c_str());
	}

	return 1;
}

LUA_FUNCTION(LuaThread_GetInterface)
{
	double id = LUA->CheckNumber(1);

	ILuaThread* thread = interfaces[id];
	if (!thread) { return 0; }

	ILuaInterface_Push(LUA, thread->IFace, id);

	return 1;
}

/*int AdvancedLuaErrorReporter(lua_State* state)
{
	return func_AdvancedLuaErrorReporter(state);
}*/

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
		while(!thread->init) { // Wait until it initialized
			ThreadSleep(1);
		}
	} else {
		thread->IFace = CreateInterface();
	}

	ILuaInterface_Push(LUA, thread->IFace, interfaces_count);

	return 1;
}

LUA_FUNCTION(LuaThread_CloseInterface)
{
	ILuaInterface_CheckType(LUA, 1);
	LUA_ILuaInterface* obj = ILuaInterface_GetUserdata(LUA, 1);

	double id = obj->ID;
	ILuaThread* thread = GetValidThread(LUA, id);

	if (!thread)
	{
		Msg("Failed to get Thread somehow.");
		return 0;
	}

	if (thread->threaded) {
		thread->run = false;
	} else {
		ShutdownInterface(thread);
	}

	return 0;
}

LUA_FUNCTION(LuaThread_GetTable)
{
    LUA->CreateTable();
    shared_table_mutex.Lock();
    for (auto& [key, value] : shared_table)
    {
		PushValue(LUA, key);
		PushValue(LUA, value);
		LUA->SetTable(-3);
    }
    shared_table_mutex.Unlock();

    return 1;
}

LUA_FUNCTION(LuaThread_SetValue)
{
	ILuaValue* key = new ILuaValue;
	FillValue(LUA, key, 1, LUA->GetType(1));

	int key_type = LUA->GetType(1);
	int type = LUA->GetType(2);

	if (type == GarrysMod::Lua::Type::Nil)
	{
		shared_table_mutex.Lock();
		for (auto& [sKey, sVal] : shared_table)
		{
			if (EqualValue(key, sKey))
			{
				SafeDelete(sKey);
				SafeDelete(sVal);
			}
		}
		shared_table_mutex.Unlock();
		SafeDelete(key);

		return 0;
	}

	ILuaValue* val = new ILuaValue;
	FillValue(LUA, val, 2, type);

	shared_table_mutex.Lock();
	shared_table[key] = val;
	shared_table_mutex.Unlock();

	return 0;
}

LUA_FUNCTION(LuaThread_GetValue)
{
	ILuaValue* key = new ILuaValue;
	FillValue(LUA, key, 1, LUA->GetType(1));

	ILuaValue* val = nullptr;
	shared_table_mutex.Lock();
	for (auto& [sKey, sVal] : shared_table)
	{
		if (EqualValue(key, sKey))
		{
			val = sVal;
			break;
		}
	}
	shared_table_mutex.Unlock();

	if (val)
	{
		PushValue(LUA, val);
	} else {
		LUA->PushNil();
	}

    return 1;
}

int LuaThread_Msg(lua_State* L)
{
	const char* msg = func_lua_tostring(L, 1, NULL);

	Msg("[LuaThreaded] %s\n", msg);

	return 0;
}

LUA_FUNCTION(LuaThread_IsMainThread)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	LUA->GetField(-1, "__InterfaceID");
	
	LUA->PushBool(LUA->GetNumber(0) == 0);

    return 1;
}

LUA_FUNCTION(LuaThread_ReadyThreads)
{
	if (!ThreadInMainThread())
		return 0;

	GMOD->threadready = true;

    return 1;
}

void InitLuaThreaded(GarrysMod::Lua::ILuaInterface* LUA, int id)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->PushNumber(id);
		LUA->SetField(-2, "__InterfaceID"); // Someone could change the __InterfaceID var but if someone would do this, it could break a bunch of stuff. So don't.

		LUA->CreateTable();
			Add_Func(LUA, LuaThread_GetAllInterfaces, "GetAllInterfaces");
			Add_Func(LUA, LuaThread_GetInterface, "GetInterface");
			Add_Func(LUA, LuaThread_CreateInterface, "CreateInterface");
			Add_Func(LUA, LuaThread_CloseInterface, "CloseInterface");
			Add_Func(LUA, LuaThread_Msg, "Msg");
			Add_Func(LUA, LuaThread_GetTable, "GetTable");
			Add_Func(LUA, LuaThread_SetValue, "SetValue");
			Add_Func(LUA, LuaThread_GetValue, "GetValue");
			Add_Func(LUA, LuaThread_IsMainThread, "IsMainThread");
			Add_Func(LUA, LuaThread_ReadyThreads, "ReadyThreads");

		LUA->SetField(-2, "LuaThreaded");
	LUA->Pop();
}