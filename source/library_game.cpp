#include "lua_threaded.h"
#include "source_ammodef.h"

LUA_FUNCTION(game_AddDecal)
{
	const char* decalName = LUA->CheckString(1);
	const char* materialName = LUA->CheckString(2);

	// ToDo: How does this even work

	return 0;
}

LUA_FUNCTION(game_AddParticles)
{
	const char* particleFileName = LUA->CheckString(1);
	
	// Gmod calls IsFileAccessAllowed which checks if the file extension is allowed. Gmods file whitelist / blacklist is implemented in that function.

	std::string str_file = particleFileName;
	if (str_file.size() >= 4 && str_file.substr(str_file.size() - 4) == ".pcf") {
		func_GMOD_LoadParticleConfigFile(particleFileName, false);
	}

	return 0;
}

LUA_FUNCTION(game_CleanUpMap)
{
	//bool dontSendToClients = LUA->GetBool(1);
	/* second arg: table -> entity filter
	// third arg: callback function

	g_bRemovingEntity_ptr // What does this do?

	g_bDisableEhandleAccess_ptr // What does this do?

	if (g_bRemovingEntity_ptr)
	{
		LUA->ThrowError("Cannot run game.CleanUpMap while deleting entities!");
		return 0;
	}

	if (CleanUpMapRunning)
	{
		LUA->ThrowError("Cannot run game.CleanUpMap while it is already running!");
		return 0;
	}

	// Do the actual cleanup

	if (!dontSendToClients)
		BroadcastLua("game.CleanUpMap()") // Thats really how it works.

	RagdollClear();
	*/

	return 0;
}

LUA_FUNCTION(game_ConsoleCommand)
{
	const char* cmd = LUA->CheckString(1);

	engine->GMOD_RawServerCommand(cmd);

	return 0;
}

LUA_FUNCTION(game_GetAmmoDamageType)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	LUA->PushNumber(ammo->DamageType(id));

	return 1;
}

LUA_FUNCTION(game_GetAmmoData)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	Ammo_t* data = ammo->GetAmmoOfIndex(id);

	if (!data)
	{
		LUA->PushNil();
		return 1;
	}

	LUA->CreateTable();
		LUA->PushNumber(data->eTracerType);
		LUA->SetField(-2, "tracer");

		LUA->PushNumber(data->nDamageType);
		LUA->SetField(-2, "dmgtype");

		LUA->PushNumber(data->nFlags);
		LUA->SetField(-2, "flags");

		LUA->PushNumber(data->nMaxSplashSize);
		LUA->SetField(-2, "maxsplash");

		LUA->PushNumber(data->nMinSplashSize);
		LUA->SetField(-2, "minsplash");

		LUA->PushNumber(data->physicsForceImpulse);
		LUA->SetField(-2, "force");

		LUA->PushString(data->pName);
		LUA->SetField(-2, "name");

		LUA->PushNumber(data->pMaxCarry);
		LUA->SetField(-2, "maxcarry");

		LUA->PushNumber(data->pNPCDmg);
		LUA->SetField(-2, "npcdmg");

		LUA->PushNumber(data->pPlrDmg);
		LUA->SetField(-2, "plydmg");

	return 1;
}

LUA_FUNCTION(game_GetAmmoForce)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	LUA->PushNumber(ammo->DamageForce(id));

	return 1;
}

LUA_FUNCTION(game_GetAmmoID)
{
	const char* name = LUA->CheckString(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	LUA->PushNumber(ammo->Index(name));

	return 1;
}

LUA_FUNCTION(game_GetAmmoMax)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	LUA->PushNumber(ammo->MaxCarry(id)); // Gmod uses GMOD_MaxCarry

	return 1;
}

LUA_FUNCTION(game_GetAmmoName)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	Ammo_t* data = ammo->GetAmmoOfIndex(id);

	if (!data)
	{
		LUA->PushNil();
	} else {
		LUA->PushString(data->pName);
	}

	return 1;
}

LUA_FUNCTION(game_GetAmmoNPCDamage)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	LUA->PushNumber(ammo->NPCDamage(id));

	return 1;
}

LUA_FUNCTION(game_GetAmmoPlayerDamage)
{
	int id = LUA->CheckNumber(1);

	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();
	LUA->PushNumber(ammo->PlrDamage(id));

	return 1;
}

LUA_FUNCTION(game_GetAmmoTypes)
{
	CAmmoDef* ammo = (CAmmoDef*)func_GetAmmoDef();

	LUA->CreateTable();
	for (int i=0;i<MAX_AMMO_TYPES;++i)
	{
		Ammo_t* data = ammo->GetAmmoOfIndex(i);
		if (data)
		{
			LUA->PushNumber(i);
			LUA->PushString(data->pName);
			LUA->SetTable(-3);
		}
	}

	return 1;
}

LUA_FUNCTION(game_GetGlobalCounter)
{
	//const char* name = LUA->CheckString(1);

	// ToDo

	return 0;
}

LUA_FUNCTION(game_GetGlobalState)
{
	//const char* name = LUA->CheckString(1);

	// ToDo

	return 0;
}

