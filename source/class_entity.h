#include <GarrysMod/Lua/LuaInterface.h>

using namespace GarrysMod::Lua;

struct LUA_Entity
{
	char type[];
	char classname[];
};

extern void InitAngleClass(ILuaInterface*);
extern int Global_Entity(lua_State*);

extern void Push_Entity(ILuaBase* LUA, char type[], char classname[]);
extern bool IsEntity(ILuaBase* LUA, int index);