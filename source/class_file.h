#include <GarrysMod/Lua/LuaInterface.h>

using namespace GarrysMod::Lua;

struct LUA_File
{
	const char* filename;
	FileHandle_t handle;
};

extern void InitFileClass(ILuaInterface*);

extern void Push_File(ILuaBase* LUA, const char* filename, const char* fileMode, const char* path);
extern bool IsFile(ILuaBase* LUA, int index);
extern LUA_File* File_Get(ILuaBase* LUA, int index, bool error = true);
extern void File_CheckType(ILuaBase* LUA, int index);