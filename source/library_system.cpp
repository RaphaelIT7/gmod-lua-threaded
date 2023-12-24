#include "lua_threaded.h"

LUA_FUNCTION(system_IsWindows)
{
#ifdef SYSTEM_WINDOWS
	LUA->PushBool(true);
#else
	LUA->PushBool(false);
#endif

	return 1;
}

LUA_FUNCTION(system_IsOSX)
{
#ifdef SYSTEM_MACOS
	LUA->PushBool(true);
#else
	LUA->PushBool(false);
#endif

	return 1;
}

LUA_FUNCTION(system_IsLinux)
{
#ifdef SYSTEM_LINUX
	LUA->PushBool(true);
#else
	LUA->PushBool(false);
#endif

	return 1;
}

#define DEDICATED 1 // All functions below return nothing on a dedicated server.
LUA_FUNCTION(system_HasFocus)
{
#ifdef DEDICATED
	return 0;
#else
	// Steam Stuff

	return 1;
#endif
}

LUA_FUNCTION(system_BatteryPower)
{
#ifdef DEDICATED
	return 0;
#else
	// Steam Stuff

	return 1;
#endif
}

LUA_FUNCTION(system_AppTime)
{
#ifdef DEDICATED
	return 0;
#else
	// Steam Stuff

	return 1;
#endif
}

LUA_FUNCTION(system_UpTime)
{
#ifdef DEDICATED
	return 0;
#else
	// Steam Stuff

	return 1;
#endif
}

LUA_FUNCTION(system_SteamTime)
{
#ifdef DEDICATED
	return 0;
#else
	// Steam Stuff

	return 1;
#endif
}

LUA_FUNCTION(system_GetCountry)
{
#ifdef DEDICATED
	return 0;
#else
	// Steam Stuff

	return 1;
#endif
}

void InitSystem(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, system_IsWindows, "IsWindows");
			Add_Func(LUA, system_IsOSX, "IsOSX");
			Add_Func(LUA, system_IsLinux, "IsLinux");
			Add_Func(LUA, system_HasFocus, "HasFocus");
			Add_Func(LUA, system_BatteryPower, "BatteryPower");
			Add_Func(LUA, system_AppTime, "AppTime");
			Add_Func(LUA, system_UpTime, "UpTime");
			Add_Func(LUA, system_SteamTime, "SteamTime");
			Add_Func(LUA, system_GetCountry, "GetCountry");
			
		LUA->SetField(-2, "system");
	LUA->Pop();
}