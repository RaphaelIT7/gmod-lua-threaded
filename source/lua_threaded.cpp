#include <GarrysMod/Lua/Interface.h>
#include "lua_threaded.h"

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