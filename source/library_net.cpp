#include "lua_threaded.h"
#include <inetchannel.h>
#include <inetmsghandler.h>
#include <inetmessage.h>
#include <bitbuf.h>

bf_write g_Started;

LUA_FUNCTION_STATIC(net_Start)
{
	if (g_Started.GetNumBitsWritten() != 0) // ToDo: Learn how netmessages work and how to use them.
	{
		// Error existing net message
	}

	// Get net message name and add it. 12 bits uint?

	return 0;
}

void InitNet(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			// ToDo
		LUA->SetField(-2, "net");
	LUA->Pop();
}