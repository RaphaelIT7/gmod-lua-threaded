#include "detours.h"
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>
#include <detouring/hook.hpp>
#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/Symbols.hpp>
#include <unordered_map>

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;
TLuaPanic func_LuaPanic;
Tlua_pcall func_lua_pcall;
Tlua_atpanic func_lua_atpanic;
luaL_loadstring func_luaL_loadstring;
lua_tostring func_lua_tostring;
llua_getstack func_lua_getstack;
llua_getinfo func_lua_getinfo;
llua_pushstring func_lua_pushstring;
llua_setfield func_lua_setfield;
luaL_loadbuffer func_luaL_loadbuffer;
GMOD_LoadBinaryModule func_GMOD_LoadBinaryModule;
TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;

TInitLuaLibraries func_InitLuaLibraries;
InitLuaClasses func_InitLuaClasses;
CLuaGlobalLibrary_InitLibraries func_CLuaGlobalLibrary_InitLibraries;
CLuaGameEnums_InitLibraries func_CLuaGameEnums_InitLibraries;
void* g_pGlobalLuaLibraryFactory;
ConCommand_IsBlocked func_ConCommand_IsBlocked;
TUTIL_GetCommandClient func_UTIL_GetCommandClient;
TEditor_SendCommand func_Editor_SendCommand;
TGMOD_LoadParticleConfigFile func_GMOD_LoadParticleConfigFile;
TGetAmmoDef func_GetAmmoDef;

CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;
CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;
CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;
CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;
CLuaGameCallback_Msg func_CLuaGameCallback_Msg;
CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;

IFileSystem* gpFileSystem;

SymbolFinder symfinder;
void* FindFunction(void* loader, Symbol symbol)
{
	return symfinder.Resolve(loader, symbol.name.c_str(), symbol.length);
}

void* FindFunction(void* loader, const std::vector<Symbol> symbols)
{
	void* func;
	for (Symbol sym : symbols)
	{
		func = FindFunction(loader, sym);
		
		if (func)
			return func;
	}

	return nullptr;
}

void* FindSymbol(std::string name)
{
	SourceSDK::ModuleLoader server_loader("server");
	void* func;
	for (int i=15; i<35;i++) // Search from _Z15 up to _Z35. Should work in most cases.
	{
		func = FindFunction(server_loader.GetModule(), Symbol::FromName(((std::string)"_ZL") + std::to_string(i) + ((std::string)"redir__") + name + ((std::string)"P9lua_State"))); // Build a string like: _Z23real__GLobal__ServerLogP13ILuaInterface
		if (func != nullptr)
			break;
	}

	return func;
}

void* GetFunction(SourceSDK::ModuleLoader loader, const char* name, Symbol sym)
{
	void* func = FindFunction(loader.GetModule(), sym);
	CheckFunction(func, name);

	return func;
}

void* GetFunction(SourceSDK::ModuleLoader loader, const char* name, std::vector<Symbol> sym)
{
	void* func = FindFunction(loader.GetModule(), sym);
	CheckFunction(func, name);

	return func;
}

std::unordered_map<void*, DoStackCheckCallback> stackcheck_callback;
void AddStackCheckCallback(void* key, DoStackCheckCallback func)
{
	stackcheck_callback[key] = func;
}

void RemoveStackCheckCallback(void* key)
{
	stackcheck_callback.erase(key);
}

std::vector<Detouring::Hook*> detours = {};
void CreateDetour(Detouring::Hook* hook, const char* name, void* module, Symbol symbol, void* hook_func)
{
	void* func = symfinder.Resolve(module, symbol.name.c_str(), symbol.length);
	if (!CheckFunction(func, name))
		return;

	hook->Create(func, hook_func);
	hook->Enable();

	detours.push_back(hook);

	if (!hook->IsValid()) {
		Msg("Failed to detour %s!\n", name);
	}
}

Detouring::Hook detour_CLuaInterface_DoStackCheck;
void hook_CLuaInterface_DoStackCheck(void* iface)
{
	if (stackcheck_callback.find(iface) != stackcheck_callback.end())
		stackcheck_callback[iface](iface);

	detour_CLuaInterface_DoStackCheck.GetTrampoline<CLuaInterface_DoStackCheck>()(iface);
}


