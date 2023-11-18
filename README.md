# gmod-lua-threaded

This project aims to add a multithreading library to Gmod.  

## What is implemented
- [x] Lua Panics handled  

## ToDo/Testing
- [ ] Shared Table  
- [ ] Add all Gmod libraries (Currently crashes upon loading it. Same crash as print)  
- [ ] Print should work (Currently crashes inside GMOD_Print `lua_shared.so`)  
- [ ] Created ILuaInterface should load all Gmod scripts/allow one to do so´ 