#include "lua_threaded.h"

static int32_t metatype = GarrysMod::Lua::Type::Vector;
static const char metaname[] = "Vector";
static const char invalid_error[] = "invalid Vector";
static const char table_name[] = "Vector_object";

struct LUA_Vector
{
	Vector vec;
};

void Push_Vector(ILuaBase* LUA, Vector& vec)
{
	//LUA->PushVector(vec);

	/*ILuaInterface* ILUA = (ILuaInterface*)LUA;
	ILuaObject* vec_obj = ILUA->CreateObject();
	vec_obj->SetUserData(&vec);

	ILuaObject* meta_obj = ILUA->GetMetaTableObject("Vector", metatype);
	vec_obj->SetMetaTable(meta_obj);

	ILUA->PushLuaObject(vec_obj);

	LUA->Remove(-2);*/ // 1 and a string Is pushed from somewhere?!?


	LUA_Vector* udata = (LUA_Vector*)LUA->NewUserdata(sizeof(LUA_Vector));
	udata->vec = vec;

	LUA->CreateMetaTableType("Vector", metatype);
	LUA->SetMetaTable(-2);
}

void Vector_CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, Type::UserData))
		luaL_typerror(LUA->GetState(), index, metaname);
}

LUA_Vector* Vector_GetUserdata(ILuaBase *LUA, int index)
{
	return (LUA_Vector*)LUA->GetUserdata(index);
}

Vector& Vector_Get(ILuaBase* LUA, int index)
{
	Vector_CheckType(LUA, index);

	Vector& vec = Vector_GetUserdata(LUA, index)->vec;
	if(vec == nullptr)
		LUA->ArgError(index, invalid_error);

	return vec;
}

void Vector_Destroy(ILuaBase *LUA, int index)
{
	LUA_Vector* vec = Vector_GetUserdata(LUA, index);

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(vec);
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
	Vector& vec = Vector_Get(LUA, 1);
	char szBuf[64] = {};
	V_snprintf(szBuf, sizeof(szBuf),"%f %f %f", vec[0], vec[1], vec[2]);
	LUA->PushString(szBuf);
	return 1;
}

LUA_FUNCTION_STATIC(Vector__index)
{
	const char* key = LUA->GetString(2);
	if (key != NULL) 
	{
		Vector& vec1 = Vector_Get(LUA, 1);
		if (strcmp(key, "x") == 0 || strcmp(key, "1") == 0) {
			LUA->PushNumber(vec1.x);
			return 1;
		} else if (strcmp(key, "y") == 0 || strcmp(key, "2") == 0) {
			LUA->PushNumber(vec1.y);
			return 1;
		} else if (strcmp(key, "z") == 0 || strcmp(key, "3") == 0) {
			LUA->PushNumber(vec1.z);
			return 1;
		}
	}

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
	const char* key = LUA->GetString(2);
	if (key == NULL)
		return 0;

	Vector& vec1 = Vector_Get(LUA, 1);
	if (strcmp(key, "x") == 0 || strcmp(key, "1") == 0) {
		vec1.x = std::stoi(LUA->GetString(3));
	} else if (strcmp(key, "y") == 0 || strcmp(key, "2") == 0) {
		vec1.y = std::stoi(LUA->GetString(3));
	} else if (strcmp(key, "z") == 0 || strcmp(key, "3") == 0) {
		vec1.z = std::stoi(LUA->GetString(3));
	}

	return 0;
}

LUA_FUNCTION_STATIC(Vector__eq)
{
	LUA->PushBool(Vector_Get(LUA, 1) == Vector_Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(Vector__add)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	Vector new_vec = Vector(vec1 + vec2);

	Push_Vector(LUA, new_vec);

	return 1;
}

LUA_FUNCTION_STATIC(Vector__sub)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	Vector new_vec = Vector(vec1 - vec2);
	Push_Vector(LUA, new_vec);

	return 1;
}

