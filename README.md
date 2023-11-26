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
- [ ] Created ILuaInterface should load all Gmod scripts/allow one to do so  
- [ ] Documentation  
- [ ] Recreate all Gmod libraries  

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
- - [ ] os (Verify)  
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