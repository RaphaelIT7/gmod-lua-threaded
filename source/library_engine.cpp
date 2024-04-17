#include <GarrysMod/InterfacePointers.hpp>
#include "lua_threaded.h"
#include <icommandline.h>
#include <eiface.h>

#define GMOD_DEDICATED 1
#include <source_Addon.h>
#include <source_Gamemode.h>
#include <source_GameDepot.h>

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

LUA_FUNCTION(engine_GetAddons)
{
	LUA->CreateTable();
	int i = 0;
	New_Addon::FileSystem* addon_filesystem = (New_Addon::FileSystem*)filesystem->Addons();
	for (IAddonSystem::Information addon : addon_filesystem->GetList())
	{
		++i;
		LUA->CreateTable();

		LUA->PushBool(true); // ToDo: How can I check this? (Look into it while creating custom filesystem_stdio.dll)
		LUA->SetField(-2, "downloaded");

		LUA->PushNumber(addon.placeholder6); // Maybe placeholder6 contains the models count?
		LUA->SetField(-2, "models");

		LUA->PushString(addon.title.c_str());
		LUA->SetField(-2, "title");

		LUA->PushString(addon.file.c_str());
		LUA->SetField(-2, "file");

		LUA->PushBool(true); // ToDo: Same todo as for downloaded
		LUA->SetField(-2, "mounted");

		LUA->PushNumber(addon.wsid);
		LUA->SetField(-2, "wsid");

		LUA->PushNumber(addon.placeholder4); // Maybe placerholder4 is the size?
		LUA->SetField(-2, "size");

		LUA->PushNumber(addon.time_updated);
		LUA->SetField(-2, "updated");

		LUA->PushString(addon.tags.c_str());
		LUA->SetField(-2, "tags");

		LUA->PushNumber(addon.time_updated);
		LUA->SetField(-2, "timeadded");

		LUA->SetField(-2, std::to_string(i).c_str());
	}

	return 1;
}

LUA_FUNCTION(engine_GetUserContent) // Deprecated. If anyone wants to use it, tell me. Until then, I won't bother implementing it.
{
	LUA->CreateTable();

	return 1;
}

LUA_FUNCTION(engine_GetGames)
{
	LUA->CreateTable();
	int i = 0;
	New_GameDepot::System* games_system = (New_GameDepot::System*)filesystem->Games();
	for (IGameDepotSystem::Information game : games_system->GetList())
	{
		++i;
		LUA->CreateTable();

		LUA->PushNumber(game.depot);
		LUA->SetField(-2, "depot");

		LUA->PushString(game.title.c_str());
		LUA->SetField(-2, "title");

		LUA->PushBool(game.owned);
		LUA->SetField(-2, "owned");

		LUA->PushString(game.folder.c_str());
		LUA->SetField(-2, "folder");

		LUA->PushBool(game.mounted);
		LUA->SetField(-2, "mounted");

		LUA->PushBool(game.installed);
		LUA->SetField(-2, "installed");

		LUA->SetField(-2, std::to_string(i).c_str());
	}

	return 1;
}

LUA_FUNCTION(engine_GetGamemodes)
{
	LUA->CreateTable();
	int i = 0;
	New_Gamemode::System* gamemode_system = (New_Gamemode::System*)filesystem->Gamemodes();
	for (IGamemodeSystem::Information gamemode : gamemode_system->GetList())
	{
		++i;
		LUA->CreateTable();

		LUA->PushString(gamemode.title.c_str());
		LUA->SetField(-2, "title");

		LUA->PushNumber(gamemode.workshopid);
		LUA->SetField(-2, "workshopid");

		LUA->PushBool(gamemode.menusystem);
		LUA->SetField(-2, "menusystem");

		LUA->PushString(gamemode.maps.c_str());
		LUA->SetField(-2, "maps");

		LUA->PushString(gamemode.name.c_str());
		LUA->SetField(-2, "name");

		LUA->SetField(-2, std::to_string(i).c_str());
	}

	return 1;
}

LUA_FUNCTION(engine_ActiveGamemode)
{
	IGamemodeSystem::Information info = ((New_Gamemode::System*)filesystem->Gamemodes())->Active();
	LUA->PushString(info.title.c_str());

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

bool PushEngineFunction(GarrysMod::Lua::ILuaInterface* LUA, const char* eng)
{
	int ref = -1;

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "engine");
		if (LUA->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			LUA->GetField(-1, eng);
			if (LUA->IsType(-1, GarrysMod::Lua::Type::Function))
			{
				ref = LUA->ReferenceCreate();
			} else {
				Msg("engine.%s is not a valid function!\n", eng);
			}
			LUA->Pop(1);

		} else {
			Msg("engine is not a valid table!\n");
		}
	LUA->Pop(2);

	if (ref != -1)
	{
		LUA->ReferencePush(ref);
		LUA->ReferenceFree(ref);
		return true;
	}

	return false;
}

void InitEngine(GarrysMod::Lua::ILuaInterface* LUA)
{
	if (engine == nullptr)
	{
		engine = InterfacePointers::VEngineServer();
	}

	if (gpGlobal == nullptr)
	{
		gpGlobal = GlobalVars();
	}

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
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