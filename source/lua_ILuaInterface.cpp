#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include "lua_threaded.h"
#include <sstream>

static int32_t metatype = GarrysMod::Lua::Type::NONE;
static const char metaname[] = "ILuaInterface";
static const char invalid_error[] = "invalid ILuaInterface";
static const char table_name[] = "ILuaInterface_object";

GMOD_Info* GMOD;
void ILuaInterface_CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, metatype))
		luaL_typerror(LUA->GetState(), index, metaname);
}

LUA_ILuaInterface *ILuaInterface_GetUserdata(ILuaBase *LUA, int index)
{
	return LUA->GetUserType<LUA_ILuaInterface>(index, metatype);
}

ILuaInterface* ILuaInterface_Get(ILuaBase* LUA, int index)
{
	ILuaInterface_CheckType(LUA, index);

	LUA_ILuaInterface *udata = ILuaInterface_GetUserdata(LUA, index);
	if(udata == nullptr)
		LUA->ArgError(index, invalid_error);

	return udata->IFace;
}

void ILuaInterface_Push(ILuaBase* LUA, ILuaInterface* Interface, int ID)
{
	if(Interface == nullptr)
	{
		LUA->PushNil();
		return;
	}

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(Interface);
	LUA->GetTable(-2);
	if(LUA->IsType(-1, metatype))
	{
		LUA->Remove(-2);
		return;
	}

	LUA->Pop(1);

	LUA_ILuaInterface *udata = LUA->NewUserType<LUA_ILuaInterface>(metatype);
	udata->IFace = Interface;
	udata->ID = ID;

	LUA->PushMetaTable(metatype);
	LUA->SetMetaTable(-2);

	LUA->CreateTable();
	LUA->SetFEnv(-2);

	LUA->PushUserdata(Interface);
	LUA->Push(-2);
	LUA->SetTable(-4);
	LUA->Remove(-2);
}

void ILuaInterface_Destroy(ILuaBase *LUA, int index)
{
	LUA_ILuaInterface *udata = ILuaInterface_GetUserdata(LUA, index);
	if (udata == nullptr)
		return;

	ILuaInterface* IFace = udata->IFace;

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(IFace);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

ILuaThread* GetValidThread(ILuaBase* LUA, double index)
{
	if (ThreadInMainThread())
	{
		LUA_ILuaInterface* IData = ILuaInterface_GetUserdata(LUA, index);

		ILuaThread* thread = FindThread(IData->ID);
		if (!thread)
		{
			LUA->ThrowError("Invalid ILuaInterface!");
		}

		return thread;
	} else {
		LUA->PushSpecial(SPECIAL_GLOB);
			LUA->GetField(-1, "__InterfaceID");
			double id = LUA->GetNumber(-1);
		LUA->Pop(2);

		return FindThread(id);
	}
}

LUA_FUNCTION_STATIC(gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	ILuaInterface_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(tostring)
{
	LUA->PushFormattedString("%s: %p", metaname, ILuaInterface_Get(LUA, 1));
	return 1;
}

LUA_FUNCTION_STATIC(eq)
{
	LUA->PushBool(ILuaInterface_Get(LUA, 1) == ILuaInterface_Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(index)
{
	LUA->GetMetaTable(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	if(!LUA->IsType(-1, GarrysMod::Lua::Type::NIL))
		return 1;

	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	return 1;
}

LUA_FUNCTION_STATIC(newindex)
{
	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);
	return 0;
}

std::string generateStackTrace(lua_State* L) {
	std::stringstream trace;
	int level = 0;
	lua_Debug ar;

	func_lua_getstack(L, 0, &ar);
	func_lua_getinfo(L, "nSl", &ar);

	trace << "Error in: " << ar.source << ":" << ar.currentline << " [" << (ar.name ? ar.name : "main") << "]" << std::endl;

	while (func_lua_getstack(L, level, &ar)) {
		func_lua_getinfo(L, "nSl", &ar);

		for (int i=0; i<level; ++i) {
			trace << "  ";
		}

		if (ar.name != NULL) {
			trace << "  " << level << ". " << ar.name << " - [" << ar.source << "]:" << ar.currentline << std::endl;
		} else {
			trace << "  " << level << ". unknown - [" << ar.source << "]:" << ar.currentline << std::endl;
		}

		level++;
	}

	return trace.str();
}

void HandleError(ILuaInterface* LUA, int result, const char* pFile)
{
	if (result != 0)
	{
		std::string stack = generateStackTrace(LUA->GetState());
		const char* err = func_lua_tostring(LUA->GetState(), -1, NULL);
		LUA->Pop();

		Msg("[ERROR] ILuaInterface:RunString: %s\n%s", err, stack.c_str());
		return;
	}
}

void RunString(ILuaThread* thread, const char* str, const char* pFile)
{
	ILuaInterface* LUA = thread->IFace;
	if (setjmp(thread->jumpBuffer) == 0)
    {
		int result = func_luaL_loadbuffer(LUA->GetState(), str, strlen(str), pFile);
		HandleError(LUA, result, pFile);
	}
    else
    {
		HandleError(LUA, -1, pFile); // Could crash if the Lua Panic wan't created by pcall or loadstring.
    }

	if (setjmp(thread->jumpBuffer) == 0)
    {
		LUA->CallFunctionProtected(0, 0, true); // ToDo: Document that the third argument is showError
		//int result = func_lua_pcall(LUA->GetState(), 0, 0, 0);
		//HandleError(LUA, result, pFile);
	}
    else
    {
		HandleError(LUA, -1, pFile); // Could crash if the Lua Panic wan't created by pcall or loadstring.
    }
}

LUA_FUNCTION(ILuaInterface_RunString)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* str = LUA->CheckString(2);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_RunString;
		action->data = str;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		RunString(thread, str, "RunString");
	}

	return 0;
}

void InitClasses(ILuaInterface* LUA)
{
	InitVectorClass(LUA);
	InitAngleClass(LUA);
	InitFileClass(LUA);

	//func_InitLuaClasses(LUA);
}

LUA_FUNCTION(ILuaInterface_InitClasses)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_InitClasses;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		InitClasses(thread->IFace);
	}

	return 0;
}

