# gmod-lua-threaded

This project aims to add a multithreading library to Gmod.  
The Goal is to be able to create a ILuaInterface with all of Gmod's libraries, functions and classes.  

NOTE:  
This only works on Linux because on Windows creating a ILuaInterface on another thread currently crashes the game.  

## What is implemented
- [x] print should work  
- [x] Shared Table  
- [x] Better Error handling(It loved to crash on any Lua error.)  

## ToDo/Testing
- [ ] Created ILuaInterface should load all Gmod scripts/allow one to do so  
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