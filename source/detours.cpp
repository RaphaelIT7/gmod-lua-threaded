#include "detours.h"
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>
#include <GarrysMod/InterfacePointers.hpp>
#include <GarrysMod/Symbols.hpp>

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;
TLuaPanic func_LuaPanic;
Tlua_pcall func_lua_pcall;
Tlua_atpanic func_lua_atpanic;
luaL_loadstring func_luaL_loadstring;
lua_tostring func_lua_tostring;
GMOD_LoadBinaryModule func_GMOD_LoadBinaryModule;

TInitLuaLibraries func_InitLuaLibraries;
InitLuaClasses func_InitLuaClasses;
CLuaGlobalLibrary_InitLibraries func_CLuaGlobalLibrary_InitLibraries;
CLuaGameEnums_InitLibraries func_CLuaGameEnums_InitLibraries;
void* g_pGlobalLuaLibraryFactory;
ConCommand_IsBlocked func_ConCommand_IsBlocked;
UTIL_GetCommandClient func_UTIL_GetCommandClient;
TEditor_SendCommand func_Editor_SendCommand;

CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;
CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;
CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;
CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;
CLuaGameCallback_Msg func_CLuaGameCallback_Msg;
CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;

IFileSystem* gpFileSystem;

template<class T>
void CheckFunction(T func, const char* name)
{
	if (func == nullptr) {
		Msg("Could not locate %s symbol!\n", name);
	}
}

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
	CheckFunction(func_CreateLuaInterface, name);

	return func;
}

void* GetFunction(SourceSDK::ModuleLoader loader, const char* name, std::vector<Symbol> sym)
{
	void* func = FindFunction(loader.GetModule(), sym);
	CheckFunction(func_CreateLuaInterface, name);

	return func;
}


void Symbols_Init() 
{
	gpFileSystem = InterfacePointers::FileSystemServer();

	SourceSDK::ModuleLoader lua_shared_loader("lua_shared");

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

	func_GMOD_LoadBinaryModule = (GMOD_LoadBinaryModule)FindFunction(lua_shared_loader.GetModule(), GMOD_LoadBinaryModuleSym);
	CheckFunction(func_GMOD_LoadBinaryModule, "GMOD_LoadBinaryModule");

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

	func_UTIL_GetCommandClient = (UTIL_GetCommandClient)FindFunction(server_loader.GetModule(), UTIL_GetCommandClientSym);
	CheckFunction(func_UTIL_GetCommandClient, "UTIL_GetCommandClient");

	func_Editor_SendCommand = (TEditor_SendCommand)FindFunction(server_loader.GetModule(), Editor_SendCommandSym);
	CheckFunction(func_Editor_SendCommand, "Editor_SendCommand");

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