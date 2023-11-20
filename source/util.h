#pragma once

#include "GarrysMod/Lua/Interface.h"
#include <string>
#include "detours.h"
#include <GarrysMod/FactoryLoader.hpp>

using namespace GarrysMod::Lua;

extern GarrysMod::Lua::ILuaBase* GlobalLUA;
extern void Start_Table();

extern void Add_Func(ILuaInterface*, CFunc, const char*);
extern void Add_Func(CFunc, const char*);
extern void Add_Func(CFunc, std::string);

extern void Finish_Table(const char*);
extern void Finish_Table(std::string);

extern void LuaPrint(const char*);
extern void LuaPrint(std::string);

#include <scanning/symbolfinder.hpp>
static SymbolFinder symbol_finder;
template<class T>
static inline T* ResolveSymbol(
	SourceSDK::FactoryLoader& loader, const Symbol& symbol
)
{
	if (symbol.type == Symbol::Type::None)
		return nullptr;

#if defined SYSTEM_WINDOWS

	auto iface = reinterpret_cast<T**>(symbol_finder.Resolve(
		loader.GetModule(), symbol.name.c_str(), symbol.length
	));
	return iface != nullptr ? *iface : nullptr;

#elif defined SYSTEM_POSIX

	return reinterpret_cast<T*>(symbol_finder.Resolve(
		loader.GetModule(), symbol.name.c_str(), symbol.length
	));

#endif

}

template<class T>
static inline T* ResolveSymbols(
	SourceSDK::FactoryLoader& loader, const std::vector<Symbol>& symbols
)
{
	T* iface_pointer = nullptr;
	for (const auto& symbol : symbols)
	{
		iface_pointer = ResolveSymbol<T>(loader, symbol);
		if (iface_pointer != nullptr)
			break;
	}

	return iface_pointer;
}