LUA_FUNCTION(game_GetIPAddress)
{
	LUA->PushString(engine->GMOD_GetServerAddress());

	return 1;
}

LUA_FUNCTION(game_GetMap)
{
	LUA->PushString(gpGlobal->mapname.ToCStr());

	return 1;
}

LUA_FUNCTION(game_GetMapNext)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_GetMapVersion)
{
	LUA->PushNumber(gpGlobal->mapversion);

	return 1;
}

LUA_FUNCTION(game_GetSkillLevel)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_GetTimeScale)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_GetWorld)
{
	// ToDo: We need the entity class for this.

	return 0;
}

LUA_FUNCTION(game_IsDedicated)
{
	LUA->PushBool(engine->IsDedicatedServer());

	return 1;
}

LUA_FUNCTION(game_KickID)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_LoadNextMap)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_MapLoadType)
{
	switch(gpGlobal->eLoadType)
	{
		case MapLoad_NewGame:
			LUA->PushString("newgame");
			break;
		case MapLoad_LoadGame:
			LUA->PushString("loadgame");
			break;
		case MapLoad_Transition:
			LUA->PushString("transition");
			break;
		case MapLoad_Background:
			LUA->PushString("background");
			break;
		default:
			LUA->PushNil();
	}

	return 1;
}

LUA_FUNCTION(game_MaxPlayers)
{
	LUA->PushNumber(gpGlobal->maxClients);

	return 1;
}

LUA_FUNCTION(game_MountGMA)
{
	// ToDo: This is going to be complex

	return 0;
}

LUA_FUNCTION(game_RemoveRagdolls)
{
	// ToDo
	// RagdollClear();

	return 0;
}

LUA_FUNCTION(game_SetGlobalCounter)
{
	// ToDo: This is going to be complex

	return 0;
}

LUA_FUNCTION(game_SetGlobalState)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_SetSkillLevel)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_SetTimeScale)
{
	// ToDo

	return 0;
}

LUA_FUNCTION(game_SinglePlayer)
{
	LUA->PushBool(gpGlobal->maxClients == 1);

	return 1;
}

LUA_FUNCTION(game_StartSpot)
{
	// ToDo

	// gpGlobal->startspot

	return 0;
}

void InitGame(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			//Add_Func(LUA, game_AddDecal, "AddDecal");
			Add_Func(LUA, game_AddParticles, "AddParticles");
			//Add_Func(LUA, game_CleanUpMap, "CleanUpMap");
			Add_Func(LUA, game_ConsoleCommand, "ConsoleCommand");

			Add_Func(LUA, game_GetAmmoDamageType, "GetAmmoDamageType");
			Add_Func(LUA, game_GetAmmoData, "GetAmmoData");
			Add_Func(LUA, game_GetAmmoForce, "GetAmmoForce");
			Add_Func(LUA, game_GetAmmoID, "GetAmmoID");
			Add_Func(LUA, game_GetAmmoMax, "GetAmmoMax");
			Add_Func(LUA, game_GetAmmoName, "GetAmmoName");
			Add_Func(LUA, game_GetAmmoNPCDamage, "GetAmmoNPCDamage");
			Add_Func(LUA, game_GetAmmoPlayerDamage, "GetAmmoPlayerDamage");
			Add_Func(LUA, game_GetAmmoTypes, "GetAmmoTypes");

			//Add_Func(LUA, game_GetGlobalCounter, "GetGlobalCounter");
			//Add_Func(LUA, game_GetGlobalState, "GetGlobalState");
			Add_Func(LUA, game_GetIPAddress, "GetIPAddress");
			Add_Func(LUA, game_GetMap, "GetMap");
			//Add_Func(LUA, game_GetMapNext, "GetMapNext");
			Add_Func(LUA, game_GetMapVersion, "GetMapVersion");
			//Add_Func(LUA, game_GetSkillLevel, "GetSkillLevel");
			//Add_Func(LUA, game_GetTimeScale, "GetTimeScale");
			//Add_Func(LUA, game_GetWorld, "GetWorld");
			Add_Func(LUA, game_IsDedicated, "IsDedicated");
			//Add_Func(LUA, game_KickID, "KickID");
			//Add_Func(LUA, game_LoadNextMap, "LoadNextMap");
			Add_Func(LUA, game_MapLoadType, "MapLoadType");
			Add_Func(LUA, game_MaxPlayers, "MaxPlayers");
			//Add_Func(LUA, game_MountGMA, "MountGMA");
			//Add_Func(LUA, game_RemoveRagdolls, "RemoveRagdolls");
			//Add_Func(LUA, game_SetGlobalCounter, "SetGlobalCounter");
			//Add_Func(LUA, game_SetGlobalState, "SetGlobalState");
			//Add_Func(LUA, game_SetSkillLevel, "SetSkillLevel");
			//Add_Func(LUA, game_SetTimeScale, "SetTimeScale");
			Add_Func(LUA, game_SinglePlayer, "SinglePlayer");
			//Add_Func(LUA, game_StartSpot, "StartSpot");

		LUA->SetField(-2, "game");
	LUA->Pop();
}