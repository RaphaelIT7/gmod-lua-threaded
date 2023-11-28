#include <GarrysMod/Lua/Interface.h>
#include <unordered_map>
#include "ILuaConVars.h"
#include "detours.h"
#include <setjmp.h>
#include <vector>
#include <lua.h>

using namespace GarrysMod::Lua;

enum LuaAction
{
	ACT_RunString,
	ACT_InitClasses,
	ACT_InitLibraries,
	ACT_LoadFunc,
	ACT_Autorun,
	ACT_RunFile,
	ACT_InitEnums,
	ACT_InitGmod,
	ACT_RunCommand,
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

	CCommand cmd;
	void* ply;
};

struct ILuaThread
{
	ILuaInterface* IFace;
	CThreadFastMutex mutex;
	int shared_tbl_reference = -1;

	std::vector<ILuaAction*> actions;

	bool run = true;
	bool init = false;
	bool threaded = false;
	
	int id = -1;

	std::string current_path;

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

extern std::string ToPath(std::string);