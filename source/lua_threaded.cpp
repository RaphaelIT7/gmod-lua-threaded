#include <GarrysMod/Lua/Interface.h>
#include "lua_threaded.h"

GMOD_MODULE_OPEN()
{
	Msg("LuaThreaded Loading\n");

	GMOD = new GMOD_Info;

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "VERSION");
		GMOD->version = LUA->GetNumber(-1);
		
		LUA->GetField(-1, "VERSIONSTR");
		GMOD->versionstr = LUA->GetString(-1);

		LUA->GetField(-1, "BRANCH");
		GMOD->branch = LUA->GetString(-1);
	LUA->Pop(4); // Global, VERSION, VERSIONSTR, BRANCH

	Symbols_Init();

	InitLuaThreaded((ILuaInterface*)LUA);

	InitMetaTable((ILuaInterface*)LUA);

	Msg("LuaThreaded Loaded\n");

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
			ShutdownInterface(thread);
		}
	}

	for (auto& [key, val] : shared_table)
	{
		PushValue(LUA, val);
		LUA->SetField(-2, key.c_str());
	}

	return 0;
}