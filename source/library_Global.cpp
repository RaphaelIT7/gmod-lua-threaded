#include "lua_threaded.h"

LUA_FUNCTION(include)
{
	ILuaThread* thread = GetValidThread(LUA, NULL);

	RunFile(thread, LUA->CheckString(1));
}

void InitGlobal(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		Add_Func(LUA, include, "include");

	LUA->Pop(1);
}