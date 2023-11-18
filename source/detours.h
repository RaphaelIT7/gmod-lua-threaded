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
const Symbol luaL_newstateSym = Symbol::FromName("luaL_newstate");
extern luaL_newstate func_luaL_newstate;

typedef void (*TLuaPanic)(lua_State*);
const Symbol LuaPanicSym = Symbol::FromName("_ZL8LuaPanicP9lua_State");
extern TLuaPanic func_LuaPanic;

typedef void (*Tlua_atpanic)(lua_State*, CFunc);
const Symbol lua_atpanicSym = Symbol::FromName("lua_atpanic");
extern Tlua_atpanic func_lua_atpanic;

typedef void (*TAdvancedLuaErrorReporter)(lua_State*);
const Symbol AdvancedLuaErrorReporterSym = Symbol::FromName("_Z24AdvancedLuaErrorReporterP9lua_State");
extern TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;

typedef void (*luaL_loadstring)(lua_State*, const char*);
const Symbol luaL_loadstringSym = Symbol::FromName("luaL_loadstring");
extern luaL_loadstring func_luaL_loadstring;

typedef void (*luaopen_base)(lua_State*);
const Symbol luaopen_baseSym = Symbol::FromName("luaopen_base");
extern luaopen_base func_luaopen_base;

typedef void (*luaopen_bit)(lua_State*);
const Symbol luaopen_bitSym = Symbol::FromName("luaopen_bit");
extern luaopen_bit func_luaopen_bit;

typedef void (*luaopen_debug)(lua_State*);
const Symbol luaopen_debugSym = Symbol::FromName("luaopen_debug");
extern luaopen_debug func_luaopen_debug;

typedef void (*luaopen_jit)(lua_State*);
const Symbol luaopen_jitSym = Symbol::FromName("luaopen_jit");
extern luaopen_jit func_luaopen_jit;

typedef void (*luaopen_math)(lua_State*);
const Symbol luaopen_mathSym = Symbol::FromName("luaopen_math");
extern luaopen_math func_luaopen_math;

typedef void (*luaopen_os)(lua_State*);
const Symbol luaopen_osSym = Symbol::FromName("luaopen_os");
extern luaopen_os func_luaopen_os;

typedef void (*luaopen_package)(lua_State*);
const Symbol luaopen_packageSym = Symbol::FromName("luaopen_package");
extern luaopen_package func_luaopen_package;

typedef void (*luaopen_string)(lua_State*);
const Symbol luaopen_stringSym = Symbol::FromName("luaopen_string");
extern luaopen_string func_luaopen_string;

typedef void (*luaopen_table)(lua_State*);
const Symbol luaopen_tableSym = Symbol::FromName("luaopen_table");
extern luaopen_table func_luaopen_table;

/*
	server_srv stuff
*/
typedef void (*InitLuaLibraries)(ILuaInterface*);
const Symbol InitLuaLibrariesSym = Symbol::FromName("_Z16InitLuaLibrariesP13ILuaInterface");
extern InitLuaLibraries func_InitLuaLibraries;

typedef void (*InitLuaClasses)(ILuaInterface*);
const Symbol InitLuaClassesSym = Symbol::FromName("_Z14InitLuaClassesP13ILuaInterface");
extern InitLuaClasses func_InitLuaClasses;

extern void Symbols_Init();