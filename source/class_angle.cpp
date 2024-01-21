#include "lua_threaded.h"

static int32_t metatype = GarrysMod::Lua::Type::Angle;
static const char metaname[] = "Angle";
static const char invalid_error[] = "invalid Angle";
static const char table_name[] = "Angle_object";

void Push_Angle(ILuaBase* LUA, int pitch, int yaw, int roll)
{
	LUA_Angle* udata = (LUA_Angle*)LUA->NewUserdata(sizeof(LUA_Angle));
	udata->x = pitch;
	udata->y = yaw;
	udata->z = roll;

	LUA->CreateMetaTableType("Angle", metatype);
	LUA->SetMetaTable(-2);
}

void Push_Angle(ILuaBase* LUA, QAngle ang)
{
	Push_Angle(LUA, ang.x, ang.y, ang.z);
}

void Angle_CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, Type::UserData))
		luaL_typerror(LUA->GetState(), index, metaname);
}

LUA_Angle* Angle_GetUserdata(ILuaBase *LUA, int index)
{
	return (LUA_Angle*)LUA->GetUserdata(index);
}

LUA_Angle* Angle_Get(ILuaBase* LUA, int index)
{
	Angle_CheckType(LUA, index);

	LUA_Angle* ang = Angle_GetUserdata(LUA, index);

	return ang;
}

void Angle_Destroy(ILuaBase *LUA, int index)
{
	LUA_Angle* ang = Angle_GetUserdata(LUA, index);

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(ang);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(Angle__gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	Angle_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(Angle__tostring)
{
	LUA_Angle* ang = Angle_Get(LUA, 1);
	char szBuf[64] = {};
	V_snprintf(szBuf, sizeof(szBuf),"%.3f %.3f %.3f", ang->x, ang->y, ang->z);
	LUA->PushString(szBuf);
	return 1;
}

LUA_FUNCTION_STATIC(Angle__index)
{
	const char* key = LUA->GetString(2);
	if (key != NULL) 
	{
		LUA_Angle* ang = Angle_Get(LUA, 1);
		if (strcmp(key, "p") == 0 || strcmp(key, "pitch") == 0 || strcmp(key, "x") == 0 || strcmp(key, "1") == 0) {
			LUA->PushNumber(ang->x);
			return 1;
		} else if (strcmp(key, "y") == 0 || strcmp(key, "yaw") == 0 || strcmp(key, "y") == 0 || strcmp(key, "2") == 0) {
			LUA->PushNumber(ang->y);
			return 1;
		} else if (strcmp(key, "r") == 0 || strcmp(key, "roll") == 0 || strcmp(key, "z") == 0 || strcmp(key, "3") == 0) {
			LUA->PushNumber(ang->z);
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

LUA_FUNCTION_STATIC(Angle__newindex)
{
	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);
	const char* key = LUA->GetString(2);
	if (key == NULL)
		return 0;

	LUA_Angle* ang = Angle_Get(LUA, 1);
	if (strcmp(key, "p") == 0 || strcmp(key, "pitch") == 0 || strcmp(key, "x") == 0 || strcmp(key, "1") == 0) {
		ang->x = std::stoi(LUA->GetString(3));
	} else if (strcmp(key, "y") == 0 || strcmp(key, "yaw") == 0 || strcmp(key, "y") == 0 || strcmp(key, "2") == 0) {
		ang->y = std::stoi(LUA->GetString(3));
	} else if (strcmp(key, "r") == 0 || strcmp(key, "roll") == 0 || strcmp(key, "z") == 0 || strcmp(key, "3") == 0) {
		ang->z = std::stoi(LUA->GetString(3));
	}

	return 0;
}

LUA_FUNCTION_STATIC(Angle__eq)
{
	LUA->PushBool(Angle_Get(LUA, 1) == Angle_Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(Angle__add)
{
	Angle_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	LUA_Angle* ang1 = Angle_Get(LUA, 1);
	LUA_Angle* ang2 = Angle_Get(LUA, 2);

	Push_Angle(LUA, ang1->x + ang2->x, ang1->y + ang2->y, ang1->z + ang2->z);

	return 1;
}

LUA_FUNCTION_STATIC(Angle__sub)
{
	Angle_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	LUA_Angle* ang1 = Angle_Get(LUA, 1);
	LUA_Angle* ang2 = Angle_Get(LUA, 2);

	Push_Angle(LUA, ang1->x - ang2->x, ang1->y - ang2->y, ang1->z - ang2->z);

	return 1;
}

LUA_FUNCTION_STATIC(Angle__unm)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	Push_Angle(LUA, -ang->x, -ang->y, -ang->z);

	return 1;
}

LUA_FUNCTION_STATIC(Angle__mul)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	int num = LUA->CheckNumber(2);
	Push_Angle(LUA, ang->x * num, ang->y * num, ang->z * num);

	return 1;
}

LUA_FUNCTION_STATIC(Angle__div)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	int num = LUA->CheckNumber(2);
	Push_Angle(LUA, ang->x / num, ang->y / num, ang->z / num);

	return 1;
}

LUA_FUNCTION(Angle_Add)
{
	Angle_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	LUA_Angle* ang1 = Angle_Get(LUA, 1);
	LUA_Angle* ang2 = Angle_Get(LUA, 2);

	ang1->x += ang2->x;
	ang1->y += ang2->y;
	ang1->z += ang2->z;

	return 0;
}

LUA_FUNCTION(Angle_Sub)
{
	Angle_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	LUA_Angle* ang1 = Angle_Get(LUA, 1);
	LUA_Angle* ang2 = Angle_Get(LUA, 2);

	ang1->x -= ang2->x;
	ang1->y -= ang2->y;
	ang1->z -= ang2->z;

	return 0;
}

LUA_FUNCTION(Angle_Mul)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	int num = LUA->CheckNumber(2);
	ang->x *= num;
	ang->y *= num;
	ang->z *= num;

	return 0;
}

LUA_FUNCTION(Angle_Div)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	int num = LUA->CheckNumber(2);
	ang->x /= num;
	ang->y /= num;
	ang->z /= num;

	return 0;
}

LUA_FUNCTION(Angle_Forward)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	Vector forward = Vector();
	AngleVectors(QAngle(ang->x, ang->y, ang->z), &forward);

	Push_Vector(LUA, forward);

	return 1;
}

