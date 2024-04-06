#include <GarrysMod/Lua/LuaInterface.h>
struct LUA_Entity
{
	CBaseEntity* entity;
};

extern void InitEntityClass(GarrysMod::Lua::ILuaInterface*);
extern int Global_Entity(lua_State*);

extern void Push_Entity(GarrysMod::Lua::ILuaBase* LUA, CBaseEntity* ent);
extern bool IsEntity(GarrysMod::Lua::ILuaBase* LUA, int index);
extern LUA_Entity* Entity_Get(GarrysMod::Lua::ILuaBase* LUA, int index);
extern void Entity_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);