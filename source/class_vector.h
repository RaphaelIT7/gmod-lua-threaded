#include <GarrysMod/Lua/LuaInterface.h>

using namespace GarrysMod::Lua;

extern void InitVectorClass(ILuaInterface*);
extern int Global_Vector(lua_State*);
extern int Global_LerpVector(lua_State*);
extern int Global_OrderVectors(lua_State*);

extern void Push_Vector(ILuaBase* LUA, Vector& vec);
extern Vector& Vector_Get(ILuaBase* LUA, int index);
extern void Vector_CheckType(ILuaBase* LUA, int index);