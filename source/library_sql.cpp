#include "lua_threaded.h"

LUA_FUNCTION_STATIC(sql_Query) // ToDo: Use sqlite3. Should this use a different db than sv.db?
{
	return 0;
}

void InitSql(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, sql_Query, "Query");
			
		LUA->SetField(-2, "sql");
	LUA->Pop();
}