#include "detours.h"
#include <GarrysMod/ModuleLoader.hpp>
#include <scanning/symbolfinder.hpp>

CreateLuaInterface func_CreateLuaInterface;
CloseLuaInterface func_CloseLuaInterface;

template<class T>
void CheckFunction(T func, const char* name)
{
	if (func == nullptr) {
		//Msg("Could not locate %s symbol!", name);
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
}