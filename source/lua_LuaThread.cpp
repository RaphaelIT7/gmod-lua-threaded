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
	} else {
		thread->IFace = CreateInterface();
	}

	ThreadSleep(100); // Remove it later.

	return 0;
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
		PushValue(LUA, value);
		LUA->SetField(-2, key.c_str());
    }
    shared_table_mutex.Unlock();

    return 1;
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

LUA_FUNCTION(LuaThread_GetValue)
{
	std::string key = LUA->CheckString(1);

	auto it = shared_table.find(key);
	if (it != shared_table.end())
	{
		ILuaValue* val = it->second;
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
			Add_Func(LUA, LuaThread_GetValue, "GetValue");

		LUA->SetField(-2, "LuaThreaded");
	LUA->Pop();
}