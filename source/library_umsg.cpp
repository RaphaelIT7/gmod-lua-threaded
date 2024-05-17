#include "lua_threaded.h"

LUA_FUNCTION(umsg_Start)
{
	return 0;
}

void InitUmsg(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, umsg_Start, "Start");
			Add_Func(LUA, umsg_Start, "Short");
			Add_Func(LUA, umsg_Start, "PoolString");
			Add_Func(LUA, umsg_Start, "Long");
			Add_Func(LUA, umsg_Start, "Float");
			Add_Func(LUA, umsg_Start, "Entity");
			Add_Func(LUA, umsg_Start, "End");
			Add_Func(LUA, umsg_Start, "Char");
			Add_Func(LUA, umsg_Start, "Bool");
			Add_Func(LUA, umsg_Start, "Angle");
			Add_Func(LUA, umsg_Start, "String");
			Add_Func(LUA, umsg_Start, "Vector");
			Add_Func(LUA, umsg_Start, "VectorNormal");
			
		LUA->SetField(-2, "umsg");
	LUA->Pop();
}