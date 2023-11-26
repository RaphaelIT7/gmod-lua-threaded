#include <GarrysMod/Lua/LuaInterface.h>
#include "lua_utils.h" // We need to include utils here because of ILuaThread. ToDo: Look into it and maybe change it later.
#include <cstdint>

using namespace GarrysMod::Lua;

struct LUA_ILuaInterface
{
	ILuaInterface* IFace;
	int ID;
};

static int32_t metatype = GarrysMod::Lua::Type::NONE;
static const char metaname[] = "ILuaInterface";
static const char invalid_error[] = "invalid ILuaInterface";
static const char table_name[] = "ILuaInterface_object";

extern void CheckType(ILuaBase*, int);
extern LUA_ILuaInterface* GetUserdata(ILuaBase*, int);
extern ILuaInterface* Get(ILuaBase*, int);
extern void Push(ILuaBase*, ILuaInterface*, int);
extern void Destroy(ILuaBase*, int);

extern unsigned LuaThread(void*);
extern void InitMetaTable(ILuaInterface*);

extern ILuaThread* GetValidThread(ILuaBase*, double);

extern void RunFile(ILuaThread*, const char*);