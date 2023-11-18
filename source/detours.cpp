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

luaopen_base func_luaopen_base;
luaopen_bit func_luaopen_bit;
luaopen_debug func_luaopen_debug;
luaopen_jit func_luaopen_jit;
luaopen_math func_luaopen_math;
luaopen_os func_luaopen_os;
luaopen_package func_luaopen_package;
luaopen_string func_luaopen_string;
luaopen_table func_luaopen_table;

InitLuaLibraries func_InitLuaLibraries;
InitLuaClasses func_InitLuaClasses;

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

	/*
		Lua open stuff
	*/
	func_luaopen_base = (luaopen_base)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_baseSym.name.c_str(), luaopen_baseSym.length);
	CheckFunction(func_luaopen_base, "luaopen_base");

	func_luaopen_bit = (luaopen_bit)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_bitSym.name.c_str(), luaopen_bitSym.length);
	CheckFunction(func_luaopen_bit, "luaopen_bit");

	func_luaopen_debug = (luaopen_debug)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_debugSym.name.c_str(), luaopen_debugSym.length);
	CheckFunction(func_luaopen_debug, "luaopen_debug");

	func_luaopen_jit = (luaopen_jit)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_jitSym.name.c_str(), luaopen_jitSym.length);
	CheckFunction(func_luaopen_jit, "luaopen_jit");

	func_luaopen_math = (luaopen_math)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_mathSym.name.c_str(), luaopen_mathSym.length);
	CheckFunction(func_luaopen_math, "luaopen_math");

	func_luaopen_os = (luaopen_os)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_osSym.name.c_str(), luaopen_osSym.length);
	CheckFunction(func_luaopen_os, "luaopen_os");

	func_luaopen_package = (luaopen_package)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_packageSym.name.c_str(), luaopen_packageSym.length);
	CheckFunction(func_luaopen_package, "luaopen_package");

	func_luaopen_string = (luaopen_string)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_stringSym.name.c_str(), luaopen_stringSym.length);
	CheckFunction(func_luaopen_string, "luaopen_string");

	func_luaopen_table = (luaopen_table)symfinder.Resolve(lua_shared_loader.GetModule(), luaopen_tableSym.name.c_str(), luaopen_tableSym.length);
	CheckFunction(func_luaopen_table, "luaopen_table");

	/*
		Server suff
	*/
	SourceSDK::ModuleLoader server_loader("server");
	func_InitLuaLibraries = (InitLuaLibraries)symfinder.Resolve(server_loader.GetModule(), InitLuaLibrariesSym.name.c_str(), InitLuaLibrariesSym.length);
	CheckFunction(func_InitLuaLibraries, "InitLuaLibraries");

	func_InitLuaClasses = (InitLuaClasses)symfinder.Resolve(server_loader.GetModule(), InitLuaClassesSym.name.c_str(), InitLuaClassesSym.length);
	CheckFunction(func_InitLuaClasses, "InitLuaClasses");
}