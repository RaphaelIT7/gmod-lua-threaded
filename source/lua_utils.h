#include <GarrysMod/Lua/LuaInterface.h>
#include <unordered_map>
#include "detours.h"
#include <setjmp.h>
#include <vector>
#include <lua.h>

using namespace GarrysMod::Lua;

enum LuaAction
{
	RunString,
	InitClasses,
	InitLibraries,
	LoadFunc,
	Autorun,
	RunFile,
};

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
	LuaAction type;
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

	jmp_buf jumpBuffer;
};

struct GMOD_Info
{
	int version;
	const char* versionstr;
	const char* branch;
};

extern GMOD_Info* GMOD;
extern int interfaces_count;
extern std::unordered_map<double, ILuaThread*> interfaces;

extern int shared_table_reference;
extern CThreadFastMutex shared_table_mutex;
extern std::unordered_map<std::string, ILuaValue*> shared_table;

extern void PushValue(ILuaBase*, ILuaValue*);
extern void SafeDelete(ILuaValue*);
extern ILuaValue* GetOrCreate(std::string);
extern void FillValue(ILuaBase*, ILuaValue*, int, int);

extern void Add_Func(GarrysMod::Lua::ILuaBase*, CFunc, const char*);
extern ILuaThread* FindThread(int);

extern ILuaInterface* CreateInterface();
extern void ShutdownInterface(ILuaThread*);