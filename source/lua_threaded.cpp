#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/Lua/Interface.h>
#include "lua_threaded.h"

GMOD_MODULE_OPEN()
{
	Msg("LuaThreaded Loading\n");

	GMOD = new GMOD_Info;

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "VERSION");
		GMOD->version = LUA->GetNumber(-1);
		LUA->Pop();
		
		LUA->GetField(-1, "VERSIONSTR");
		GMOD->versionstr = LUA->GetString(-1);
		LUA->Pop();

		LUA->GetField(-1, "BRANCH");
		GMOD->branch = LUA->GetString(-1);
	LUA->Pop(2); // Global, VERSION, VERSIONSTR, BRANCH

	Symbols_Init();

	InitLuaThreaded((ILuaInterface*)LUA);

	InitMetaTable((ILuaInterface*)LUA);

	InitEnums((ILuaInterface*)LUA);

	filesystem = InterfacePointers::FileSystem();

	PreInitEngine();

	Msg("LuaThreaded Loaded\n");

	return 0;
}

GMOD_MODULE_CLOSE()
{
	DestroyMetaTable((ILuaInterface*)LUA);

	for (auto& [id, thread]: interfaces) {
		if (thread->threaded) {
			thread->run = false;
		} else {
			ShutdownInterface(thread);
		}
	}

	for (auto& [key, val] : shared_table)
	{
		PushValue(LUA, val);
		LUA->SetField(-2, key.c_str());
	}

	RemoveEnums();

	return 0;
}