#include <GarrysMod/Symbol.hpp>
#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/FactoryLoader.hpp>
#include <scanning/symbolfinder.hpp>
#include <filesystem.h>
#include <vector>

using namespace GarrysMod::Lua;

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

typedef int (*TLuaPanic)(lua_State*);
extern TLuaPanic func_LuaPanic;
const Symbol LuaPanicSym = Symbol::FromName("_ZL8LuaPanicP9lua_State");

typedef void (*Tlua_atpanic)(lua_State*, CFunc);
extern Tlua_atpanic func_lua_atpanic;
const Symbol lua_atpanicSym = Symbol::FromName("lua_atpanic");

typedef int (*Tlua_pcall)(lua_State*, int, int, int);
extern Tlua_pcall func_lua_pcall;
const Symbol lua_pcallSym = Symbol::FromName("lua_pcall");

typedef int (*luaL_loadstring)(lua_State*, const char*);
extern luaL_loadstring func_luaL_loadstring;
const Symbol luaL_loadstringSym = Symbol::FromName("luaL_loadstring");

typedef const char* (*lua_tostring)(lua_State*, int, int);
extern lua_tostring func_lua_tostring;
const Symbol lua_tostringSym = Symbol::FromName("lua_tolstring");

typedef const char* (*GMOD_LoadBinaryModule)(lua_State*, const char*);
extern GMOD_LoadBinaryModule func_GMOD_LoadBinaryModule;
const Symbol GMOD_LoadBinaryModuleSym = Symbol::FromName("GMOD_LoadBinaryModule");

/*
	server_srv stuff
*/
typedef void (*TInitLuaLibraries)(ILuaInterface*);
extern TInitLuaLibraries func_InitLuaLibraries;
const Symbol InitLuaLibrariesSym = Symbol::FromName("_Z16InitLuaLibrariesP13ILuaInterface");

typedef void (*InitLuaClasses)(ILuaInterface*);
extern InitLuaClasses func_InitLuaClasses;
const Symbol InitLuaClassesSym = Symbol::FromName("_Z14InitLuaClassesP13ILuaInterface");

extern void* g_pGlobalLuaLibraryFactory;
const Symbol g_pGlobalLuaLibraryFactorySym = Symbol::FromName("_ZL26g_pGlobalLuaLibraryFactory");

typedef void (*CLuaGlobalLibrary_InitLibraries)(void*, ILuaInterface*);
extern CLuaGlobalLibrary_InitLibraries func_CLuaGlobalLibrary_InitLibraries;
const Symbol CLuaGlobalLibrary_InitLibrariesSym = Symbol::FromName("_ZN17CLuaGlobalLibrary13InitLibrariesEP13ILuaInterface");

typedef void (*CLuaGameEnums_InitLibraries)(void*, ILuaInterface*);
extern CLuaGameEnums_InitLibraries func_CLuaGameEnums_InitLibraries;
const Symbol CLuaGameEnums_InitLibrariesSym = Symbol::FromName("_ZN13CLuaGameEnums13InitLibrariesEP13ILuaInterface");

typedef bool (*ConCommand_IsBlocked)(const char*);
extern ConCommand_IsBlocked func_ConCommand_IsBlocked;
const Symbol ConCommand_IsBlockedSym = Symbol::FromName("_Z20ConCommand_IsBlockedPKc");

typedef void* (*UTIL_GetCommandClient)();
extern UTIL_GetCommandClient func_UTIL_GetCommandClient;
const Symbol UTIL_GetCommandClientSym = Symbol::FromName("_Z21UTIL_GetCommandClientv");

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

extern IFileSystem* gpFileSystem;

extern void Symbols_Init();
extern void* FindSymbol(std::string);

extern void* GetFunction(SourceSDK::ModuleLoader, const char*, Symbol);
extern void* GetFunction(SourceSDK::ModuleLoader, const char*, std::vector<Symbol>);

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