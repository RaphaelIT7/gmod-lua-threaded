#include <GarrysMod/Lua/Interface.h>
#include "lua_threaded.h"

void CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, metatype))
		luaL_typerror(LUA->GetState(), index, metaname);
}

LUA_ILuaInterface *GetUserdata(ILuaBase *LUA, int index)
{
	return LUA->GetUserType<LUA_ILuaInterface>(index, metatype);
}

ILuaInterface* Get(ILuaBase* LUA, int index)
{
	CheckType(LUA, index);

	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if(udata == nullptr)
		LUA->ArgError(index, invalid_error);

	return udata->IFace;
}

void Push(ILuaBase* LUA, ILuaInterface* Interface, int ID)
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

void Destroy(ILuaBase *LUA, int index)
{
	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
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
		LUA_ILuaInterface* IData = GetUserdata(LUA, index);

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

	Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(tostring)
{
	LUA->PushFormattedString("%s: %p", metaname, Get(LUA, 1));
	return 1;
}

LUA_FUNCTION_STATIC(eq)
{
	LUA->PushBool(Get(LUA, 1) == Get(LUA, 2));
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

void HandleError(ILuaInterface* LUA, int result)
{
	if (result != 0)
	{
		const char* err = func_lua_tostring(LUA->GetState(), -1, NULL);
		LUA->Pop();

		Msg("[ERROR] ILuaInterface:RunString: %s\n", err);
		return;
	}
}

void RunString(ILuaThread* thread, const char* str)
{
	ILuaInterface* LUA = thread->IFace;
	if (setjmp(thread->jumpBuffer) == 0)
    {
		int result = func_luaL_loadstring(LUA->GetState(), str);
		HandleError(LUA, result);

		result = func_lua_pcall(LUA->GetState(), 0, 0, 0);
		HandleError(LUA, result);
	}
    else
    {
		HandleError(LUA, -1); // Could crash if the Lua Panic wan't created by pcall or loadstring.
    }
}

LUA_FUNCTION(ILuaInterface_RunString)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* str = LUA->CheckString(2);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "run";
		action->data = str;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		RunString(thread, str);
	}

	return 0;
}

LUA_FUNCTION(ILuaInterface_InitClasses)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "initclasses";

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		func_InitLuaClasses(thread->IFace);
	}

	return 0;
}

void* g_pGlobalLuaLibraryFactorySig;
void InitLuaLibraries(ILuaInterface* LUA)
{
	Msg("InitLuaLibraries called\n");
	func_CLuaGlobalLibrary_InitLibraries(g_pGlobalLuaLibraryFactory, LUA);

	func_InitLuaLibraries(LUA);
}

LUA_FUNCTION(ILuaInterface_InitLibraries)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "initlibraries";

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		InitLuaLibraries(thread->IFace);
	}

	return 0;
}

void RunFile(ILuaThread* LUA, const char* file)
{
	FileHandle_t fh = gpFileSystem->Open(file, "r", "LUA");
	if(fh)
	{
		int file_len = gpFileSystem->Size(fh);
		char* code = new char[file_len + 1];

		gpFileSystem->Read((void*)code,file_len,fh);
		code[file_len] = 0;

		gpFileSystem->Close(fh);

		RunString(LUA, code);

		delete[] code;
	} else {
		Msg("Failed to find %s!\n", file);
	}
}

void Autorun(ILuaThread* LUA)
{
	RunFile(LUA, "includes/init.lua");

	/*FileFindHandle_t findHandle;
	const char *pFilename = gpFileSystem->FindFirstEx("autorun/*.lua", "LUA", &findHandle);
	while (pFilename)
	{
		RunFile(); // ToDo

		pFilename = gpFileSystem->FindNext(findHandle);
	}
	gpFileSystem->FindClose(findHandle);*/
}

LUA_FUNCTION(ILuaInterface_Autorun)
{
	ILuaThread* thread = GetValidThread(LUA, 1);

	if (thread->threaded)
	{
		ILuaAction* action = new ILuaAction;
		action->type = "autorun";

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
		action->type = "runfile";
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
		action->type = "loadfunc";
		action->data = func;

		thread->mutex.Lock();
		thread->actions.push_back(action);
		thread->mutex.Unlock();
	} else {
		LoadFunction(thread->IFace, func);
	}

	return 0;
}

unsigned LuaThread(void* data)
{
	ILuaThread* thread_data = (ILuaThread*)data;
	ILuaInterface* IFace = CreateInterface();
	thread_data->IFace = IFace;
	InitLuaThreaded(IFace, thread_data->id);
	InitMetaTable(IFace);

	while(thread_data->run)
	{
		thread_data->mutex.Lock();

		for (ILuaAction* action : thread_data->actions)
		{
			if (strcmp(action->type, "run") == 0)
			{
				RunString(thread_data, action->data);
			} else if (strcmp(action->type, "initclasses") == 0)
			{
				func_InitLuaClasses(IFace);
			} else if (strcmp(action->type, "initlibraries") == 0)
			{
				InitLuaLibraries(IFace);
			} else if (strcmp(action->type, "loadfunc") == 0)
			{
				LoadFunction(IFace, action->data);
			} else if (strcmp(action->type, "autorun") == 0)
			{
				Autorun(thread_data);
			} else if (strcmp(action->type, "runfile") == 0)
			{
				Autorun(thread_data);
			}

			delete action;
		}

		thread_data->actions.clear();

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

	LUA->PushCFunction(gc);
	LUA->SetField(-2, "__gc");

	LUA->PushCFunction(tostring);
	LUA->SetField(-2, "__tostring");

	LUA->PushCFunction(eq);
	LUA->SetField(-2, "__eq");

	LUA->PushCFunction(index);
	LUA->SetField(-2, "__index");

	LUA->PushCFunction(newindex);
	LUA->SetField(-2, "__newindex");

	LUA->PushCFunction(ILuaInterface_RunString);
	LUA->SetField(-2, "RunString");

	LUA->PushCFunction(ILuaInterface_InitClasses);
	LUA->SetField(-2, "InitClasses");

	LUA->PushCFunction(ILuaInterface_InitLibraries);
	LUA->SetField(-2, "InitLibraries");

	LUA->PushCFunction(ILuaInterface_LoadFunction);
	LUA->SetField(-2, "LoadFunction");

	LUA->PushCFunction(ILuaInterface_Autorun);
	LUA->SetField(-2, "Autorun");

	LUA->PushCFunction(ILuaInterface_RunFile);
	LUA->SetField(-2, "RunFile");

	LUA->Pop(1);
}