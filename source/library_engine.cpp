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
std::list<IAddonSystem::Information> addons; // Maybe switch to use ->GetSubList()?
LUA_FUNCTION(engine_GetAddons)
{
	LUA->CreateTable();
	int i = 0;
	for (IAddonSystem::Information addon : addons)
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

std::list<IGameDepotSystem::Information> games;
LUA_FUNCTION(engine_GetGames)
{
	LUA->CreateTable();
	int i = 0;
	for (IGameDepotSystem::Information game : games)
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

std::list<IGamemodeSystem::Information> gamemodes;
LUA_FUNCTION(engine_GetGamemodes)
{
	LUA->CreateTable();
	int i = 0;
	for (IGamemodeSystem::Information gamemode : gamemodes)
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

IGamemodeSystem::Information active_gamemode;
LUA_FUNCTION(engine_ActiveGamemode)
{
	LUA->PushString(active_gamemode.name.c_str());

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

namespace GameDepot2
{

class System2
{
public:
	virtual void Refresh( ) = 0;
	virtual void Clear( ) = 0;
	virtual void Save( ) = 0;
	virtual void SetMount( uint, bool ) = 0;
	virtual void MarkGameAsMounted( std::string ) = 0;
	virtual const std::list<IGameDepotSystem::Information> &GetList( ) const = 0;
	virtual void MountAsMapFix( uint ) = 0;
	virtual void MountCurrentGame( const std::string & ) = 0;
};

}


void UpdateEngine() // We need to get all of this stuff on the main thread or else it will crash.
{
	Msg("1\n");
	if (filesystem->Addons())
		addons = filesystem->Addons()->GetList();
	Msg("2\n");
	//filesystem->Gamemodes();
	//gamemodes = filesystem->Gamemodes()->GetList();
	Msg("3\n");
	if (filesystem->Games())
		games = ((GameDepot2::System2*)filesystem->Games())->GetList();
	Msg("4\n");
	//if (filesystem->Gamemodes())
		//active_gamemode = filesystem->Gamemodes()->Active();
	Msg("5\n");
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