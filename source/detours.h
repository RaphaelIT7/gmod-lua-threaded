#include <GarrysMod/Symbol.hpp>
#include <GarrysMod/Lua/LuaInterface.h>

using namespace GarrysMod::Lua;

/*
	lua_shared Symbols
*/
typedef ILuaInterface* (*CreateLuaInterface)(bool);
const Symbol CreateLuaInterfaceSym = Symbol::FromName("_Z18CreateLuaInterfaceb");
extern CreateLuaInterface func_CreateLuaInterface;

typedef void (*CloseLuaInterface)(ILuaInterface*);
const Symbol CloseLuaInterfaceSym = Symbol::FromName("_Z17CloseLuaInterfaceP13ILuaInterface");
extern CloseLuaInterface func_CloseLuaInterface;

typedef lua_State* (*luaL_newstate)();
const Symbol luaL_newstateSym = Symbol::FromName("_luaL_newstate");
extern luaL_newstate func_luaL_newstate;

typedef void (*TLuaPanic)(lua_State*);
const Symbol LuaPanicSym = Symbol::FromName("_ZL8LuaPanicP9lua_State");
extern TLuaPanic func_LuaPanic;

typedef void (*Tlua_atpanic)(lua_State*, CFunc);
const Symbol lua_atpanicSym = Symbol::FromName("_lua_atpanic");
extern Tlua_atpanic func_lua_atpanic;

typedef void (*TAdvancedLuaErrorReporter)(lua_State*);
const Symbol AdvancedLuaErrorReporterSym = Symbol::FromName("_Z24AdvancedLuaErrorReporterP9lua_State");
extern TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;

typedef void (*InitLuaLibraries)(ILuaInterface*);
const Symbol InitLuaLibrariesSym = Symbol::FromName("_Z16InitLuaLibrariesP13ILuaInterface");
extern InitLuaLibraries func_InitLuaLibraries;

typedef void (*InitLuaClasses)(ILuaInterface*);
const Symbol InitLuaClassesSym = Symbol::FromName("_Z14InitLuaClassesP13ILuaInterface");
extern InitLuaClasses func_InitLuaClasses;

extern void Symbols_Init();