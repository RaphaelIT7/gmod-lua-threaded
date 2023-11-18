#include "detours.h"
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;
luaL_newstate func_luaL_newstate;
TLuaPanic func_LuaPanic;
TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;
Tlua_atpanic func_lua_atpanic;
luaL_loadstring func_luaL_loadstring;
luaL_openlibs func_luaL_openlibs;
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

void Symbols_Init() 
{
	SymbolFinder symfinder;
	SourceSDK::ModuleLoader lua_shared_loader("lua_shared");

	func_CreateLuaInterface = (CreateLuaInterface)symfinder.Resolve(lua_shared_loader.GetModule(), CreateLuaInterfaceSym.name.c_str(), CreateLuaInterfaceSym.length);
	CheckFunction(func_CreateLuaInterface, "CreateLuaInterface");

	func_CloseLuaInterface = (CloseLuaInterface)symfinder.Resolve(lua_shared_loader.GetModule(), CloseLuaInterfaceSym.name.c_str(), CloseLuaInterfaceSym.length);
	CheckFunction(func_CloseLuaInterface, "CloseLuaInterface");

	func_luaL_newstate = (luaL_newstate)symfinder.Resolve(lua_shared_loader.GetModule(), luaL_newstateSym.name.c_str(), luaL_newstateSym.length);
	CheckFunction(func_luaL_newstate, "luaL_newstate");

	func_LuaPanic = (TLuaPanic)symfinder.Resolve(lua_shared_loader.GetModule(), LuaPanicSym.name.c_str(), LuaPanicSym.length);
	CheckFunction(func_LuaPanic, "LuaPanic");

	func_lua_atpanic = (Tlua_atpanic)symfinder.Resolve(lua_shared_loader.GetModule(), lua_atpanicSym.name.c_str(), lua_atpanicSym.length);
	CheckFunction(func_lua_atpanic, "lua_atpanic");

	func_AdvancedLuaErrorReporter = (TAdvancedLuaErrorReporter)symfinder.Resolve(lua_shared_loader.GetModule(), AdvancedLuaErrorReporterSym.name.c_str(), AdvancedLuaErrorReporterSym.length);
	CheckFunction(func_AdvancedLuaErrorReporter, "AdvancedLuaErrorReporter");

	func_luaL_loadstring = (luaL_loadstring)symfinder.Resolve(lua_shared_loader.GetModule(), luaL_loadstringSym.name.c_str(), luaL_loadstringSym.length);
	CheckFunction(func_luaL_loadstring, "luaL_loadstring");

	func_luaL_openlibs = (luaL_openlibs)symfinder.Resolve(lua_shared_loader.GetModule(), luaL_openlibsSym.name.c_str(), luaL_openlibsSym.length);
	CheckFunction(func_luaL_openlibs, "luaL_openlibs");

	func_lua_tostring = (lua_tostring)symfinder.Resolve(lua_shared_loader.GetModule(), lua_tostringSym.name.c_str(), lua_tostringSym.length);
	CheckFunction(func_lua_tostring, "lua_tostring");

	/*
		Server suff
	*/
	SourceSDK::ModuleLoader server_loader("server");
	func_InitLuaLibraries = (InitLuaLibraries)symfinder.Resolve(server_loader.GetModule(), InitLuaLibrariesSym.name.c_str(), InitLuaLibrariesSym.length);
	CheckFunction(func_InitLuaLibraries, "InitLuaLibraries");

	func_InitLuaClasses = (InitLuaClasses)symfinder.Resolve(server_loader.GetModule(), InitLuaClassesSym.name.c_str(), InitLuaClassesSym.length);
	CheckFunction(func_InitLuaClasses, "InitLuaClasses");


	func_CLuaGameCallback_CreateLuaObject = (CLuaGameCallback_CreateLuaObject)symfinder.Resolve(server_loader.GetModule(), CLuaGameCallback_CreateLuaObjectSym.name.c_str(), CLuaGameCallback_CreateLuaObjectSym.length);
	CheckFunction(func_CLuaGameCallback_CreateLuaObject, "CLuaGameCallback::CreateLuaObject");

	func_CLuaGameCallback_DestroyLuaObject = (CLuaGameCallback_DestroyLuaObject)symfinder.Resolve(server_loader.GetModule(), CLuaGameCallback_DestroyLuaObjectSym.name.c_str(), CLuaGameCallback_DestroyLuaObjectSym.length);
	CheckFunction(func_CLuaGameCallback_DestroyLuaObject, "CLuaGameCallback::DestroyLuaObject");

	func_CLuaGameCallback_ErrorPrint = (CLuaGameCallback_ErrorPrint)symfinder.Resolve(server_loader.GetModule(), CLuaGameCallback_ErrorPrintSym.name.c_str(), CLuaGameCallback_ErrorPrintSym.length);
	CheckFunction(func_CLuaGameCallback_ErrorPrint, "CLuaGameCallback::ErrorPrint");

	func_CLuaGameCallback_LuaError = (CLuaGameCallback_LuaError)symfinder.Resolve(server_loader.GetModule(), CLuaGameCallback_LuaErrorSym.name.c_str(), CLuaGameCallback_LuaErrorSym.length);
	CheckFunction(func_CLuaGameCallback_LuaError, "CLuaGameCallback::LuaError");

	func_CLuaGameCallback_Msg = (CLuaGameCallback_Msg)symfinder.Resolve(server_loader.GetModule(), CLuaGameCallback_MsgSym.name.c_str(), CLuaGameCallback_MsgSym.length);
	CheckFunction(func_CLuaGameCallback_Msg, "CLuaGameCallback::Msg");

	func_CLuaGameCallback_MsgColour = (CLuaGameCallback_MsgColour)symfinder.Resolve(server_loader.GetModule(), CLuaGameCallback_MsgColourSym.name.c_str(), CLuaGameCallback_MsgColourSym.length);
	CheckFunction(func_CLuaGameCallback_MsgColour, "CLuaGameCallback::MsgColour");
}