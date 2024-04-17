#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/Lua/Interface.h>
#include "lua_threaded.h"

LUA_FUNCTION(LUA_ReadyThreads)
{
	GMOD->threadready = true;
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
	GMOD->gamecallback = ((GarrysMod::Lua::CLuaInterface*)LUA)->GetLuaGameCallback();
	if (get)
	{
		GMOD->version = get->Version();
		GMOD->versionstr = get->VersionStr();
		GMOD->branch = "unknown"; // Always unknown
	} else {
		LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
			LUA->GetField(-1, "VERSION");
			GMOD->version = LUA->GetNumber(-1);
			LUA->Pop();
		
			LUA->GetField(-1, "VERSIONSTR");
			GMOD->versionstr = LUA->GetString(-1);
			LUA->Pop();

			LUA->GetField(-1, "BRANCH");
			GMOD->branch = LUA->GetString(-1);
		LUA->Pop(2); // Global, BRANCH
	}

	GarrysMod::Lua::ILuaInterface* LLUA = (GarrysMod::Lua::ILuaInterface*)LUA;

	Symbols_Init();

	InitLuaThreaded(LLUA);

	InitMetaTable(LLUA);

	InitEnums(LLUA);

	filesystem = InterfacePointers::FileSystem();

	// NOTE: We need to wait until InitPostEntity is called because a bunch of stuff is missing which cause a crash.
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "hook");
		if (LUA->IsType(-1, GarrysMod::Lua::Type::Table)) {
			LUA->GetField(-1, "Add");
			if (LUA->IsType(-1, GarrysMod::Lua::Type::Function)) {
				LUA->PushString("InitPostEntity");
				LUA->PushString("LuaThreaded");
				LUA->PushCFunction(LUA_ReadyThreads);
				LLUA->CallFunctionProtected(3, 0, true);
			} else {
				LUA->Pop();
			}
		}
	LUA->Pop(2);

	Msg("LuaThreaded Loaded\n");

	return 0;
}

GMOD_MODULE_CLOSE()
{
	DestroyMetaTable((GarrysMod::Lua::ILuaInterface*)LUA);

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