void* g_pGlobalLuaLibraryFactorySig;
void InitLuaLibraries(ILuaThread* thread)
{
	Msg("InitLuaLibraries called\n");

	ILuaInterface* LUA = thread->IFace;

	InitGlobal(LUA);
	InitNet(LUA);
	InitGameevent(thread);
	InitPhysEnv(LUA);
	InitResource(LUA);
	InitSql(LUA);
	InitSystem(LUA);
	InitEngine(LUA);
	InitTimer(LUA);
	InitHammer(LUA);
	InitGmodLib(LUA);
	InitFileLib(LUA);
}

LUA_FUNCTION(ILuaInterface_InitLibraries)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_InitLibraries;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		InitLuaLibraries(thread);
	}

	return 0;
}

void InitLuaEnums(ILuaInterface* LUA)
{
	PushEnums(LUA);
}

LUA_FUNCTION(ILuaInterface_InitEnums)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_InitEnums;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		InitEnums(thread->IFace);
	}

	return 0;
}

void InitGmod(ILuaThread* thread)
{
	InitLuaEnums(thread->IFace);

	InitLuaLibraries(thread);

	InitClasses(thread->IFace);
}

LUA_FUNCTION(ILuaInterface_InitGmod)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_InitGmod;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		InitGmod(thread);
	}

	return 0;
}

void RunFile_Intern(ILuaThread* LUA, const char* file, FileHandle_t fh)
{
	int file_len = gpFileSystem->Size(fh);
	char* code = new char[file_len + 1];

	gpFileSystem->Read((void*)code, file_len, fh);
	code[file_len] = 0;

	gpFileSystem->Close(fh);

	RunString(LUA, code, file);

	delete[] code;
}

void RunFile(ILuaThread* LUA, const char* file, const char* called)
{
	std::string path = file;
	if (path.substr(0, 4) != "lua/")
		path = "lua/" + path;

	FileHandle_t fh = gpFileSystem->Open(path.c_str(), "r", "GAME");
	if(fh)
	{
		RunFile_Intern(LUA, path.c_str(), fh);
	} else {
		std::string path2 = ToPath(called);
		if (path2.substr(0, 4) != "lua/")
			path2 = "lua/" + path2;

		path2 = path2 + file;

		fh = gpFileSystem->Open(path2.c_str(), "r", "GAME");
		if(fh)
		{
			RunFile_Intern(LUA, path2.c_str(), fh);
		} else {
			LUA->IFace->ThrowError(((std::string)"Failed to find " + file).c_str());
		}
	}
}

