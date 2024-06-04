#include "lua_threaded.h"

LUA_FUNCTION_STATIC(umsg_Start)
{
	const char* name = LUA->CheckString(1);
	// second argument can be a player!
	CRecipientFilter filter;
	switch (LUA->GetType(2))
	{
		case GarrysMod::Lua::Type::RecipientFilter:
			filter.CopyFrom(*RecipientFilter_Get(LUA, 2)->filter);
			break;
		default:
			filter.AddAllPlayers();
			break;
	}
	filter.MakeReliable();

	UserMessageBegin(filter, name);
	return 0;
}

LUA_FUNCTION_STATIC(umsg_Short)
{
	MessageWriteShort(LUA->CheckNumber(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_PoolString)
{
	MessageWriteWord(LUA->CheckNumber(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Long)
{
	MessageWriteLong(LUA->CheckNumber(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Float)
{
	MessageWriteFloat(LUA->CheckNumber(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Entity)
{
	// ToDo

	return 0;
}

LUA_FUNCTION_STATIC(umsg_End)
{
	MessageEnd();

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Char)
{
	MessageWriteChar(LUA->CheckNumber(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Bool)
{
	MessageWriteBool(LUA->GetBool(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Angle)
{
	LUA_Angle* ang = Angle_Get(LUA, 1);
	MessageWriteAngles(QAngle(ang->x, ang->y, ang->z));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_String)
{
	MessageWriteString(LUA->CheckString(1));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_Vector)
{
	LUA_Vector* vec = Vector_Get(LUA, 1);
	MessageWriteVec3Coord(Vector(vec->x, vec->y, vec->z));

	return 0;
}

LUA_FUNCTION_STATIC(umsg_VectorNormal)
{
	LUA_Vector* vec = Vector_Get(LUA, 1);
	MessageWriteVec3Normal(Vector(vec->x, vec->y, vec->z));

	return 0;
}

void InitUmsg(GarrysMod::Lua::ILuaInterface* LUA) // NOTE: If you use this libary call LuaThreaded.LockMain before and UnlockMain after the umsg.
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, umsg_Start, "Start");
			Add_Func(LUA, umsg_Short, "Short");
			Add_Func(LUA, umsg_PoolString, "PoolString");
			Add_Func(LUA, umsg_Long, "Long");
			Add_Func(LUA, umsg_Float, "Float");
			//Add_Func(LUA, umsg_Entity, "Entity");
			Add_Func(LUA, umsg_End, "End");
			Add_Func(LUA, umsg_Char, "Char");
			Add_Func(LUA, umsg_Bool, "Bool");
			Add_Func(LUA, umsg_Angle, "Angle");
			Add_Func(LUA, umsg_String, "String");
			Add_Func(LUA, umsg_Vector, "Vector");
			Add_Func(LUA, umsg_VectorNormal, "VectorNormal");
			
		LUA->SetField(-2, "umsg");
	LUA->Pop();
}