LUA_FUNCTION_STATIC(Vector__unm)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);

	Vector new_vec = Vector(-vec1);
	Push_Vector(LUA, new_vec);

	return 1;
}

LUA_FUNCTION_STATIC(Vector__mul)
{
	Vector_CheckType(LUA, 1);
	Vector& vec1 = Vector_Get(LUA, 1);

	if (LUA->IsType(2, Type::Number)) {
		int num = LUA->GetNumber(2);

		Vector new_vec = Vector(vec1 * num);
		Push_Vector(LUA, new_vec);
	} else {
		Vector_CheckType(LUA, 2);
		Vector& vec2 = Vector_Get(LUA, 2);

		Vector new_vec = Vector(vec1 * vec2);
		Push_Vector(LUA, new_vec);
	}

	return 1;
}

LUA_FUNCTION_STATIC(Vector__div)
{
	Vector_CheckType(LUA, 1);
	Vector& vec1 = Vector_Get(LUA, 1);

	if (LUA->IsType(2, Type::Number)) {
		int num = LUA->GetNumber(2);

		Vector new_vec = Vector(vec1 / num);
		Push_Vector(LUA, new_vec);
	} else {
		Vector_CheckType(LUA, 2);
		Vector& vec2 = Vector_Get(LUA, 2);

		Vector new_vec = Vector(vec1 / vec2);
		Push_Vector(LUA, new_vec);
	}

	return 1;
}

LUA_FUNCTION(Vector_Add)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	vec1 = vec1 + vec2;

	return 0;
}

LUA_FUNCTION(Vector_Length)
{
	Vector_CheckType(LUA, 1);
	Vector& vec1 = Vector_Get(LUA, 1);
	
	LUA->PushNumber(vec1.Length());

	return 1;
}

LUA_FUNCTION(Vector_Sub)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	vec1 = vec1 - vec2;

	return 0;
}

LUA_FUNCTION(Vector_Mul) // Undocumented: Second arg can be a Vector?
{
	Vector_CheckType(LUA, 1);
	Vector& vec1 = Vector_Get(LUA, 1);

	if (LUA->IsType(2, Type::Number)) {
		int num = LUA->GetNumber(2);

		vec1 = vec1 * num;
	} else {
		Vector_CheckType(LUA, 2);
		Vector& vec2 = Vector_Get(LUA, 2);

		vec1 = vec1 * vec2;
	}

	return 0;
}

LUA_FUNCTION(Vector_Div) // Undocumented: Second arg can be a Vector?
{
	Vector_CheckType(LUA, 1);
	Vector& vec1 = Vector_Get(LUA, 1);

	if (LUA->IsType(2, Type::Number)) {
		int num = LUA->GetNumber(2);

		vec1 = vec1 / num;
	} else {
		Vector_CheckType(LUA, 2);
		Vector& vec2 = Vector_Get(LUA, 2);

		vec1 = vec1 / vec2;
	}

	return 0;
}

LUA_FUNCTION(Vector_Normalize)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);
	
	VectorNormalize(vec);

	return 0;
}

LUA_FUNCTION(Vector_GetNormal)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);
	
	Vector new_vec = vec.Normalized();
	Push_Vector(LUA, new_vec);

	return 1;
}

LUA_FUNCTION(Vector_Dot)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	LUA->PushNumber(vec1.Dot(vec2));

	return 1;
}

LUA_FUNCTION(Vector_Cross)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	Push_Vector(LUA, vec1.Cross(vec2));

	return 1;
}

LUA_FUNCTION(Vector_Distance)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	LUA->PushNumber(vec1.DistTo(vec2));

	return 1;
}

LUA_FUNCTION(Vector_Angle)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	QAngle ang = QAngle();
	VectorAngles(vec, ang);
	Push_Angle(LUA, ang);

	return 1;
}

LUA_FUNCTION(Vector_AngleEx)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	QAngle ang = QAngle();
	VectorAngles(vec1, vec2, ang);
	Push_Angle(LUA, ang);

	return 1;
}

