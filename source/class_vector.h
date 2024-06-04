#include <GarrysMod/Lua/LuaInterface.h>

struct LUA_Vector
{
	float x;
	float y;
	float z;
};

extern void InitVectorClass(GarrysMod::Lua::ILuaInterface*);
extern int Global_Vector(lua_State*);
extern int Global_LerpVector(lua_State*);
extern int Global_OrderVectors(lua_State*);

extern void Push_Vector(GarrysMod::Lua::ILuaBase* LUA, Vector vec);
extern LUA_Vector* Vector_Get(GarrysMod::Lua::ILuaBase* LUA, int index);
extern void Vector_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);