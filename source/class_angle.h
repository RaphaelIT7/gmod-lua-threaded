#include <GarrysMod/Lua/LuaInterface.h>

using namespace GarrysMod::Lua;

struct LUA_Angle
{
	float x; // pitch
	float y; // yaw
	float z; // roll
};

extern void InitAngleClass(ILuaInterface*);
extern int Global_Angle(lua_State*);
extern int Global_LerpAngle(lua_State*);

extern void Push_Angle(ILuaBase* LUA, QAngle ang);
extern LUA_Angle* Angle_Get(ILuaBase* LUA, int index);
extern void Angle_CheckType(ILuaBase* LUA, int index);