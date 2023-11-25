#include <GarrysMod/Lua/LuaInterface.h>
#include <unordered_map>
#include <csetjmp>
#include <vector>
#include <lua.h>

using namespace GarrysMod::Lua;

struct ILuaValue
{
	int type = -1;

	int number = -1;
	const char* string = "";
	Vector vec;
	QAngle ang;
	std::unordered_map<std::string, ILuaValue*> tbl;
};

struct ILuaAction
{
	const char* type;
	const char* data;
};

struct ILuaThread
{
	ILuaInterface* IFace;
	CThreadFastMutex mutex;
	int shared_tbl_reference = -1;

	std::vector<ILuaAction*> actions;

	bool run = true;
	bool threaded = false;
	
	int id = -1;

	std::jmp_buf jumpBuffer;
};

extern void InitLuaThreaded(ILuaInterface*, int = 0);
extern void InitMetaTable(ILuaInterface*);
extern void PushValue(ILuaBase*, ILuaValue*);
extern void FillValue(ILuaBase*, ILuaValue*, int, int);

extern ILuaThread* FindThread(int);