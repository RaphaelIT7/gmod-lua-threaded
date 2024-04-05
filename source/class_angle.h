#include <GarrysMod/Lua/LuaInterface.h>

struct LUA_Angle
{
	float x; // pitch
	float y; // yaw
	float z; // roll
};

extern void InitAngleClass(GarrysMod::Lua::ILuaInterface*);
extern int Global_Angle(lua_State*);
extern int Global_LerpAngle(lua_State*);

extern void Push_Angle(GarrysMod::Lua::ILuaBase* LUA, QAngle ang);
extern bool IsAngle(GarrysMod::Lua::ILuaBase* LUA, int index);
extern LUA_Angle* Angle_Get(GarrysMod::Lua::ILuaBase* LUA, int index);
extern void Angle_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);