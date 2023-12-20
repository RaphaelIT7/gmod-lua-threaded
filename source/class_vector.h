#include <GarrysMod/Lua/LuaInterface.h>

using namespace GarrysMod::Lua;

extern void InitVectorClass(ILuaInterface*);
extern int _Vector(lua_State*);

extern void Push_Vector(ILuaBase* LUA, Vector vec);
extern Vector& Vector_Get(ILuaBase* LUA, int index);