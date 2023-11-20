#include "lua.h"
#include "detours.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;
luaL_loadstring func_luaL_loadstring;
lua_tostring func_lua_tostring;

TInitLuaLibraries func_InitLuaLibraries;
InitLuaClasses func_InitLuaClasses;
CLuaGlobalLibrary_InitLibraries func_CLuaGlobalLibrary_InitLibraries;
void* g_pGlobalLuaLibraryFactory;

CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;
CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;
CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;
CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;
CLuaGameCallback_Msg func_CLuaGameCallback_Msg;
CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;

// Detours.
Detouring::Hook detour_InitLuaClasses;
Detouring::Hook detour_CLuaLibrary_CLuaLibrary;
Detouring::Hook detour_CLuaLibrary_Add;

void hook_InitLuaClasses(GarrysMod::Lua::ILuaInterface* LUA)
{
	detour_InitLuaClasses.GetTrampoline<InitLuaClasses>()(LUA);

	Lua_Init(LUA);
}

std::string current_library = "";
std::vector<ILuaObject*> library_funcs;
void hook_CLuaLibrary_CLuaLibrary(void* funky_class, const char* pName)
{
	current_library = pName;

	library_funcs.push_back((ILuaObject*)funky_class); // I should create the CLuaLibrary class. CLuaLibrary <- CLuaUser <- CLuaTable <- CLuaUser?

	detour_CLuaLibrary_CLuaLibrary.GetTrampoline<CLuaLibrary_CLuaLibrary>()(funky_class, pName);

	current_library = "";
}

void AddLibraries(ILuaInterface* LUA)
{
	for (ILuaObject* obj : library_funcs)
	{
		LUA->PushLuaObject(obj);
	}
}

void Detours_Think()
{
}

bool pre = false;
std::vector<Detouring::Hook*> detours = {};
void CreateDetour(Detouring::Hook* hook, const char* name, Detouring::Hook::Target target, void* func)
{
	hook->Create(target, func);
	hook->Enable();

	detours.push_back(hook);

	if (!hook->IsValid()) {
		Msg("	Invalid detour for %s!\n", name);
	}
}

SymbolFinder symfinder;
void* LoadFunction(void* loader, Symbol symbol)
{
	return symfinder.Resolve(loader, symbol.name.c_str(), symbol.length);
}

void* LoadFunction(void* loader, const std::vector<Symbol> symbols)
{
	void* func;
	for (Symbol sym : symbols)
	{
		func = LoadFunction(loader, sym);
		
		if (func)
			return func;
	}
}

