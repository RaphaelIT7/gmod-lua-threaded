#include <GarrysMod/Lua/LuaInterface.h>
#include "convar.h"

struct LUA_ConVar
{
	ConVar* cvar;
};

extern void InitConVarClass(GarrysMod::Lua::ILuaInterface*);

extern void Push_ConVar(GarrysMod::Lua::ILuaBase* LUA, ConVar*);
extern LUA_ConVar* ConVar_Get(GarrysMod::Lua::ILuaBase* LUA, int index, bool error = true);
extern void ConVar_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);