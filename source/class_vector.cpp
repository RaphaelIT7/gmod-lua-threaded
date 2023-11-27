#include "lua_threaded.h"

LUA_FUNCTION(Vector_Add)
{
	LUA->CheckType(1, Type::Vector);
	LUA->CheckType(2, Type::Vector);
	Vector vec1 = LUA->GetVector(1);
	Vector vec2 = LUA->GetVector(2);

	vec1.Init(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

void InitVectorClass(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_REG);
		LUA->CreateMetaTable("Vector");

			Add_Func(LUA, Vector_Add, "Add");

		LUA->SetField(-2, "Vector");
	LUA->Pop(2);
}