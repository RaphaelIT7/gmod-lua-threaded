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

extern void Symbols_Init();