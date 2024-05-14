#ifndef LUA_UTIL
#define LUA_UTIL

/*
	It should be noted that CreateMetaTableType and CreateMetaTable don't act the same. 
	CreateMetaTableType is marked as deprecated, but it shouldn't be if you want to create a MetaTable for a specific TypeID like Type::Vector.
*/
#ifndef GARRYSMOD_LUA_INTERFACE_H
#define GARRYSMOD_LUA_INTERFACE_H

#define GMOD_ALLOW_DEPRECATED
#include "GarrysMod/Lua/LuaBase.h"

struct lua_State
{
#if ( defined( _WIN32 ) || defined( __linux__ ) || defined( __APPLE__ ) ) && \
    !defined( __x86_64__ ) && !defined( _M_X64 )
    // Win32, Linux32 and macOS32
    unsigned char _ignore_this_common_lua_header_[48 + 22];
#elif ( defined( _WIN32 ) || defined( __linux__ ) || defined( __APPLE__ ) ) && \
    ( defined( __x86_64__ ) || defined( _M_X64 ) )
    // Win64, Linux64 and macOS64 (not tested)
    unsigned char _ignore_this_common_lua_header_[92 + 22];
#else
    #error Unsupported platform
#endif

    GarrysMod::Lua::ILuaBase* luabase;
};

#ifndef GMOD
    #ifdef _WIN32
        #define GMOD_DLL_EXPORT extern "C" __declspec( dllexport )
    #else
        #define GMOD_DLL_EXPORT extern "C" __attribute__((visibility("default")))
    #endif

        #define GMOD_MODULE_OPEN()                                  \
            int gmod13_open__Imp( GarrysMod::Lua::ILuaBase* LUA );  \
            GMOD_DLL_EXPORT int gmod13_open( lua_State* L )         \
            {                                                       \
                return gmod13_open__Imp( L->luabase );              \
            }                                                       \
            int gmod13_open__Imp( [[maybe_unused]] GarrysMod::Lua::ILuaBase* LUA )

        #define GMOD_MODULE_CLOSE()                                 \
            int gmod13_close__Imp( GarrysMod::Lua::ILuaBase* LUA ); \
            GMOD_DLL_EXPORT int gmod13_close( lua_State* L )        \
            {                                                       \
                return gmod13_close__Imp( L->luabase );             \
            }                                                       \
            int gmod13_close__Imp( [[maybe_unused]] GarrysMod::Lua::ILuaBase* LUA )

        #define LUA_FUNCTION( FUNC )                            \
            int FUNC##__Imp( GarrysMod::Lua::ILuaBase* LUA );   \
            int FUNC( lua_State* L )                            \
            {                                                   \
                GarrysMod::Lua::ILuaBase* LUA = L->luabase;     \
                LUA->SetState(L);                               \
                return FUNC##__Imp( LUA );                      \
            }                                                   \
            int FUNC##__Imp( [[maybe_unused]] GarrysMod::Lua::ILuaBase* LUA )

        #define LUA_FUNCTION_STATIC( FUNC )                             \
            static int FUNC##__Imp( GarrysMod::Lua::ILuaBase* LUA );    \
            static int FUNC( lua_State* L )                             \
            {                                                           \
                GarrysMod::Lua::ILuaBase* LUA = L->luabase;             \
                LUA->SetState(L);                                       \
                return FUNC##__Imp( LUA );                              \
            }                                                           \
            static int FUNC##__Imp( [[maybe_unused]] GarrysMod::Lua::ILuaBase* LUA )

        #define LUA_FUNCTION_DECLARE( FUNC ) \
            int FUNC( lua_State *L )

        #define LUA_FUNCTION_STATIC_DECLARE( FUNC ) \
            static int FUNC( lua_State *L )

        #define LUA_FUNCTION_IMPLEMENT( FUNC )                                                  \
            [[deprecated("Use LUA_FUNCTION_STATIC_MEMBER instead of LUA_FUNCTION_IMPLEMENT.")]] \
            static int FUNC##__Imp( [[maybe_unused]] GarrysMod::Lua::ILuaBase* LUA )

        #define LUA_FUNCTION_WRAP( FUNC )                                                   \
            [[deprecated("Use LUA_FUNCTION_STATIC_MEMBER instead of LUA_FUNCTION_WRAP.")]]  \
            static int FUNC( lua_State *L )                                                 \
            {                                                                               \
                GarrysMod::Lua::ILuaBase* LUA = L->luabase;                                 \
                LUA->SetState(L);                                                           \
                return FUNC##__Imp( LUA );                                                  \
            }

        #define LUA_FUNCTION_STATIC_MEMBER( FUNC )                  \
            static int FUNC( lua_State* L )                         \
            {                                                       \
                GarrysMod::Lua::ILuaBase* LUA = L->luabase;         \
                LUA->SetState(L);                                   \
                return FUNC##__Imp( LUA );                          \
            }                                                       \
            static int FUNC##__Imp( GarrysMod::Lua::ILuaBase* LUA )
    #endif
#endif

