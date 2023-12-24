#include <GarrysMod/InterfacePointers.hpp>
#include "lua_threaded.h"
#include <icommandline.h>
#include <eiface.h>

static const char playerinfomanager_name[] = "PlayerInfoManager002";
class IPlayerInfoManager
{
public:
	virtual IPlayerInfo *GetPlayerInfo( edict_t *pEdict ) = 0;
	virtual CGlobalVars *GetGlobalVars( ) = 0;
};

CGlobalVars* GlobalVars()
{
	static CGlobalVars *iface_pointer = nullptr;
	if (iface_pointer == nullptr)
	{
		SourceSDK::FactoryLoader server_loader("server");
		auto player_info_manager = server_loader.GetInterface<IPlayerInfoManager>(
			playerinfomanager_name
		);
		if (player_info_manager != nullptr)
			iface_pointer = player_info_manager->GetGlobalVars();
	}

	return iface_pointer;
}

CGlobalVars* gpGlobal;
IVEngineServer* engine;
LUA_FUNCTION(engine_GetAddons)
{
	return 0;
}

LUA_FUNCTION(engine_GetUserContent)
{
	return 0;
}

LUA_FUNCTION(engine_GetGames)
{
	return 0;
}

LUA_FUNCTION(engine_GetGamemodes)
{
	return 0;
}

LUA_FUNCTION(engine_ActiveGamemode)
{
	return 0;
}

LUA_FUNCTION(engine_TickInterval)
{
	LUA->PushNumber(gpGlobal->interval_per_tick);

	return 1;
}

LUA_FUNCTION(engine_AbsoluteFrameTime)
{
	LUA->PushNumber(gpGlobal->absoluteframetime);

	return 1;
}

LUA_FUNCTION(engine_TickCount)
{
	LUA->PushNumber(gpGlobal->tickcount);

	return 1;
}

LUA_FUNCTION(engine_CloseServer)
{
	if (CommandLine()->FindParm("-systemtest"))
	{
		engine->ServerCommand("quit");
	}

	return 0;
}

LUA_FUNCTION(engine_LightStyle)
{
	int lightstyle = LUA->CheckNumber(1);
	const char* pattern = LUA->CheckString(2);

	engine->LightStyle(lightstyle, pattern);

	return 0;
}

void InitEngine(ILuaInterface* LUA)
{
	if (engine == nullptr)
	{
		engine = InterfacePointers::VEngineServer();
	}

	if (gpGlobal == nullptr)
	{
		gpGlobal = GlobalVars();
	}

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, engine_GetAddons, "GetAddons");
			Add_Func(LUA, engine_GetUserContent, "GetUserContent");
			Add_Func(LUA, engine_GetGames, "GetGames");
			Add_Func(LUA, engine_GetGamemodes, "GetGamemodes");
			Add_Func(LUA, engine_ActiveGamemode, "ActiveGamemode");
			Add_Func(LUA, engine_TickInterval, "TickInterval");
			Add_Func(LUA, engine_AbsoluteFrameTime, "AbsoluteFrameTime");
			Add_Func(LUA, engine_TickCount, "TickCount");
			Add_Func(LUA, engine_CloseServer, "CloseServer");
			Add_Func(LUA, engine_LightStyle, "LightStyle");
			
		LUA->SetField(-2, "engine");
	LUA->Pop();
}