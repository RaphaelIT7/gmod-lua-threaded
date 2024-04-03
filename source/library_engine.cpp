#include <GarrysMod/InterfacePointers.hpp>
#include "lua_threaded.h"
#include <icommandline.h>
#include <eiface.h>

class IPlayerInfo;
class edict_t;
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
	PushValue(LUA, GMOD->addons);

	return 1;
}

LUA_FUNCTION(engine_GetUserContent) // Deprecated. If anyone wants to use it, tell me. Until then, I won't bother implementing it.
{
	PushValue(LUA, GMOD->usercontent);

	return 1;
}

LUA_FUNCTION(engine_GetGames)
{
	PushValue(LUA, GMOD->games);

	return 1;
}

LUA_FUNCTION(engine_GetGamemodes)
{
	PushValue(LUA, GMOD->gamemodes);

	return 1;
}

LUA_FUNCTION(engine_ActiveGamemode)
{
	LUA->PushString(GMOD->active_gamemode);

	return 1;
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

void UpdateEngine(ILuaInterface* LUA) // We need to get all of this stuff on the main thread or else it will crash. Update: It crashes everywhere. What is broken?
{
	Msg("UpdateEngine start\n");
	if (GMOD->addons)
	{
		SafeDelete(GMOD->addons);
		SafeDelete(GMOD->games);
		SafeDelete(GMOD->gamemodes);
		SafeDelete(GMOD->usercontent);
	}

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "engine");
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, "GetAddons");
			if (LUA->IsType(-1, Type::Function))
			{
				LUA->Call(0, 1);

				GMOD->addons = new ILuaValue;
				FillValue(LUA, GMOD->addons, -1, LUA->GetType(-1));
			} else {
				Msg("engine.GetAddons is not a valid function!\n");
			}
			LUA->Pop();

			LUA->GetField(-1, "GetGames");
			if (LUA->IsType(-1, Type::Function))
			{
				LUA->Call(0, 1);

				GMOD->games = new ILuaValue;
				FillValue(LUA, GMOD->games, -1, LUA->GetType(-1));
			} else {
				Msg("engine.GetGames is not a valid function!\n");
			}
			LUA->Pop();

			LUA->GetField(-1, "GetGamemodes");
			if (LUA->IsType(-1, Type::Function))
			{
				LUA->Call(0, 1);

				GMOD->gamemodes = new ILuaValue;
				FillValue(LUA, GMOD->gamemodes, -1, LUA->GetType(-1));
			} else {
				Msg("engine.GetGamemodes is not a valid function!\n");
			}
			LUA->Pop();

			LUA->GetField(-1, "GetUserContent");
			if (LUA->IsType(-1, Type::Function))
			{
				LUA->Call(0, 1);

				GMOD->usercontent = new ILuaValue;
				FillValue(LUA, GMOD->usercontent, -1, LUA->GetType(-1));
			} else {
				Msg("engine.GetUserContent is not a valid function!\n");
			}
			LUA->Pop();

			LUA->GetField(-1, "ActiveGamemode");
			if (LUA->IsType(-1, Type::Function))
			{
				LUA->Call(0, 1);

				GMOD->active_gamemode = LUA->GetString(-1);
			} else {
				Msg("engine.ActiveGamemode is not a valid function!\n");
			}
			LUA->Pop();
		} else {
			Msg("engine is not a valid table!\n");
		}
	LUA->Pop(2);
	Msg("UpdateEngine finish\n");
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