void Detours_Init()
{
	Msg("	--- Starting Detours ---\n");

	Msg("		--- InitLuaClasses ---\n");
	SourceSDK::ModuleLoader server_loader("server");
	void* sv_InitLuaClasses = LoadFunction(server_loader.GetModule(), InitLuaClassesSym);
	CheckFunction(sv_InitLuaClasses, "InitLuaClasses");
	CreateDetour(&detour_InitLuaClasses, "InitLuaClasses", reinterpret_cast<void*>(sv_InitLuaClasses), reinterpret_cast<void*>(&hook_InitLuaClasses));
	func_InitLuaClasses = detour_InitLuaClasses.GetTrampoline<InitLuaClasses>();

	void* sv_CLuaLibrary_CLuaLibrary = LoadFunction(server_loader.GetModule(), CLuaLibrary_CLuaLibrarySym);
	CheckFunction(sv_CLuaLibrary_CLuaLibrary, "CLuaLibrary::CLuaLibrary");
	CreateDetour(&detour_CLuaLibrary_CLuaLibrary, "CLuaLibrary::CLuaLibrary", reinterpret_cast<void*>(sv_CLuaLibrary_CLuaLibrary), reinterpret_cast<void*>(&hook_CLuaLibrary_CLuaLibrary));

	Msg("	--- Finished Detours ---\n");

	Msg("	--- Function loader ---\n");

	SourceSDK::ModuleLoader lua_shared_loader("lua_shared");

	func_CreateLuaInterface = (CreateLuaInterface)LoadFunction(lua_shared_loader.GetModule(), CreateLuaInterfaceSym);
	CheckFunction(func_CreateLuaInterface, "CreateLuaInterface");

	func_CloseLuaInterface = (CloseLuaInterface)LoadFunction(lua_shared_loader.GetModule(), CloseLuaInterfaceSym);
	CheckFunction(func_CloseLuaInterface, "CloseLuaInterface");

	func_luaL_loadstring = (luaL_loadstring)LoadFunction(lua_shared_loader.GetModule(), luaL_loadstringSym);
	CheckFunction(func_luaL_loadstring, "luaL_loadstring");

	func_lua_tostring = (lua_tostring)LoadFunction(lua_shared_loader.GetModule(), lua_tostringSym);
	CheckFunction(func_lua_tostring, "lua_tostring");

	/*
		Server suff
	*/
	func_InitLuaLibraries = (TInitLuaLibraries)LoadFunction(server_loader.GetModule(), InitLuaLibrariesSym);
	CheckFunction(func_InitLuaLibraries, "InitLuaLibraries");

	func_InitLuaClasses = (InitLuaClasses)LoadFunction(server_loader.GetModule(), InitLuaClassesSym);
	CheckFunction(func_InitLuaClasses, "InitLuaClasses");

	func_CLuaGlobalLibrary_InitLibraries = (CLuaGlobalLibrary_InitLibraries)LoadFunction(server_loader.GetModule(), CLuaGlobalLibrary_InitLibrariesSym);
	CheckFunction(func_CLuaGlobalLibrary_InitLibraries, "CLuaGlobalLibrary::InitLibraries");

	SourceSDK::FactoryLoader fac_server_loader( "server" );
	g_pGlobalLuaLibraryFactory = ResolveSymbol<void>(
		fac_server_loader, g_pGlobalLuaLibraryFactorySym
	);
	CheckFunction(g_pGlobalLuaLibraryFactory, "g_pGlobalLuaLibraryFactory");

	/*
		CLuaGameCallback stuff
	*/
	func_CLuaGameCallback_CreateLuaObject = (CLuaGameCallback_CreateLuaObject)LoadFunction(server_loader.GetModule(), CLuaGameCallback_CreateLuaObjectSym);
	CheckFunction(func_CLuaGameCallback_CreateLuaObject, "CLuaGameCallback::CreateLuaObject");

	func_CLuaGameCallback_DestroyLuaObject = (CLuaGameCallback_DestroyLuaObject)LoadFunction(server_loader.GetModule(), CLuaGameCallback_DestroyLuaObjectSym);
	CheckFunction(func_CLuaGameCallback_DestroyLuaObject, "CLuaGameCallback::DestroyLuaObject");

	func_CLuaGameCallback_ErrorPrint = (CLuaGameCallback_ErrorPrint)LoadFunction(server_loader.GetModule(), CLuaGameCallback_ErrorPrintSym);
	CheckFunction(func_CLuaGameCallback_ErrorPrint, "CLuaGameCallback::ErrorPrint");

	func_CLuaGameCallback_LuaError = (CLuaGameCallback_LuaError)LoadFunction(server_loader.GetModule(), CLuaGameCallback_LuaErrorSym);
	CheckFunction(func_CLuaGameCallback_LuaError, "CLuaGameCallback::LuaError");

	func_CLuaGameCallback_Msg = (CLuaGameCallback_Msg)LoadFunction(server_loader.GetModule(), CLuaGameCallback_MsgSym);
	CheckFunction(func_CLuaGameCallback_Msg, "CLuaGameCallback::Msg");

	func_CLuaGameCallback_MsgColour = (CLuaGameCallback_MsgColour)LoadFunction(server_loader.GetModule(), CLuaGameCallback_MsgColourSym);
	CheckFunction(func_CLuaGameCallback_MsgColour, "CLuaGameCallback::MsgColour");

	Msg("	--- Finished loading functions ---\n");
}

void Detours_Shutdown()
{
	for (Detouring::Hook* hook : detours) {
		hook->Destroy();
	}

	/*mutex.Lock();
	snapshotdata->run = false;
	mutex.Unlock();*/
}