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

typedef int (*TLuaPanic)(lua_State*);
const Symbol LuaPanicSym = Symbol::FromName("_ZL8LuaPanicP9lua_State");
extern TLuaPanic func_LuaPanic;

typedef void (*Tlua_atpanic)(lua_State*, CFunc);
const Symbol lua_atpanicSym = Symbol::FromName("lua_atpanic");
extern Tlua_atpanic func_lua_atpanic;

typedef int (*TAdvancedLuaErrorReporter)(lua_State*);
const Symbol AdvancedLuaErrorReporterSym = Symbol::FromName("_Z24AdvancedLuaErrorReporterP9lua_State");
extern TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;

typedef int (*luaL_loadstring)(lua_State*, const char*);
const Symbol luaL_loadstringSym = Symbol::FromName("luaL_loadstring");
extern luaL_loadstring func_luaL_loadstring;

typedef void (*luaL_openlibs)(lua_State*);
const Symbol luaL_openlibsSym = Symbol::FromName("luaL_openlibs");
extern luaL_openlibs func_luaL_openlibs;

typedef const char* (*lua_tostring)(lua_State*, int, int);
const Symbol lua_tostringSym = Symbol::FromName("lua_tolstring");
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

/*
	CLuaGameCallback stuff
*/

typedef ILuaObject* (*CLuaGameCallback_CreateLuaObject)(void*);
const Symbol CLuaGameCallback_CreateLuaObjectSym = Symbol::FromName("_ZN16CLuaGameCallback15CreateLuaObjectEv");
extern CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;

typedef void (*CLuaGameCallback_DestroyLuaObject)(void*, ILuaObject*);
const Symbol CLuaGameCallback_DestroyLuaObjectSym = Symbol::FromName("_ZN16CLuaGameCallback16DestroyLuaObjectEP10ILuaObject");
extern CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;

typedef void (*CLuaGameCallback_ErrorPrint)(void*, const char*, bool);
const Symbol CLuaGameCallback_ErrorPrintSym = Symbol::FromName("_ZN16CLuaGameCallback10ErrorPrintEPKcb");
extern CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;

typedef void (*CLuaGameCallback_LuaError)(void*, void*);
const Symbol CLuaGameCallback_LuaErrorSym = Symbol::FromName("_ZN16CLuaGameCallback8LuaErrorEP9CLuaError");
extern CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;

typedef void (*CLuaGameCallback_Msg)(void*, const char*, bool);
const Symbol CLuaGameCallback_MsgSym = Symbol::FromName("_ZN16CLuaGameCallback3MsgEPKcb");
extern CLuaGameCallback_Msg func_CLuaGameCallback_Msg;

typedef void (*CLuaGameCallback_MsgColour)(void*, const char*, const Color&);
const Symbol CLuaGameCallback_MsgColourSym = Symbol::FromName("_ZN16CLuaGameCallback9MsgColourEPKcRK5Color");
extern CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;

extern void Symbols_Init();