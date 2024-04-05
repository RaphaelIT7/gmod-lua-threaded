#include <GarrysMod/Lua/LuaInterface.h>
/* #include <entitylist_base.h>
#include <baseentity.h> */
struct LUA_Entity
{
	const char* type;
	const char* classname;
};

extern void InitEntityClass(GarrysMod::Lua::ILuaInterface*);
extern int Global_Entity(lua_State*);

extern void Push_Entity(GarrysMod::Lua::ILuaBase* LUA, const char* type, const char* classname);
extern bool IsEntity(GarrysMod::Lua::ILuaBase* LUA, int index);
extern LUA_Entity* Entity_Get(GarrysMod::Lua::ILuaBase* LUA, int index);
extern void Entity_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);