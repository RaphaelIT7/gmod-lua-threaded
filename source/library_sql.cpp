#include "lua_threaded.h"

LUA_FUNCTION(sql_Query) // ToDo
{

	return 0;
}

void InitSql(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, sql_Query, "Query");
			
		LUA->SetField(-2, "sql");
	LUA->Pop();
}