void Symbols_Init() 
{
	gpFileSystem = InterfacePointers::FileSystemServer();

	SourceSDK::ModuleLoader lua_shared_loader("lua_shared");
	CreateDetour(&detour_CLuaInterface_DoStackCheck, "CLuaInterface::DoStackCheck", lua_shared_loader.GetModule(), CLuaInterface_DoStackCheckSym, (void*)hook_CLuaInterface_DoStackCheck);

	func_CreateLuaInterface = (CreateLuaInterface)FindFunction(lua_shared_loader.GetModule(), CreateLuaInterfaceSym);
	CheckFunction(func_CreateLuaInterface, "CreateLuaInterface");

	func_CloseLuaInterface = (CloseLuaInterface)FindFunction(lua_shared_loader.GetModule(), CloseLuaInterfaceSym);
	CheckFunction(func_CloseLuaInterface, "CloseLuaInterface");

	/*func_luaL_newstate = (luaL_newstate)FindFunction(lua_shared_loader.GetModule(), luaL_newstateSym);
	CheckFunction(func_luaL_newstate, "luaL_newstate");*/

	func_LuaPanic = (TLuaPanic)FindFunction(lua_shared_loader.GetModule(), LuaPanicSym);
	CheckFunction(func_LuaPanic, "LuaPanic");

	func_lua_atpanic = (Tlua_atpanic)FindFunction(lua_shared_loader.GetModule(), lua_atpanicSym);
	CheckFunction(func_lua_atpanic, "lua_atpanic");

	func_lua_pcall = (Tlua_pcall)FindFunction(lua_shared_loader.GetModule(), lua_pcallSym);
	CheckFunction(func_lua_pcall, "lua_pcall");

	/*func_AdvancedLuaErrorReporter = (TAdvancedLuaErrorReporter)FindFunction(lua_shared_loader.GetModule(), AdvancedLuaErrorReporterSym);
	CheckFunction(func_AdvancedLuaErrorReporter, "AdvancedLuaErrorReporter");*/

	func_luaL_loadstring = (luaL_loadstring)FindFunction(lua_shared_loader.GetModule(), luaL_loadstringSym);
	CheckFunction(func_luaL_loadstring, "luaL_loadstring");

	//func_luaL_openlibs = (luaL_openlibs)FindFunction(lua_shared_loader.GetModule(), luaL_openlibsSym);
	//CheckFunction(func_luaL_openlibs, "luaL_openlibs");

	func_lua_tostring = (lua_tostring)FindFunction(lua_shared_loader.GetModule(), lua_tostringSym);
	CheckFunction(func_lua_tostring, "lua_tostring");

	func_lua_getstack = (llua_getstack)FindFunction(lua_shared_loader.GetModule(), lua_getstackSym);
	CheckFunction(func_lua_getstack, "lua_getstack");

	func_lua_getinfo = (llua_getinfo)FindFunction(lua_shared_loader.GetModule(), lua_getinfoSym);
	CheckFunction(func_lua_getinfo, "lua_getinfo");

	func_lua_pushstring = (llua_pushstring)FindFunction(lua_shared_loader.GetModule(), lua_pushstringSym);
	CheckFunction(func_lua_pushstring, "lua_pushstring");

	func_lua_setfield = (llua_setfield)FindFunction(lua_shared_loader.GetModule(), lua_setfieldSym);
	CheckFunction(func_lua_setfield, "lua_setfield");

	func_luaL_loadbuffer = (luaL_loadbuffer)FindFunction(lua_shared_loader.GetModule(), luaL_loadbufferSym);
	CheckFunction(func_luaL_loadbuffer, "luaL_loadbuffer");

	func_GMOD_LoadBinaryModule = (GMOD_LoadBinaryModule)FindFunction(lua_shared_loader.GetModule(), GMOD_LoadBinaryModuleSym);
	CheckFunction(func_GMOD_LoadBinaryModule, "GMOD_LoadBinaryModule");

	func_AdvancedLuaErrorReporter = (TAdvancedLuaErrorReporter)FindFunction(lua_shared_loader.GetModule(), AdvancedLuaErrorReporterSym);
	CheckFunction(func_AdvancedLuaErrorReporter, "AdvancedLuaErrorReporter");

	/*
		Server suff
	*/
	SourceSDK::ModuleLoader server_loader("server");
	func_InitLuaLibraries = (TInitLuaLibraries)FindFunction(server_loader.GetModule(), InitLuaLibrariesSym);
	CheckFunction(func_InitLuaLibraries, "InitLuaLibraries");

	func_InitLuaClasses = (InitLuaClasses)FindFunction(server_loader.GetModule(), InitLuaClassesSym);
	CheckFunction(func_InitLuaClasses, "InitLuaClasses");

	func_CLuaGlobalLibrary_InitLibraries = (CLuaGlobalLibrary_InitLibraries)FindFunction(server_loader.GetModule(), CLuaGlobalLibrary_InitLibrariesSym);
	CheckFunction(func_CLuaGlobalLibrary_InitLibraries, "CLuaGlobalLibrary::InitLibraries");

	SourceSDK::FactoryLoader fac_server_loader( "server" );
	g_pGlobalLuaLibraryFactory = ResolveSymbol<void>(
		fac_server_loader, g_pGlobalLuaLibraryFactorySym
	);
	CheckFunction(g_pGlobalLuaLibraryFactory, "g_pGlobalLuaLibraryFactory");

	func_CLuaGameEnums_InitLibraries = (CLuaGameEnums_InitLibraries)FindFunction(server_loader.GetModule(), CLuaGameEnums_InitLibrariesSym);
	CheckFunction(func_CLuaGameEnums_InitLibraries, "CLuaGameEnums::InitLibraries");

	func_ConCommand_IsBlocked = (ConCommand_IsBlocked)FindFunction(server_loader.GetModule(), ConCommand_IsBlockedSym);
	CheckFunction(func_ConCommand_IsBlocked, "ConCommand_IsBlocked");

	func_UTIL_GetCommandClient = (TUTIL_GetCommandClient)FindFunction(server_loader.GetModule(), UTIL_GetCommandClientSym);
	CheckFunction(func_UTIL_GetCommandClient, "UTIL_GetCommandClient");

	func_Editor_SendCommand = (TEditor_SendCommand)FindFunction(server_loader.GetModule(), Editor_SendCommandSym);
	CheckFunction(func_Editor_SendCommand, "Editor_SendCommand");

	func_GMOD_LoadParticleConfigFile = (TGMOD_LoadParticleConfigFile)FindFunction(server_loader.GetModule(), GMOD_LoadParticleConfigFileSym);
	CheckFunction(func_GMOD_LoadParticleConfigFile, "GMOD_LoadParticleConfigFile");

	func_GetAmmoDef = (TGetAmmoDef)FindFunction(server_loader.GetModule(), GetAmmoDefSym);
	CheckFunction(func_GetAmmoDef, "GetAmmoDef");

	/*
		CLuaGameCallback stuff
	*/
	func_CLuaGameCallback_CreateLuaObject = (CLuaGameCallback_CreateLuaObject)FindFunction(server_loader.GetModule(), CLuaGameCallback_CreateLuaObjectSym);
	CheckFunction(func_CLuaGameCallback_CreateLuaObject, "CLuaGameCallback::CreateLuaObject");

	func_CLuaGameCallback_DestroyLuaObject = (CLuaGameCallback_DestroyLuaObject)FindFunction(server_loader.GetModule(), CLuaGameCallback_DestroyLuaObjectSym);
	CheckFunction(func_CLuaGameCallback_DestroyLuaObject, "CLuaGameCallback::DestroyLuaObject");

	func_CLuaGameCallback_ErrorPrint = (CLuaGameCallback_ErrorPrint)FindFunction(server_loader.GetModule(), CLuaGameCallback_ErrorPrintSym);
	CheckFunction(func_CLuaGameCallback_ErrorPrint, "CLuaGameCallback::ErrorPrint");

	func_CLuaGameCallback_LuaError = (CLuaGameCallback_LuaError)FindFunction(server_loader.GetModule(), CLuaGameCallback_LuaErrorSym);
	CheckFunction(func_CLuaGameCallback_LuaError, "CLuaGameCallback::LuaError");

	func_CLuaGameCallback_Msg = (CLuaGameCallback_Msg)FindFunction(server_loader.GetModule(), CLuaGameCallback_MsgSym);
	CheckFunction(func_CLuaGameCallback_Msg, "CLuaGameCallback::Msg");

	func_CLuaGameCallback_MsgColour = (CLuaGameCallback_MsgColour)FindFunction(server_loader.GetModule(), CLuaGameCallback_MsgColourSym);
	CheckFunction(func_CLuaGameCallback_MsgColour, "CLuaGameCallback::MsgColour");
}