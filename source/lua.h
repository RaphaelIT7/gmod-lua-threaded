#pragma once

#include <GarrysMod/Lua/LuaShared.h>
#include <unordered_map>
#include "util.h"

extern void Lua_Init(GarrysMod::Lua::ILuaBase* LUA);
extern void Server_Init();
extern void Lua_Shutdown();
extern GarrysMod::Lua::ILuaInterface* GetRealm(unsigned char);

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
};

extern void InitLuaThreaded(ILuaInterface*, int = 0);
extern void InitMetaTable(ILuaInterface*);
extern void PushValue(ILuaBase*, ILuaValue*);
extern void FillValue(ILuaBase*, ILuaValue*, int, int);

extern ILuaThread* FindThread(int);