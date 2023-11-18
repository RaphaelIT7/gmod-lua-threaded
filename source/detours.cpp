#include "detours.h"
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;
/*luaL_newstate func_luaL_newstate;
TLuaPanic func_LuaPanic;
TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;
Tlua_atpanic func_lua_atpanic;*/
luaL_loadstring func_luaL_loadstring;
//luaL_openlibs func_luaL_openlibs;
lua_tostring func_lua_tostring;

InitLuaLibraries func_InitLuaLibraries;
InitLuaClasses func_InitLuaClasses;

CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;
CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;
CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;
CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;
CLuaGameCallback_Msg func_CLuaGameCallback_Msg;
CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;

template<class T>
void CheckFunction(T func, const char* name)
{
	if (func == nullptr) {
		Msg("Could not locate %s symbol!\n", name);
	}
}

SymbolFinder symfinder;
void* CheckFunction(void* loader, Symbol symbol)
{
	return symfinder.Resolve(loader, symbol.name.c_str(), symbol.length);
}

void* CheckFunction(void* loader, const std::vector<Symbol> symbols)
{
	void* func;
	for (Symbol sym : symbols)
	{
		func = CheckFunction(loader, sym);
		
		if (func)
			return func;
	}
}

void Symbols_Init() 
{
	SourceSDK::ModuleLoader lua_shared_loader("lua_shared");

	func_CreateLuaInterface = (CreateLuaInterface)CheckFunction(lua_shared_loader.GetModule(), CreateLuaInterfaceSym);
	CheckFunction(func_CreateLuaInterface, "CreateLuaInterface");

	func_CloseLuaInterface = (CloseLuaInterface)CheckFunction(lua_shared_loader.GetModule(), CloseLuaInterfaceSym);
	CheckFunction(func_CloseLuaInterface, "CloseLuaInterface");

	/*func_luaL_newstate = (luaL_newstate)CheckFunction(lua_shared_loader.GetModule(), luaL_newstateSym);
	CheckFunction(func_luaL_newstate, "luaL_newstate");

	func_LuaPanic = (TLuaPanic)CheckFunction(lua_shared_loader.GetModule(), LuaPanicSym);
	CheckFunction(func_LuaPanic, "LuaPanic");

	func_lua_atpanic = (Tlua_atpanic)CheckFunction(lua_shared_loader.GetModule(), lua_atpanicSym);
	CheckFunction(func_lua_atpanic, "lua_atpanic");

	func_AdvancedLuaErrorReporter = (TAdvancedLuaErrorReporter)CheckFunction(lua_shared_loader.GetModule(), AdvancedLuaErrorReporterSym);
	CheckFunction(func_AdvancedLuaErrorReporter, "AdvancedLuaErrorReporter");*/

	func_luaL_loadstring = (luaL_loadstring)CheckFunction(lua_shared_loader.GetModule(), luaL_loadstringSym);
	CheckFunction(func_luaL_loadstring, "luaL_loadstring");

	//func_luaL_openlibs = (luaL_openlibs)CheckFunction(lua_shared_loader.GetModule(), luaL_openlibsSym);
	//CheckFunction(func_luaL_openlibs, "luaL_openlibs");

	func_lua_tostring = (lua_tostring)CheckFunction(lua_shared_loader.GetModule(), lua_tostringSym);
	CheckFunction(func_lua_tostring, "lua_tostring");

	/*
		Server suff
	*/
	SourceSDK::ModuleLoader server_loader("server");
	func_InitLuaLibraries = (InitLuaLibraries)CheckFunction(server_loader.GetModule(), InitLuaLibrariesSym);
	CheckFunction(func_InitLuaLibraries, "InitLuaLibraries");

	func_InitLuaClasses = (InitLuaClasses)CheckFunction(server_loader.GetModule(), InitLuaClassesSym);
	CheckFunction(func_InitLuaClasses, "InitLuaClasses");


	func_CLuaGameCallback_CreateLuaObject = (CLuaGameCallback_CreateLuaObject)CheckFunction(server_loader.GetModule(), CLuaGameCallback_CreateLuaObjectSym);
	CheckFunction(func_CLuaGameCallback_CreateLuaObject, "CLuaGameCallback::CreateLuaObject");

	func_CLuaGameCallback_DestroyLuaObject = (CLuaGameCallback_DestroyLuaObject)CheckFunction(server_loader.GetModule(), CLuaGameCallback_DestroyLuaObjectSym);
	CheckFunction(func_CLuaGameCallback_DestroyLuaObject, "CLuaGameCallback::DestroyLuaObject");

	func_CLuaGameCallback_ErrorPrint = (CLuaGameCallback_ErrorPrint)CheckFunction(server_loader.GetModule(), CLuaGameCallback_ErrorPrintSym);
	CheckFunction(func_CLuaGameCallback_ErrorPrint, "CLuaGameCallback::ErrorPrint");

	func_CLuaGameCallback_LuaError = (CLuaGameCallback_LuaError)CheckFunction(server_loader.GetModule(), CLuaGameCallback_LuaErrorSym);
	CheckFunction(func_CLuaGameCallback_LuaError, "CLuaGameCallback::LuaError");

	func_CLuaGameCallback_Msg = (CLuaGameCallback_Msg)CheckFunction(server_loader.GetModule(), CLuaGameCallback_MsgSym);
	CheckFunction(func_CLuaGameCallback_Msg, "CLuaGameCallback::Msg");

	func_CLuaGameCallback_MsgColour = (CLuaGameCallback_MsgColour)CheckFunction(server_loader.GetModule(), CLuaGameCallback_MsgColourSym);
	CheckFunction(func_CLuaGameCallback_MsgColour, "CLuaGameCallback::MsgColour");
}