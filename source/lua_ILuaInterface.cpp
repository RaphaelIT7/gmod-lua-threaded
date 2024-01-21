#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include "lua_threaded.h"

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

void HandleError(ILuaInterface* LUA, int result, const char* pFile)
{
	if (result != 0)
	{
		const char* err = func_lua_tostring(LUA->GetState(), -1, NULL);
		LUA->Pop();

		Msg("[ERROR] ILuaInterface:RunString: %s (%s)\n", err, pFile);
		return;
	}
}

void RunString(ILuaThread* thread, const char* str, const char* pFile)
{
	ILuaInterface* LUA = thread->IFace;
	if (setjmp(thread->jumpBuffer) == 0)
    {
		int result = func_luaL_loadstring(LUA->GetState(), str);
		HandleError(LUA, result, pFile);
	}
    else
    {
		HandleError(LUA, -1, pFile); // Could crash if the Lua Panic wan't created by pcall or loadstring.
    }

	if (setjmp(thread->jumpBuffer) == 0)
    {
		int result = func_lua_pcall(LUA->GetState(), 0, 0, 0); // ToDo: Find out why it sometimes crashes :< (lj_BC_TGETS)
		HandleError(LUA, result, pFile);
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

void RunFile(ILuaThread* LUA, const char* file)
{
	std::string old_path = LUA->current_path;
	LUA->current_path = ToPath(file);

	FileHandle_t fh = gpFileSystem->Open(file, "r", "GAME");
	if(fh)
	{
		int file_len = gpFileSystem->Size(fh);
		char* code = new char[file_len + 1];

		gpFileSystem->Read((void*)code,file_len,fh);
		code[file_len] = 0;

		gpFileSystem->Close(fh);

		RunString(LUA, code, file);

		delete[] code;
	} else {
		Msg("Failed to find Path!. Next try. New path: %s\n", (old_path + file).c_str());
		LUA->current_path = ToPath(old_path + file);
		fh = gpFileSystem->Open((old_path + file).c_str(), "r", "GAME");
		if(fh)
		{
			int file_len = gpFileSystem->Size(fh);
			char* code = new char[file_len + 1];

			gpFileSystem->Read((void*)code,file_len,fh);
			code[file_len] = 0;

			gpFileSystem->Close(fh);

			RunString(LUA, code, file);

			delete[] code;
		} else {
			Msg("Failed to find %s! Try 2.\n", file);
		}
	}

	LUA->current_path = old_path;
}

void Autorun(ILuaThread* LUA)
{
	RunFile(LUA, "lua/includes/init.lua");

	FileFindHandle_t findHandle;
	const char *pFilename = gpFileSystem->FindFirstEx("lua/autorun/*.lua", "GAME", &findHandle);
	while (pFilename)
	{
		RunFile(LUA, (((std::string)"lua/autorun/") + pFilename).c_str()); // Kill me later.

		pFilename = gpFileSystem->FindNext(findHandle);
	}

	pFilename = gpFileSystem->FindFirstEx("lua/autorun/server/*.lua", "GAME", &findHandle);
	while (pFilename)
	{
		RunFile(LUA, (((std::string)"lua/autorun/server/") + pFilename).c_str()); // Kill me later.

		pFilename = gpFileSystem->FindNext(findHandle);
	}

	/*
		Load sensorbones. Yes. Gmod also loads them.
	*/
	pFilename = gpFileSystem->FindFirstEx("lua/autorun/server/sensorbones/*.lua", "GAME", &findHandle);
	while (pFilename)
	{
		RunFile(LUA, (((std::string)"lua/autorun/server/sensorbones/") + pFilename).c_str()); // Kill me later.

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
				RunGameevent(IFace, action->data, action->val);
			}

			delete action;
		}

		thread_data->actions.clear();

		TimerThink(thread_data); // Check & Run all Timers. Done in Gmod by GarrysMod::Lua::Libraries::Timer::Cycle

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

		Add_Func(LUA, ILuaInterface_RunString, "RunString");
		Add_Func(LUA, ILuaInterface_InitClasses, "InitClasses");
		Add_Func(LUA, ILuaInterface_InitLibraries, "InitLibraries");
		Add_Func(LUA, ILuaInterface_LoadFunction, "LoadFunction");
		Add_Func(LUA, ILuaInterface_Autorun, "Autorun");
		Add_Func(LUA, ILuaInterface_RunFile, "RunFile");
		Add_Func(LUA, ILuaInterface_InitEnums, "InitEnums");
		Add_Func(LUA, ILuaInterface_InitGmod, "InitGmod");

	LUA->Pop(1);
}

void DestroyMetaTable(ILuaInterface* LUA)
{
	LUA->PushNil();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, metaname);
}