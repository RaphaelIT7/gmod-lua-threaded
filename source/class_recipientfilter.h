#include <GarrysMod/Lua/LuaInterface.h>

struct LUA_RecipientFilter
{
	CRecipientFilter* filter;
	bool shoulddelete;
};

extern void InitRecipientFilterClass(GarrysMod::Lua::ILuaInterface*);
extern int Global_RecipientFilter(lua_State*);

extern void Push_RecipientFilter(GarrysMod::Lua::ILuaBase* LUA, CRecipientFilter* vec, bool shoulddelete = true);
extern LUA_RecipientFilter* RecipientFilter_Get(GarrysMod::Lua::ILuaBase* LUA, int index);
extern void RecipientFilter_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);