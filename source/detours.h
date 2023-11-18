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

typedef void (*luaL_openlibs)(lua_State*);
const Symbol luaL_openlibsSym = Symbol::FromName("luaL_openlibs");
extern luaL_openlibs func_luaL_openlibs;

typedef const char* (*lua_tostring)(lua_State*, int);
const Symbol lua_tostringSym = Symbol::FromName("lua_tostring");
extern lua_tostring func_lua_tostring;

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