LUA_FUNCTION(Angle_Right)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	Vector right = Vector();
	AngleVectors(QAngle(ang->x, ang->y, ang->z), nullptr, &right, nullptr);

	Push_Vector(LUA, right);

	return 1;
}

LUA_FUNCTION(Angle_Up)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	Vector up = Vector();
	AngleVectors(QAngle(ang->x, ang->y, ang->z), nullptr, nullptr, &up);

	Push_Vector(LUA, up);

	return 1;
}

LUA_FUNCTION(Angle_RotateAroundAxis) // Verify this. Probably shit.
{
	Angle_CheckType(LUA, 1);
	Vector_CheckType(LUA, 2);
    LUA_Angle* ang = Angle_Get(LUA, 1);
    LUA_Vector* vec = Vector_Get(LUA, 2);
    double number = LUA->CheckNumber(3);

    double radians = DEG2RAD(number);

    matrix3x4_t matrix;
    AngleMatrix(QAngle(ang->x, ang->y, ang->z), matrix);

    Vector rotated;
    VectorIRotate(&vec->x, matrix, &rotated.x);

    Quaternion rotation;
    AxisAngleQuaternion(rotated, radians, rotation);

    matrix3x4_t matrix2;
    QuaternionMatrix(rotation, matrix2);

    QAngle new_ang;
    MatrixAngles(matrix2, new_ang);

    ang->x = new_ang.x;
    ang->y = new_ang.y;
    ang->z = new_ang.z;

	return 0;
}

LUA_FUNCTION(Angle_Normalize)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	ang->x = AngleNormalize(ang->x);
	ang->y = AngleNormalize(ang->y);
	ang->z = AngleNormalize(ang->z);

	return 0;
}

LUA_FUNCTION(Angle_Set)
{
	Angle_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	LUA_Angle* ang1 = Angle_Get(LUA, 1);
	LUA_Angle* ang2 = Angle_Get(LUA, 2);

	ang1->x = ang2->x;
	ang1->y = ang2->y;
	ang1->z = ang2->z;

	return 0;
}

LUA_FUNCTION(Angle_Zero)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	ang->x = 0;
	ang->y = 0;
	ang->z = 0;

	return 0;
}

