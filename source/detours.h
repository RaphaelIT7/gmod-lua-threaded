#pragma once

#include "GarrysMod/Lua/Interface.h"
#include <GarrysMod/Lua/LuaInterface.h>
#include <scanning/symbolfinder.hpp>
#include <GarrysMod/Symbol.hpp>
#include <detouring/hook.hpp>
#include <vector>

using namespace GarrysMod::Lua;

//---------------------------------------------------------------------------------
// Some Detours
//---------------------------------------------------------------------------------

typedef bool (*InitLuaClasses)(GarrysMod::Lua::ILuaInterface*);
extern InitLuaClasses func_InitLuaClasses;
const Symbol InitLuaClassesSym = Symbol::FromName("_Z14InitLuaClassesP13ILuaInterface"); // Our signature for InitLuaClasses

typedef void (*CLuaLibrary_CLuaLibrary)(void*, const char*);
const Symbol CLuaLibrary_CLuaLibrarySym = Symbol::FromName("_ZN11CLuaLibraryC1EPKc");

struct CLuaLibraryFunction {
    const char* name;
    CFunc function;
};

typedef void (*CLuaLibrary_Add)(void*, CLuaLibraryFunction*);
const Symbol CLuaLibrary_AddSym = Symbol::FromName("_ZN11CLuaLibrary3AddEP19CLuaLibraryFunction");

extern void AddLibraries(ILuaInterface*);

//---------------------------------------------------------------------------------
// Some Functions
//---------------------------------------------------------------------------------

/*
	lua_shared Symbols
*/
typedef ILuaInterface* (*CreateLuaInterface)(bool);
extern CreateLuaInterface func_CreateLuaInterface;
const std::vector<Symbol> CreateLuaInterfaceSym = {
	Symbol::FromName("_Z18CreateLuaInterfaceb"),
	Symbol::FromSignature("\x55\x8B\xEC\x56\x57\x8B\x7D\x08\x8B\xF1\x3B\xF7\x74\x38\xE8\x0D"),
};

typedef void (*CloseLuaInterface)(ILuaInterface*);
extern CloseLuaInterface func_CloseLuaInterface;
const std::vector<Symbol> CloseLuaInterfaceSym = {
	Symbol::FromName("_Z17CloseLuaInterfaceP13ILuaInterface"),
	Symbol::FromSignature("\x55\x8B\xEC\x53\x56\x8B\xF1\x57\x8B\x7D\x08\xC7\x46\x10"),
};

typedef int (*luaL_loadstring)(lua_State*, const char*);
extern luaL_loadstring func_luaL_loadstring;
const Symbol luaL_loadstringSym = Symbol::FromName("luaL_loadstring");

typedef const char* (*lua_tostring)(lua_State*, int, int);
extern lua_tostring func_lua_tostring;
const Symbol lua_tostringSym = Symbol::FromName("lua_tolstring");

/*
	server_srv stuff
*/
typedef void (*TInitLuaLibraries)(ILuaInterface*);
extern TInitLuaLibraries func_InitLuaLibraries;
const Symbol InitLuaLibrariesSym = Symbol::FromName("_Z16InitLuaLibrariesP13ILuaInterface");

extern void* g_pGlobalLuaLibraryFactory;
const Symbol g_pGlobalLuaLibraryFactorySym = Symbol::FromName("_ZL26g_pGlobalLuaLibraryFactory");

typedef void (*CLuaGlobalLibrary_InitLibraries)(void*, ILuaInterface*);
extern CLuaGlobalLibrary_InitLibraries func_CLuaGlobalLibrary_InitLibraries;
const Symbol CLuaGlobalLibrary_InitLibrariesSym = Symbol::FromName("_ZN17CLuaGlobalLibrary13InitLibrariesEP13ILuaInterface");

/*
	CLuaGameCallback stuff
*/
typedef ILuaObject* (*CLuaGameCallback_CreateLuaObject)(void*);
extern CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;
const std::vector<Symbol> CLuaGameCallback_CreateLuaObjectSym = {
	Symbol::FromName("_ZN16CLuaGameCallback15CreateLuaObjectEv"),
	Symbol::FromSignature("\x56\x8B\xF1\x57\x8B\x16\x8B\x7A\x08\x80\x7F\x0D\x00\x74\x22\x8B"), // ToDo: Verify it.
};

typedef void (*CLuaGameCallback_DestroyLuaObject)(void*, ILuaObject*);
extern CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;
const std::vector<Symbol> CLuaGameCallback_DestroyLuaObjectSym = {
	Symbol::FromName("_ZN16CLuaGameCallback16DestroyLuaObjectEP10ILuaObject"),
	Symbol::FromSignature("\x55\x8B\xEC\x56\x8B\x75\x08\x85\xF6\x74\x12\x8B\xCE\xE8\x5E\x17"),
};

typedef void (*CLuaGameCallback_ErrorPrint)(void*, const char*, bool);
extern CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;
const std::vector<Symbol> CLuaGameCallback_ErrorPrintSym = {
	Symbol::FromName("_ZN16CLuaGameCallback10ErrorPrintEPKcb"),
	Symbol::FromSignature("\x55\x8B\xEC\xA1\x7C\xE7\x9E\x10\x8B\x50\x48\x8D\x48\x48\x85\xD2"), // Windows 32x
};

typedef void (*CLuaGameCallback_LuaError)(void*, void*);
extern CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;
const std::vector<Symbol> CLuaGameCallback_LuaErrorSym = {
	Symbol::FromName("_ZN16CLuaGameCallback8LuaErrorEP9CLuaError"),
	Symbol::FromSignature("\x55\x8B\xEC\x81\xEC\xB8\x01\x00\x00\x53\x8B\x5D\x08\x8D\x45\xFE"),
};

typedef void (*CLuaGameCallback_Msg)(void*, const char*, bool);
extern CLuaGameCallback_Msg func_CLuaGameCallback_Msg;
const std::vector<Symbol> CLuaGameCallback_MsgSym = {
	Symbol::FromName("_ZN16CLuaGameCallback3MsgEPKcb"),
	Symbol::FromSignature("\x55\x8B\xEC\x8B\x01\xC7\x45\x0C\x60\x67\x93\x10\x5D\xFF\x60\x10"),
};

typedef void (*CLuaGameCallback_MsgColour)(void*, const char*, const Color&);
extern CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;
const std::vector<Symbol> CLuaGameCallback_MsgColourSym = {
	Symbol::FromName("_ZN16CLuaGameCallback9MsgColourEPKcRK5Color"),
	Symbol::FromSignature("\x55\x8B\xEC\xFF\x75\x08\x68\x30\xE6\x70\x10\xFF\x75\x0C\x6A\x00"),
};

//---------------------------------------------------------------------------------
// Expose/Create everything else
//---------------------------------------------------------------------------------

extern bool pre;
extern void Detours_Init();
extern void Detours_PreInit();
extern void Detours_Shutdown();
extern void Detours_Think();

extern void CreateDetour(Detouring::Hook*, const char*, Detouring::Hook::Target, void*);

extern const char* funcnotfound1;
extern const char* funcnotfound2;
extern const char* funcfound1;
extern const char* funcfound2;

template<class T>
void CheckFunction(T func, const char* name)
{
	if (func == nullptr) {
		Msg("	Could not locate %s symbol!\n", name);
	}
}