LUA_FUNCTION(Vector_Rotate)
{
	Vector_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	Vector& vec = Vector_Get(LUA, 1);
	QAngle& ang = Angle_Get(LUA, 2);

	matrix3x4_t matrix;
	AngleMatrix(ang, matrix);

	Vector out = Vector();
	VectorRotate(vec, matrix, out);

	vec = out;

	return 1;
}

LUA_FUNCTION(Vector_Length2D)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	LUA->PushNumber(vec.Length2D());

	return 1;
}

LUA_FUNCTION(Vector_LengthSqr)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	LUA->PushNumber(vec.LengthSqr());

	return 1;
}

LUA_FUNCTION(Vector_Length2DSqr)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	LUA->PushNumber(vec.Length2DSqr());

	return 1;
}

LUA_FUNCTION(Vector_DistToSqr)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	LUA->PushNumber(vec1.DistToSqr(vec2));

	return 1;
}

LUA_FUNCTION(Vector_WithinAABox)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector_CheckType(LUA, 3);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);
	Vector& vec3 = Vector_Get(LUA, 3);

	LUA->PushBool(vec1.WithinAABox(vec2, vec3));

	return 1;
}

LUA_FUNCTION(Vector_IsZero)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	LUA->PushBool(vec.x == 0 && vec.y == 0 && vec.z == 0);

	return 1;
}

LUA_FUNCTION(Vector_IsEqualTol)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);
	double tolerance = LUA->CheckNumber(3);

	LUA->PushBool(
		(abs(vec1.x - vec2.x) <= tolerance) &&
        (abs(vec1.y - vec2.y) <= tolerance) &&
        (abs(vec1.z - vec2.z) <= tolerance)
	);

	return 1;
}

LUA_FUNCTION(Vector_Zero)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	vec.x = 0;
	vec.y = 0;
	vec.z = 0;

	return 0;
}

LUA_FUNCTION(Vector_Set)
{
	Vector_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	vec1.x = vec2.x;
	vec1.y = vec2.y;
	vec1.z = vec1.z;

	return 0;
}

LUA_FUNCTION(Vector_Unpack)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	LUA->PushNumber(vec.x);
	LUA->PushNumber(vec.y);
	LUA->PushNumber(vec.z);

	return 3;
}

LUA_FUNCTION(Vector_SetUnpacked)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);
	double x = LUA->CheckNumber(2);
	double y = LUA->CheckNumber(3);
	double z = LUA->CheckNumber(4);

	vec.x = x;
	vec.y = y;
	vec.z = z;

	return 0;
}

LUA_FUNCTION(Vector_ToTable)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	LUA->CreateTable();
		LUA->PushNumber(vec.x);
		LUA->SetField(-2, "1");

		LUA->PushNumber(vec.y);
		LUA->SetField(-2, "2");

		LUA->PushNumber(vec.z);
		LUA->SetField(-2, "3");

	return 1;
}

LUA_FUNCTION(Vector_Random)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);
	int min = LUA->CheckNumber(2);
	int max = LUA->CheckNumber(3);

	vec.x = std::rand() % max + min;
	vec.y = std::rand() % max + min;
	vec.z = std::rand() % max + min;

	return 0;
}

LUA_FUNCTION(Vector_Negate)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	vec.Negate();

	return 0;
}

LUA_FUNCTION(Vector_GetNegated)
{
	Vector_CheckType(LUA, 1);
	Vector& vec = Vector_Get(LUA, 1);

	Vector new_vec = Vector(-vec.x, -vec.y, -vec.z);
	Push_Vector(LUA, new_vec);

	return 1;
}

LUA_FUNCTION(Global_Vector)
{
	double x = LUA->CheckNumber(1);
	double y = LUA->CheckNumber(2);
	double z = LUA->CheckNumber(3);

	Vector vec = Vector(x, y, z);
	Push_Vector(LUA, vec);

	return 1;
}

