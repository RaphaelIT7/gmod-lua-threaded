#include <GarrysMod/Lua/LuaInterface.h>
#include "lua_utils.h" // We need to include utils here because of ILuaThread. ToDo: Look into it and maybe change it later.
#include <cstdint>

using namespace GarrysMod::Lua;

struct LUA_ILuaInterface
{
	ILuaInterface* IFace;
	int ID;
};

extern void ILuaInterface_CheckType(ILuaBase*, int);
extern LUA_ILuaInterface* ILuaInterface_GetUserdata(ILuaBase*, int);
extern ILuaInterface* ILuaInterface_Get(ILuaBase*, int);
extern void ILuaInterface_Push(ILuaBase*, ILuaInterface*, int);
extern void ILuaInterface_Destroy(ILuaBase*, int);

extern unsigned LuaThread(void*);
extern void InitMetaTable(ILuaInterface*);
extern void DestroyMetaTable(ILuaInterface*);

extern ILuaThread* GetValidThread(ILuaBase*, double);

extern void RunFile(ILuaThread*, const char*);

extern void RunCommand(ILuaInterface*, const CCommand&, void*);