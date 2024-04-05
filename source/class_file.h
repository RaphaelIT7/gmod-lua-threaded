#include <GarrysMod/Lua/LuaInterface.h>

struct LUA_File
{
	const char* filename;
	FileHandle_t handle;
};

extern void InitFileClass(GarrysMod::Lua::ILuaInterface*);

extern void Push_File(GarrysMod::Lua::ILuaBase* LUA, const char* filename, const char* fileMode, const char* path);
extern bool IsFile(GarrysMod::Lua::ILuaBase* LUA, int index);
extern LUA_File* File_Get(GarrysMod::Lua::ILuaBase* LUA, int index, bool error = true);
extern void File_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index);