void Autorun(ILuaThread* LUA)
{
	RunFile(LUA, "includes/init.lua");

	FileFindHandle_t findHandle;
	const char *pFilename = gpFileSystem->FindFirstEx("lua/autorun/*.lua", "GAME", &findHandle);
	while (pFilename)
	{
		RunFile(LUA, (((std::string)"autorun/") + pFilename).c_str()); // Kill me later.

		pFilename = gpFileSystem->FindNext(findHandle);
	}

	pFilename = gpFileSystem->FindFirstEx("lua/autorun/server/*.lua", "GAME", &findHandle);
	while (pFilename)
	{
		RunFile(LUA, (((std::string)"autorun/server/") + pFilename).c_str()); // Kill me later.

		pFilename = gpFileSystem->FindNext(findHandle);
	}

	/*
		Load sensorbones. Yes. Gmod also loads them.
	*/
	pFilename = gpFileSystem->FindFirstEx("lua/autorun/server/sensorbones/*.lua", "GAME", &findHandle);
	while (pFilename)
	{
		RunFile(LUA, (((std::string)"autorun/server/sensorbones/") + pFilename).c_str()); // Kill me later.

		pFilename = gpFileSystem->FindNext(findHandle);
	}
	gpFileSystem->FindClose(findHandle);
}

LUA_FUNCTION(ILuaInterface_Autorun)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_Autorun;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		Autorun(thread);
	}

	return 0;
}


LUA_FUNCTION(ILuaInterface_RunFile)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* str = LUA->CheckString(2);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_RunFile;
		action->data = str;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		RunFile(thread, str);
	}

	return 0;
}

void LoadFunction(ILuaInterface* LUA, const char* func)
{
	void* loaded_func = FindSymbol((std::string)func);

	if (loaded_func == nullptr)
	{
		Msg("Failed to find function %s\n", func);
	} else {
		LUA->PushSpecial(SPECIAL_GLOB);
			LUA->PushCFunction((CFunc)loaded_func);
			LUA->SetField(-2, func);
		LUA->Pop();
	}
}

LUA_FUNCTION(ILuaInterface_LoadFunction)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* func = LUA->CheckString(2);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_LoadFunc;
		action->data = func;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		LoadFunction(thread->IFace, func);
	}

	return 0;
}

LUA_FUNCTION(ILuaInterface_Lock)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		thread->locked = true;
		while (thread->run && !thread->waiting)
		{
			ThreadSleep(1);
		}
	}

	return 0;
}

LUA_FUNCTION(ILuaInterface_Unlock)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		thread->locked = false;
	}

	return 0;
}

void RunHook(ILuaInterface* LUA, const char* name, ILuaValue* args)
{
	LUA->PushSpecial(SPECIAL_GLOB);
	LUA->GetField(-1, "hook");
	if (LUA->IsType(-1, Type::Table))
	{
		LUA->GetField(-1, "Run");
		if (LUA->IsType(-1, Type::Function))
		{
			LUA->PushString(name);
			int pushed = 1;
			if (args->type != Type::Table)
			{
				for(auto&[key, val] : args->tbl)
				{
					++pushed;
					PushValue(LUA, val);
				}
			} else {
				++pushed;
				PushValue(LUA, args);
			}
			Msg("Args: %i %i\n", pushed, (int)args->number);
			LUA->CallFunctionProtected(pushed, 0, true);
			SafeDelete(args);
		} else {
			SafeDelete(args);
			LUA->ThrowError("hook.Run is missing or not a function!");
		}
	} else {
		SafeDelete(args);
		LUA->ThrowError("hook table is missing or not a table!");
	}
}

LUA_FUNCTION(ILuaInterface_RunHook)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(2);

	ILuaValue* hook_tbl = new ILuaValue;
	hook_tbl->type = Type::Nil;
	hook_tbl->number = LUA->Top() - 2;
	Msg("RunHook: %i\n", LUA->Top());

	if (hook_tbl->number > 0)
	{
		for(int pos = 3; pos < hook_tbl->number; ++pos)
		{
			ILuaValue* val = new ILuaValue;
			FillValue(LUA, val, pos, LUA->GetType(pos));

			hook_tbl->tbl[CreateValue(std::to_string(pos - 2).c_str())] = val;
		}
	}

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = LuaAction::ACT_RunHook;
		action->data = name;
		action->val = hook_tbl;

		Msg("RunHook 2: %i\n", (int)hook_tbl->number);
		thread->mutex.Lock(); // ToDo: Why does mutex.Lock crash HERE
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		RunHook(thread->IFace, name, hook_tbl);
	}

	return 0;
}

