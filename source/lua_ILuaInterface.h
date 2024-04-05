#include <GarrysMod/Lua/LuaInterface.h>
#include "lua_utils.h" // We need to include utils here because of ILuaThread. ToDo: Look into it and maybe change it later.
#include <cstdint>

struct LUA_ILuaInterface
{
	GarrysMod::Lua::ILuaInterface* IFace;
	int ID;
};

extern void ILuaInterface_CheckType(GarrysMod::Lua::ILuaBase*, int);
extern LUA_ILuaInterface* ILuaInterface_GetUserdata(GarrysMod::Lua::ILuaBase*, int);
extern GarrysMod::Lua::ILuaInterface* ILuaInterface_Get(GarrysMod::Lua::ILuaBase*, int);
extern void ILuaInterface_Push(GarrysMod::Lua::ILuaBase*, GarrysMod::Lua::ILuaInterface*, int);
extern void ILuaInterface_Destroy(GarrysMod::Lua::ILuaBase*, int);

extern unsigned LuaThread(void*);
extern void InitMetaTable(GarrysMod::Lua::ILuaInterface*);
extern void DestroyMetaTable(GarrysMod::Lua::ILuaInterface*);

extern ILuaThread* GetValidThread(GarrysMod::Lua::ILuaBase*, double);

extern void RunFile(ILuaThread*, const char*, const char*);
inline void RunFile(ILuaThread* LUA, const char* file) { RunFile(LUA, file, ""); };

extern void RunCommand(GarrysMod::Lua::ILuaInterface*, const CCommand&, void*);
extern void RunHook(GarrysMod::Lua::ILuaInterface*, const char*, ILuaValue*);