LUA_FUNCTION(Angle_IsZero)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	LUA->PushBool(ang->x == 0 && ang->y == 0 && ang->z == 0);

	return 1;
}

LUA_FUNCTION(Angle_Unpack)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);

	LUA->PushNumber(ang->x);
	LUA->PushNumber(ang->y);
	LUA->PushNumber(ang->z);

	return 3;
}

LUA_FUNCTION(Angle_Random)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);
	int min = LUA->IsType(2, Type::Number) ? LUA->GetNumber(2) : -360;
	int max = LUA->IsType(3, Type::Number) ? LUA->GetNumber(3) : 360;

	ang->x = std::rand() % max + min;
	ang->y = std::rand() % max + min;
	ang->z = std::rand() % max + min;

	return 0;
}

LUA_FUNCTION(Angle_SetUnpacked)
{
	Angle_CheckType(LUA, 1);
	LUA_Angle* ang = Angle_Get(LUA, 1);
	double x = LUA->CheckNumber(2);
	double y = LUA->CheckNumber(3);
	double z = LUA->CheckNumber(4);

	ang->x = x;
	ang->y = y;
	ang->z = z;

	return 0;
}

LUA_FUNCTION(Angle_IsEqualTol)
{
	Angle_CheckType(LUA, 1);
	Angle_CheckType(LUA, 2);
	LUA_Angle* ang1 = Angle_Get(LUA, 1);
	LUA_Angle* ang2 = Angle_Get(LUA, 2);
	double tolerance = LUA->CheckNumber(3);

	LUA->PushBool(
		(abs(ang1->x - ang2->x) <= tolerance) &&
        (abs(ang1->y - ang2->y) <= tolerance) &&
        (abs(ang1->z - ang2->z) <= tolerance)
	);

	return 1;
}

LUA_FUNCTION(Global_Angle)
{
	double x = LUA->CheckNumber(1);
	double y = LUA->CheckNumber(2);
	double z = LUA->CheckNumber(3);

	Push_Angle(LUA, x, y, z);

	return 1;
}

LUA_FUNCTION(Global_LerpAngle)
{
	double delta = LUA->GetNumber(1);
	LUA_Angle* start = Angle_Get(LUA, 2);
	LUA_Angle* end = Angle_Get(LUA, 3);

	Push_Angle(LUA, Lerp(delta, start->x, end->x), Lerp(delta, start->y, end->y), Lerp(delta, start->z, end->z));

	return 1;
}

void InitAngleClass(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	// ToDo: Look into what Get_AngleIndex does in Gmod
	LUA->CreateMetaTableType(metaname, metatype);
		Add_Func(LUA, Angle__gc, "__gc"); // Gmod doesn't have __gc
		Add_Func(LUA, Angle__index, "__index");
		Add_Func(LUA, Angle__newindex, "__newindex");
		Add_Func(LUA, Angle__tostring, "__tostring");
		Add_Func(LUA, Angle__eq, "__eq");
		Add_Func(LUA, Angle__add, "__add");
		Add_Func(LUA, Angle__sub, "__sub");
		Add_Func(LUA, Angle__unm, "__unm");
		Add_Func(LUA, Angle__mul, "__mul");
		Add_Func(LUA, Angle__div, "__div");

		Add_Func(LUA, Angle_Add, "Add");
		Add_Func(LUA, Angle_Sub, "Sub");
		Add_Func(LUA, Angle_Mul, "Mul");
		Add_Func(LUA, Angle_Div, "Div");
		Add_Func(LUA, Angle_Forward, "Forward");
		Add_Func(LUA, Angle_Right, "Right");
		Add_Func(LUA, Angle_RotateAroundAxis, "RotateAroundAxis");
		Add_Func(LUA, Angle_Normalize, "Normalize");
		Add_Func(LUA, Angle_Set, "Set");
		Add_Func(LUA, Angle_Zero, "Zero");
		Add_Func(LUA, Angle_IsZero, "IsZero");
		Add_Func(LUA, Angle_Unpack, "Unpack");
		Add_Func(LUA, Angle_Random, "Random");
		Add_Func(LUA, Angle_SetUnpacked, "SetUnpacked");
		Add_Func(LUA, Angle_IsEqualTol, "IsEqualTol");
	LUA->Pop(1);
}