void RunCommand(ILuaInterface* LUA, const CCommand& cmd, void* ply)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "concommand");
		if (LUA->IsType(-1, Type::Table)) {
			LUA->GetField(-1, "Run");
				if (LUA->IsType(-1, Type::Function)) {
					/*
						concommand.Run:
						1. Player ply
						2. string cmd
						3. any arguments
						4. string argumentstring
					*/

					ILuaObject* obj = LUA->CreateObject();
					obj->SetEntity((BaseEntity*)ply);
					obj->Push();

					LUA->PushString(cmd.Arg(0));

					if (cmd.ArgC() == 1)
					{
						LUA->PushNil();
					} else if (cmd.ArgC() == 2) {
						LUA->PushString(cmd.Arg(1));
					} else {
						LUA->CreateTable();

						for(int i=1;i!=cmd.ArgC();i++) {
							LUA->PushString(cmd.Arg(i));
							LUA->SetField(-2, std::to_string(i).c_str());
						}
					}

					LUA->PushString(cmd.ArgS());
				} else {
					Msg("concommand.Run is not a function!\n");
				}

				LUA->Pop(1);
		} else {
			Msg("concommand is not a table!\n");
		}

	LUA->Pop(2); // Global, concommand, concommand.run
}

unsigned LuaThread(void* data)
{
	ILuaThread* thread_data = (ILuaThread*)data;
	ILuaInterface* IFace = CreateInterface();
	thread_data->IFace = IFace;
	InitLuaThreaded(IFace, thread_data->id);
	InitMetaTable(IFace);
	thread_data->init = true;

	while(thread_data->run)
	{
		if (!GMOD->threadready)
		{
			ThreadSleep(0.5);
			continue;
		}

		if (thread_data->locked)
		{
			thread_data->waiting = true;
			while (thread_data->locked) {
				ThreadSleep(0.5);
			}
			thread_data->waiting = false;
		}

		thread_data->mutex.Lock();

		for (ILuaAction* action : thread_data->actions)
		{
			if (action->type == LuaAction::ACT_RunString)
			{
				RunString(thread_data, action->data, "RunString");
			} else if (action->type == LuaAction::ACT_InitClasses)
			{
				InitClasses(IFace);
			} else if (action->type == LuaAction::ACT_InitLibraries)
			{
				InitLuaLibraries(thread_data);
			} else if (action->type == LuaAction::ACT_LoadFunc)
			{
				LoadFunction(IFace, action->data);
			} else if (action->type == LuaAction::ACT_Autorun)
			{
				Autorun(thread_data);
			} else if (action->type == LuaAction::ACT_RunFile)
			{
				RunFile(thread_data, action->data);
			} else if (action->type == LuaAction::ACT_InitEnums)
			{
				InitEnums(IFace);
			} else if (action->type == LuaAction::ACT_InitGmod)
			{
				InitGmod(thread_data);
			} else if (action->type == LuaAction::ACT_RunCommand)
			{
				RunCommand(IFace, action->cmd, action->ply);
			} else if (action->type == LuaAction::ACT_Gameevent)
			{
				RunHook(IFace, action->data, action->val);
			} else if (action->type == LuaAction::ACT_RunHook)
			{
				RunHook(IFace, action->data, action->val);
			}

			delete action;
		}

		thread_data->actions.clear();

		TimerThink(thread_data); // Check & Runs all Timers. Done in Gmod by GarrysMod::Lua::Libraries::Timer::Cycle
		FileLibThink(thread_data); // Checks & Runs all file.AsyncRead callbacks.

		thread_data->mutex.Unlock();

		ThreadSleep(1); // Sleep 1 ms
	}

	//ShutdowInterface(thread_data); // ToDo: Fix the crash

	return 0;
}

void InitMetaTable(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	metatype = LUA->CreateMetaTable(metaname);
		Add_Func(LUA, gc, "__gc");
		Add_Func(LUA, tostring, "__tostring");
		Add_Func(LUA, eq, "__eq");
		Add_Func(LUA, index, "__index");
		Add_Func(LUA, newindex, "__newindex");

		Add_Func(LUA, ILuaInterface_RunHook, "RunHook");
		Add_Func(LUA, ILuaInterface_RunFile, "RunFile");
		Add_Func(LUA, ILuaInterface_RunString, "RunString");
		Add_Func(LUA, ILuaInterface_InitClasses, "InitClasses");
		Add_Func(LUA, ILuaInterface_InitLibraries, "InitLibraries");
		Add_Func(LUA, ILuaInterface_LoadFunction, "LoadFunction");
		Add_Func(LUA, ILuaInterface_Autorun, "Autorun");
		Add_Func(LUA, ILuaInterface_InitEnums, "InitEnums");
		Add_Func(LUA, ILuaInterface_InitGmod, "InitGmod");
		Add_Func(LUA, ILuaInterface_Lock, "Lock");
		Add_Func(LUA, ILuaInterface_Unlock, "Unlock");

	LUA->Pop(1);
}

void DestroyMetaTable(ILuaInterface* LUA)
{
	LUA->PushNil();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, metaname);
}