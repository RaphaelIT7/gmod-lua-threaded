#include <GarrysMod/Symbol.hpp>
#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/FactoryLoader.hpp>
#include <scanning/symbolfinder.hpp>
#include <editor_sendcommand.h>
#include <filesystem.h>
#include <vector>

/*
	lua_shared Symbols
*/
typedef GarrysMod::Lua::ILuaInterface* (*CreateLuaInterface)(bool);
extern CreateLuaInterface func_CreateLuaInterface;
const std::vector<Symbol> CreateLuaInterfaceSym = {
	Symbol::FromName("_Z18CreateLuaInterfaceb"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\x56\x57\x8B\x7D\x08\x8B\xF1\x3B\xF7\x74\x38\xE8\x0D"),
#endif
};

typedef void (*CloseLuaInterface)(GarrysMod::Lua::ILuaInterface*);
extern CloseLuaInterface func_CloseLuaInterface;
const std::vector<Symbol> CloseLuaInterfaceSym = {
	Symbol::FromName("_Z17CloseLuaInterfaceP13ILuaInterface"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\x53\x56\x8B\xF1\x57\x8B\x7D\x08\xC7\x46\x10"),
#endif
};

typedef int (*TLuaPanic)(lua_State*);
extern TLuaPanic func_LuaPanic;
const Symbol LuaPanicSym = Symbol::FromName("_ZL8LuaPanicP9lua_State");

typedef void (*Tlua_atpanic)(lua_State*, GarrysMod::Lua::CFunc);
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

typedef const char* (*llua_getstack)(lua_State*, int, lua_Debug*);
extern llua_getstack func_lua_getstack;
const Symbol lua_getstackSym = Symbol::FromName("lua_getstack");

typedef const char* (*llua_getinfo)(lua_State*, const char*, lua_Debug*);
extern llua_getinfo func_lua_getinfo;
const Symbol lua_getinfoSym = Symbol::FromName("lua_getinfo");

typedef const char* (*llua_pushstring)(lua_State*, const char*);
extern llua_pushstring func_lua_pushstring;
const Symbol lua_pushstringSym = Symbol::FromName("lua_pushstring");

typedef const char* (*llua_setfield)(lua_State*, int, const char*);
extern llua_setfield func_lua_setfield;
const Symbol lua_setfieldSym = Symbol::FromName("lua_setfield");

typedef int (*luaL_loadbuffer)(lua_State*, const char*, size_t, const char*);
extern luaL_loadbuffer func_luaL_loadbuffer;
const Symbol luaL_loadbufferSym = Symbol::FromName("luaL_loadbuffer");

typedef const char* (*GMOD_LoadBinaryModule)(lua_State*, const char*);
extern GMOD_LoadBinaryModule func_GMOD_LoadBinaryModule;
const Symbol GMOD_LoadBinaryModuleSym = Symbol::FromName("GMOD_LoadBinaryModule");

typedef int (*TAdvancedLuaErrorReporter)(lua_State*);
extern TAdvancedLuaErrorReporter func_AdvancedLuaErrorReporter;
const Symbol AdvancedLuaErrorReporterSym = Symbol::FromName("_Z24AdvancedLuaErrorReporterP9lua_State");

typedef void (*CLuaInterface_DoStackCheck)(void* funnies);
const Symbol CLuaInterface_DoStackCheckSym = Symbol::FromName("_ZN13CLuaInterface12DoStackCheckEv");


/*
	server_srv stuff
*/
typedef void (*TInitLuaLibraries)(GarrysMod::Lua::ILuaInterface*);
extern TInitLuaLibraries func_InitLuaLibraries;
const Symbol InitLuaLibrariesSym = Symbol::FromName("_Z16InitLuaLibrariesP13ILuaInterface");

typedef void (*InitLuaClasses)(GarrysMod::Lua::ILuaInterface*);
extern InitLuaClasses func_InitLuaClasses;
const Symbol InitLuaClassesSym = Symbol::FromName("_Z14InitLuaClassesP13ILuaInterface");

extern void* g_pGlobalLuaLibraryFactory;
const Symbol g_pGlobalLuaLibraryFactorySym = Symbol::FromName("_ZL26g_pGlobalLuaLibraryFactory");

typedef void (*CLuaGlobalLibrary_InitLibraries)(void*, GarrysMod::Lua::ILuaInterface*);
extern CLuaGlobalLibrary_InitLibraries func_CLuaGlobalLibrary_InitLibraries;
const Symbol CLuaGlobalLibrary_InitLibrariesSym = Symbol::FromName("_ZN17CLuaGlobalLibrary13InitLibrariesEP13ILuaInterface");

typedef void (*CLuaGameEnums_InitLibraries)(void*, GarrysMod::Lua::ILuaInterface*);
extern CLuaGameEnums_InitLibraries func_CLuaGameEnums_InitLibraries;
const Symbol CLuaGameEnums_InitLibrariesSym = Symbol::FromName("_ZN13CLuaGameEnums13InitLibrariesEP13ILuaInterface");

typedef bool (*ConCommand_IsBlocked)(const char*);
extern ConCommand_IsBlocked func_ConCommand_IsBlocked;
const Symbol ConCommand_IsBlockedSym = Symbol::FromName("_Z20ConCommand_IsBlockedPKc");

typedef void* (*TUTIL_GetCommandClient)();
extern TUTIL_GetCommandClient func_UTIL_GetCommandClient;
const Symbol UTIL_GetCommandClientSym = Symbol::FromName("_Z21UTIL_GetCommandClientv");

typedef EditorSendResult_t (*TEditor_SendCommand)(const char*, bool);
extern TEditor_SendCommand func_Editor_SendCommand;
const std::vector<Symbol> Editor_SendCommandSym = {
	Symbol::FromName("_Z18Editor_SendCommandPKcb"),
};

typedef void* (*TGMOD_LoadParticleConfigFile)(const char*, bool);
extern TGMOD_LoadParticleConfigFile func_GMOD_LoadParticleConfigFile;
const Symbol GMOD_LoadParticleConfigFileSym = Symbol::FromName("_Z27GMOD_LoadParticleConfigFilePKcb");

typedef void* (*TGetAmmoDef)();
extern TGetAmmoDef func_GetAmmoDef;
const Symbol GetAmmoDefSym = Symbol::FromName("_Z10GetAmmoDefv");

const Symbol CGameRules_Sym = Symbol::FromName("g_pGameRules");
const Symbol CUserMessages_Sym = Symbol::FromName("usermessages");
const Symbol CTeamList_Sym = Symbol::FromName("g_Teams");

/*
	CLuaGameCallback stuff
*/
typedef GarrysMod::Lua::ILuaObject* (*CLuaGameCallback_CreateLuaObject)(void*);
extern CLuaGameCallback_CreateLuaObject func_CLuaGameCallback_CreateLuaObject;
const std::vector<Symbol> CLuaGameCallback_CreateLuaObjectSym = {
	Symbol::FromName("_ZN16CLuaGameCallback15CreateLuaObjectEv"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x56\x8B\xF1\x57\x8B\x16\x8B\x7A\x08\x80\x7F\x0D\x00\x74\x22\x8B"), // ToDo: Verify it.
#endif
};

typedef void (*CLuaGameCallback_DestroyLuaObject)(void*, GarrysMod::Lua::ILuaObject*);
extern CLuaGameCallback_DestroyLuaObject func_CLuaGameCallback_DestroyLuaObject;
const std::vector<Symbol> CLuaGameCallback_DestroyLuaObjectSym = {
	Symbol::FromName("_ZN16CLuaGameCallback16DestroyLuaObjectEP10ILuaObject"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\x56\x8B\x75\x08\x85\xF6\x74\x12\x8B\xCE\xE8\x5E\x17"),
#endif
};

typedef void (*CLuaGameCallback_ErrorPrint)(void*, const char*, bool);
extern CLuaGameCallback_ErrorPrint func_CLuaGameCallback_ErrorPrint;
const std::vector<Symbol> CLuaGameCallback_ErrorPrintSym = {
	Symbol::FromName("_ZN16CLuaGameCallback10ErrorPrintEPKcb"),
	
#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\xA1\x7C\xE7\x9E\x10\x8B\x50\x48\x8D\x48\x48\x85\xD2"), // Windows 32x
#endif
};

typedef void (*CLuaGameCallback_LuaError)(void*, void*);
extern CLuaGameCallback_LuaError func_CLuaGameCallback_LuaError;
const std::vector<Symbol> CLuaGameCallback_LuaErrorSym = {
	Symbol::FromName("_ZN16CLuaGameCallback8LuaErrorEP9CLuaError"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\x81\xEC\xB8\x01\x00\x00\x53\x8B\x5D\x08\x8D\x45\xFE"),
#endif
};

typedef void (*CLuaGameCallback_Msg)(void*, const char*, bool);
extern CLuaGameCallback_Msg func_CLuaGameCallback_Msg;
const std::vector<Symbol> CLuaGameCallback_MsgSym = {
	Symbol::FromName("_ZN16CLuaGameCallback3MsgEPKcb"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\x8B\x01\xC7\x45\x0C\x60\x67\x93\x10\x5D\xFF\x60\x10"),
#endif
};

typedef void (*CLuaGameCallback_MsgColour)(void*, const char*, const Color&);
extern CLuaGameCallback_MsgColour func_CLuaGameCallback_MsgColour;
const std::vector<Symbol> CLuaGameCallback_MsgColourSym = {
	Symbol::FromName("_ZN16CLuaGameCallback9MsgColourEPKcRK5Color"),

#ifdef SYSTEM_WINDOWS
	Symbol::FromSignature("\x55\x8B\xEC\xFF\x75\x08\x68\x30\xE6\x70\x10\xFF\x75\x0C\x6A\x00"),
#endif
};

const Symbol IGet_Sym = Symbol::FromName("_ZL4cget");

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

inline bool CheckValue(const char* msg, const char* name, bool ret)
{
	if (!ret) {
		Msg("[LuaThreaded] Failed to %s %s!\n", msg, name);
		return false;
	}

	return true;
}

inline bool CheckValue(const char* name, bool ret)
{
	return CheckValue("get function", name, ret);
}

template<class T>
bool CheckFunction(T func, const char* name)
{
	return CheckValue("get function", name, func != nullptr);
}

typedef void (*DoStackCheckCallback)(void*);
extern void AddStackCheckCallback(void* key, DoStackCheckCallback func);
extern void RemoveStackCheckCallback(void* key);