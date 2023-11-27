#include "lua_threaded.h"

void InitNet(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			// ToDo
		LUA->SetField(-2, "net");
	LUA->Pop();
}