LUA_FUNCTION(Global_LerpVector)
{
	double delta = LUA->GetNumber(1);
	Vector& start = Vector_Get(LUA, 2);
	Vector& end = Vector_Get(LUA, 3);

	Vector vec = Vector(Lerp(delta, start.x, end.x), Lerp(delta, start.y, end.y), Lerp(delta, start.z, end.z));
	Push_Vector(LUA, vec);

	return 1;
}

LUA_FUNCTION(Global_OrderVectors)
{
	Vector& vec1 = Vector_Get(LUA, 1);
	Vector& vec2 = Vector_Get(LUA, 2);

	if (vec1.x > vec2.x)
	{
		double x = vec1.x;
		vec1.x = vec2.x;
		vec2.x = x;
	}

	if (vec1.y > vec2.y)
	{
		double y = vec1.y;
		vec1.y = vec2.y;
		vec2.y = y;
	}

	if (vec1.z > vec2.z)
	{
		double z = vec1.z;
		vec1.z = vec2.z;
		vec2.z = z;
	}

	return 0;
}

void InitVectorClass(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	LUA->CreateMetaTableType(metaname, metatype);
		Add_Func(LUA, Vector__gc, "__gc"); // Gmod doesn't have __gc
		Add_Func(LUA, Vector__index, "__index");
		Add_Func(LUA, Vector__newindex, "__newindex");
		Add_Func(LUA, Vector__tostring, "__tostring");
		Add_Func(LUA, Vector__eq, "__eq");
		Add_Func(LUA, Vector__add, "__add");
		Add_Func(LUA, Vector__sub, "__sub");
		Add_Func(LUA, Vector__unm, "__unm");
		Add_Func(LUA, Vector__mul, "__mul");
		Add_Func(LUA, Vector__div, "__div");

		Add_Func(LUA, Vector_Length, "Length"); // Defined after __tostring in Gmod

		Add_Func(LUA, Vector_Add, "Add"); // Defined after __add in Gmod
		Add_Func(LUA, Vector_Sub, "Sub");
		Add_Func(LUA, Vector_Mul, "Mul");
		Add_Func(LUA, Vector_Div, "Div");

		Add_Func(LUA, Vector_Normalize, "Normalize");
		Add_Func(LUA, Vector_GetNormal, "GetNormal"); // Deprecated
		Add_Func(LUA, Vector_GetNormal, "GetNormalized");
		Add_Func(LUA, Vector_Dot, "Dot");
		Add_Func(LUA, Vector_Dot, "DotProduct"); // Deprecated
		Add_Func(LUA, Vector_Cross, "Cross");
		Add_Func(LUA, Vector_Distance, "Distance");
		Add_Func(LUA, Vector_Angle, "Angle");
		Add_Func(LUA, Vector_AngleEx, "AngleEx");
		Add_Func(LUA, Vector_Rotate, "Rotate");
		Add_Func(LUA, Vector_Length2D, "Length2D");
		Add_Func(LUA, Vector_LengthSqr, "LengthSqr");
		Add_Func(LUA, Vector_Length2DSqr, "Length2DSqr");
		Add_Func(LUA, Vector_DistToSqr, "DistToSqr");
		Add_Func(LUA, Vector_WithinAABox, "WithinAABox");
		Add_Func(LUA, Vector_IsZero, "IsZero");
		Add_Func(LUA, Vector_IsEqualTol, "IsEqualTol");
		Add_Func(LUA, Vector_Zero, "Zero");
		Add_Func(LUA, Vector_Set, "Set");
		Add_Func(LUA, Vector_Unpack, "Unpack");
		Add_Func(LUA, Vector_SetUnpacked, "SetUnpacked");
		Add_Func(LUA, Vector_ToTable, "ToTable");
		Add_Func(LUA, Vector_Random, "Random");
		Add_Func(LUA, Vector_Negate, "Negate");
		Add_Func(LUA, Vector_GetNegated, "GetNegated");

	LUA->Pop(1);
}