#include "networkstringtabledefs.h"
#include "GameEventListener.h"
#include <unordered_map>
#include "ILuaConVars.h"
#include "detours.h"
#include "player.h" // Needs to be included before gamerules.h or everything breaks.
#include <gamerules.h>
#include <setjmp.h>
#include <eiface.h>
#include <vector>
#include <lua.h>

enum LuaAction
{
	ACT_RunFile,
	ACT_RunHook,
	ACT_RunString,
	ACT_RunCommand,
	ACT_InitClasses,
	ACT_InitLibraries,
	ACT_LoadFunc,
	ACT_Autorun,
	ACT_InitEnums,
	ACT_InitGmod,
	ACT_Gameevent,
};

struct ILuaValue
{
	unsigned char type = -1;

	double number = -1;
	const char* string = "";
	std::unordered_map<ILuaValue*, ILuaValue*> tbl;
	float x, y, z;
	void* data = nullptr; // Used for LUA_File
};

struct ILuaAction
{
	LuaAction type;
	const char* data;

	// ConCommand
	CCommand cmd;
	void* ply;

	// Gameevent
	ILuaValue* val;
};

struct ILuaTimer
{
	int function;
	const char* identifier;
	double delay = 0;
	double repetitions = 0;
	bool active = true;
	bool simple = false; // true if it's a timer.Simple
	double next_run = 0;
	double next_run_time = 0;
	bool markdelete = false;
};

struct ILuaThread;
struct IAsyncFile
{
	FileAsyncRequest_t* req;
	ILuaThread* thread;
	int callback;
	int nBytesRead;
	int status;
	bool finished = false;
	const char* content;
};

struct ILuaThread
{
	GarrysMod::Lua::ILuaInterface* IFace;
	CThreadFastMutex mutex;
	int shared_tbl_reference = -1;

	std::vector<ILuaAction*> actions;

	bool run = true;
	bool init = false;
	bool threaded = false;

	bool locked = false;
	bool waiting = false;
	
	int id = -1;

	// Error Handling
	std::string current_path;
	jmp_buf jumpBuffer; 

	// gameevent library
	IGameEventListener2* listener; 

	// timer library
	std::vector<ILuaTimer*> timers;

	// File library
	std::vector<IAsyncFile*> async;
};

struct GMOD_Info
{
	int version = 0;
	const char* versionstr = "????????";
	const char* branch = "Unknown";

	bool threadready = false;
	GarrysMod::Lua::ILuaGameCallback* gamecallback;

	// engine library
	ILuaValue* addons;
	ILuaValue* games;
	ILuaValue* gamemodes;
	ILuaValue* usercontent;

	const char* active_gamemode;
};

inline float Lerp(float delta, float from, float to)
{
    return from + (to - from) * delta;
}

inline bool EqualValue(ILuaValue* val1, ILuaValue* val2)
{
	if (val1->type != val2->type)
		return false;

	switch(val1->type)
	{
		case GarrysMod::Lua::Type::NUMBER:
			return val1->number == val2->number;
		case GarrysMod::Lua::Type::BOOL:
			return val1->number == val2->number;
		case GarrysMod::Lua::Type::STRING:
			return strcmp(val1->string, val2->string) == 0;
		case GarrysMod::Lua::Type::ENTITY: // ToDo
			return false;
		case GarrysMod::Lua::Type::VECTOR:
			return val1->x == val2->x && val1->y == val2->y && val1->z == val2->z;
		case GarrysMod::Lua::Type::ANGLE:
			return val1->x == val2->x && val1->y == val2->y && val1->z == val2->z;
		case GarrysMod::Lua::Type::File: // ToDo
			return false;
		case GarrysMod::Lua::Type::Table: // ToDo
			return false;
		default:
			break;
	}

	return false;
}

inline ILuaValue* CreateValue(int value)
{
	ILuaValue* val = new ILuaValue;
	val->type = GarrysMod::Lua::Type::Number;
	val->number = value;

	return val;
}

inline ILuaValue* CreateValue(const char* value)
{
	ILuaValue* val = new ILuaValue;
	val->type = GarrysMod::Lua::Type::String;
	val->string = value;

	return val;
}

extern GMOD_Info* GMOD;
extern int interfaces_count;
extern std::unordered_map<double, ILuaThread*> interfaces;

extern int shared_table_reference;
extern CThreadFastMutex shared_table_mutex;
extern std::unordered_map<ILuaValue*, ILuaValue*> shared_table;

extern void PushValue(GarrysMod::Lua::ILuaBase*, ILuaValue*);
extern void SafeDelete(ILuaValue*);
extern void FillValue(GarrysMod::Lua::ILuaBase*, ILuaValue*, int, int);

extern void Add_Func(GarrysMod::Lua::ILuaBase*, GarrysMod::Lua::CFunc, const char*);
extern ILuaThread* FindThread(int);

extern GarrysMod::Lua::ILuaInterface* CreateInterface();
extern void ShutdownInterface(ILuaThread*);

extern std::string ToPath(std::string);

extern void InitInterfaces();
extern IFileSystem* filesystem;
extern CGlobalVars* gpGlobals;
extern IVEngineServer* engine;
extern INetworkStringTableContainer* networkstringtables;
#endif