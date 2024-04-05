#include <GarrysMod/Lua/LuaInterface.h>
/* #include <entitylist_base.h>
#include <baseentity.h> */

using namespace GarrysMod::Lua;
struct LUA_Entity
{
	const char* type;
	const char* classname;
};

extern void InitEntityClass(ILuaInterface*);
extern int Global_Entity(lua_State*);

extern void Push_Entity(ILuaBase* LUA, const char* type, const char* classname);
extern bool IsEntity(ILuaBase* LUA, int index);
extern LUA_Entity* Entity_Get(ILuaBase* LUA, int index);
extern void Entity_CheckType(ILuaBase* LUA, int index);