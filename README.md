# gmod-lua-threaded

WIP  
A side project I work on to learn more about Lua, and It's API for my [gmod-lua-shared project](https://github.com/RaphaelIT7/gmod-lua-shared)  

This project aims to provide all necessary functions to run Lua on different threads.  
The Goal is to be able to create a ILuaInterface with all of Gmod's libraries, functions and classes.  

NOTE:  
This only works on Linux because on Windows creating a ILuaInterface on another thread currently crashes the game.  

## What is implemented
- [x] Shared Table  
- [x] Better Error handling(It loved to crash on any Lua error.)  

## ToDo/Testing
- [x] Created ILuaInterface should load all Gmod scripts/allow one to do so  
- [x] Change ILuaAction to use Enums instead of strings.  
- [x] Documentation  
- [ ] Add all Gmod Enums  
- [ ] Recreate all Gmod libraries  
- [ ] Look again into Error handling. Maybe show which file had an error?

I listed all Libraries below that are serverside. Regardless if they are implemented in lua or in c++.  
- - [ ] Global(functions)  
- - [ ] ai  
- - [ ] ai_schedule  
- - [ ] ai_task  
- - [ ] baseclass  
- - [x] bit - Implemented by Lua  
- - [ ] cleanup  
- - [ ] concommand  
- - [ ] constraint  
- - [ ] construct  
- - [ ] cookie  
- - [x] coroutine - Implemented by Lua  
- - [ ] cvars  
- - [x] debug (Verify)  
- - [ ] debugoverlay  
- - [ ] drive?  
- - [ ] duplicator  
- - [ ] effects  
- - [ ] engine  
- - [ ] ents  
- - [ ] file  
- - [ ] game  
- - [ ] gameevent  
- - [ ] gamemode  
- - [ ] gmod  
- - [ ] gmsave  
- - [ ] hammer?  
- - [ ] hook  
- - [ ] http  
- - [x] jit (Verify)  
- - [ ] list  
- - [ ] math  
- - [ ] motionsensor  
- - [ ] navmesh  
- - [ ] net  
- - [ ] numpad  
- - [x] os (Verify)  
- - [x] package (Verify)  
- - [ ] physenv  
- - [ ] player  
- - [ ] player_manager  
- - [ ] properties  
- - [ ] resource  
- - [ ] saverestore  
- - [ ] scripted_ents  
- - [ ] sound  
- - [ ] sql  
- - [ ] string  
- - [ ] system  
- - [ ] table  
- - [ ] team  
- - [ ] timer  
- - [ ] umsg  
- - [ ] undo  
- - [ ] usermessage  
- - [ ] utf8  
- - [ ] util  
- - [ ] weapons  
- - [ ] widgets
- [ ] Recreate all Gmod metatable functions  
- [ ] Cleanup the code  
- [ ] Make this compatible for x86-64  
- [ ] Look again into Windows  

# API

## LuaThreaded
### LuaThreaded.GetAllInterfaces()
Returns a table containing all Interfaces created.  
`[ID] = ILuaInterface (Class)`

### LuaThreaded.GetInterface(number ID)
Returns the Interface with the given ID.  

Args:  
1. ID of the given Interface  

### LuaThreaded.CreateInterface(boolean not_threaded = false)
Creates a new Interface.  

Args:  
1. If the Created Interface should run on the main thread or not.  

### LuaThreaded.CloseInterface(ILuaInterface interface)
Closes the given Interface.  

Args:  
1. The Interface to close.  

### LuaThreaded.Msg(string msg)
Prints a Message.  
This function may be removed in the future!  

Args:
1. The string to print.  

### LuaThreaded.GetTable()
Returns the Shared table.  
NOTE: You need to call this function to get an updated version!  

### LuaThreaded.SetValue(string key, any value)
Sets a Value inside the shared table.  

Args:  
1. The Key inside the Table  
2. The Value to set. Allowed Types: Number, Bool, String, Vector, Angle and Table  

## ILuaInterface
### ILuaInterface:RunString(string code)
Runs the given code  

Args:  
1. Code to run.  

### ILuaInterface:InitClasses()  
Initializes all Classes.  
This function may be removed in the future!  

### ILuaInterface:InitLibraries()  
Initializes all Libraries.  
This function may be removed in the future!  

### ILuaInterface:LoadFunction(string sig)  
Loads a function into the Interface.  
NOTE: This is unsafe and can cause random crashes.  
This function **will** be removed in the future!  

Args:  
1. Signature of the Function to load.  

### ILuaInterface:Autorun()
Loads everything like autorun does.  
NOTE: It currently loves to crash. Just call RunFile with init.lua  

Loads:  
- `includes/init.lua`
- `autorun/server/*.lua`
- `autorun/server/sensorbones/*.lua`

### ILuaInterface:RunFile(string path)
Runs the given File.  
This function could be renamed in the future!  

Args:  
1. Path to the file.  
NOTE: This will use the `GAME` search path!  

# Structure

## CLuaGameCallback class
Creates the CLuaGameCallback class and implements all functions.  

Files:  
- `CLuaGameCallback.h`
- `CLuaGameCallback.cpp`


## detours
Contains all signatures and functions we need from Gmod.  

Files:  
- `detours.h`
- `detours.cpp`

## lua_ILuaInterface
Contains all ILuaInterface metatable functions.  

Files:  
- `lua_ILuaInterface.h`
- `lua_ILuaInterface.cpp`

## lua_LuaThread
Contains our LuaThread table and all its functions.  

Files:  
- `lua_LuaThread.h`
- `lua_LuaThread.cpp`

## lua_utils
Contains important functions PushValue, FillValue and so on.  

Files:  
- `lua_utils.h`
- `lua_utils.cpp`

## lua_threaded
Contains module base and manages everything for Init and Shutdown.  

Files:  
- `lua_threaded.h`
- `lua_threaded.cpp`