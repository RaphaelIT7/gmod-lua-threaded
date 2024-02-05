#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/Lua/Interface.h>
#include "lua_threaded.h"

LUA_FUNCTION(LUA_UpdateEngine)
{
	UpdateEngine();
	return 0;
}

GMOD_MODULE_OPEN()
{
	Msg("LuaThreaded Loading\n");

	IGet* get;
	SourceSDK::FactoryLoader engine_loader("engine");
	get = ResolveSymbol<IGet>(
		engine_loader, IGet_Sym
	);
	
	GMOD = new GMOD_Info;
	if (get)
	{
		GMOD->version = get->Version();
		GMOD->versionstr = get->VersionStr();
		GMOD->branch = "unknown"; // Always unknown
	} else {
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
	}

	Symbols_Init();

	InitLuaThreaded((ILuaInterface*)LUA);

	InitMetaTable((ILuaInterface*)LUA);

	InitEnums((ILuaInterface*)LUA);

	filesystem = InterfacePointers::FileSystem();
	UpdateEngine();

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "hook");
		if (LUA->IsType(-1, Type::Table)) {
			LUA->GetField(-1, "Add");
			if (LUA->IsType(-1, Type::Function)) {
				LUA->PushString("GameContentChanged");
				LUA->PushString("LuaThreaded");
				LUA->PushCFunction(LUA_UpdateEngine);
				LUA->Call(3, 0);
			}
			LUA->Pop();
		}
	LUA->Pop(2);

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