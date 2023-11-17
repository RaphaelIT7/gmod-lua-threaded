#include "detours.h"
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;
luaL_newstate func_luaL_newstate;
TLuaPanic func_LuaPanic;
TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;
Tlua_atpanic func_lua_atpanic;

InitLuaLibraries func_InitLuaLibraries;
InitLuaClasses func_InitLuaClasses;

template<class T>
void CheckFunction(T func, const char* name)
{
	if (func == nullptr) {
		Msg("Could not locate %s symbol!", name);
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

	SourceSDK::ModuleLoader server_loader("server");

	func_InitLuaLibraries = (InitLuaLibraries)symfinder.Resolve(server_loader.GetModule(), InitLuaLibrariesSym.name.c_str(), InitLuaLibrariesSym.length);
	CheckFunction(func_InitLuaLibraries, "InitLuaLibraries");

	func_InitLuaClasses = (InitLuaClasses)symfinder.Resolve(server_loader.GetModule(), InitLuaClassesSym.name.c_str(), InitLuaClassesSym.length);
	CheckFunction(func_InitLuaClasses, "InitLuaClasses");
}