#include "lua_threaded.h"

static int32_t metatype = GarrysMod::Lua::Type::None;
static const char metaname[] = "Vector";
static const char invalid_error[] = "invalid Vector";
static const char table_name[] = "Vector_object";

struct LUA_Vector
{
	Vector vec;
};

void Push_Vector(ILuaBase* LUA, Vector vec)
{
	Msg("0, %i\n", LUA->Top());

	LUA_Vector *udata = LUA->NewUserType<LUA_Vector>(metatype);
	udata->vec = vec;

	Msg("1, %i\n", LUA->Top());
	LUA->PushMetaTable(metatype);
	Msg("2, %i\n", LUA->Top());
	LUA->SetMetaTable(-2);
	Msg("3, %i\n", LUA->Top());

	LUA->CreateTable();
	Msg("4, %i\n", LUA->Top());
	LUA->SetFEnv(-2);

	Msg("5, %i\n", LUA->Top());
	LUA->PushUserdata(udata);
	Msg("6, %i\n", LUA->Top());
	LUA->Push(-2);
	Msg("7, %i\n", LUA->Top());
	//ThreadSleep(100); // Crashes next line. So wait for Msg to show.
	LUA->Remove(-1);
	Msg("9, %i\n", LUA->Top());
}

void Vector_CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, metatype))
		luaL_typerror(LUA->GetState(), index, metaname);
}

Vector *Vector_GetUserdata(ILuaBase *LUA, int index)
{
	return LUA->GetUserType<Vector>(index, metatype);
}

Vector* Vector_Get(ILuaBase* LUA, int index)
{
	Vector_CheckType(LUA, index);

	Vector *vec = Vector_GetUserdata(LUA, index);
	if(vec == nullptr)
		LUA->ArgError(index, invalid_error);

	return vec;
}

void Vector_Destroy(ILuaBase *LUA, int index)
{
	Vector *vec = Vector_Get(LUA, index);

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata((void*)vec);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(Vector__gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	Vector_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(Vector__tostring)
{
	LUA->PushFormattedString("%s: %p", metaname, Vector_Get(LUA, 1));
	return 1;
}

LUA_FUNCTION_STATIC(Vector__eq)
{
	LUA->PushBool(Vector_Get(LUA, 1) == Vector_Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(Vector__index)
{
	LUA->GetMetaTable(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	if(!LUA->IsType(-1, GarrysMod::Lua::Type::NIL))
		return 1;

	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	return 1;
}

LUA_FUNCTION_STATIC(Vector__newindex)
{
	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);
	return 0;
}

LUA_FUNCTION(Vector_Add)
{
	LUA->CheckType(1, Type::Vector);
	LUA->CheckType(2, Type::Vector);
	Vector vec1 = LUA->GetVector(1);
	Vector vec2 = LUA->GetVector(2);

	vec1.Init(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);

	return 0;
}

LUA_FUNCTION(_Vector)
{
	int x = LUA->CheckNumber(1);
	int y = LUA->CheckNumber(2);
	int z = LUA->CheckNumber(3);

	Vector vec = Vector(x, y, z);
	Push_Vector(LUA, vec);
	//LUA->PushVector(vec);

	return 1;
}

void InitVectorClass(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	metatype = LUA->CreateMetaTable(metaname);

		Add_Func(LUA, Vector__gc, "__gc");
		Add_Func(LUA, Vector__tostring, "__tostring");
		Add_Func(LUA, Vector__eq, "__eq");
		Add_Func(LUA, Vector__index, "__index");
		Add_Func(LUA, Vector__newindex, "__newindex");
		Add_Func(LUA, Vector__gc, "__gc");
		Add_Func(LUA, Vector_Add, "Add");

	LUA->Pop(1);
}