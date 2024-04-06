#include "lua_threaded.h"
#include <player.h>

LUA_FUNCTION(ents_Create)
{
    const char* classname = LUA->CheckString(1);
    
    //LUA->PushMetaTable(1);

    return  0; // 1;
}

LUA_FUNCTION(ents_FindEntityByName)
{
    if (LUA->CheckString(1)) {
        const char* classname = LUA->GetString(1);
        // Use the FindEntityByName function
        CBaseEntity* entity = gEntList.FindEntityByName(NULL, classname);
        if (entity != NULL) {
            // Entity found, do something with it...
            printf("Entity found: %s\n", entity->GetClassname());
        } else {
            // Entity not found
        }
    } else {
        LUA->ThrowError("Invalid classname");
    }
    
    return  0;
}

void InitEntsLib(GarrysMod::Lua::ILuaInterface* LUA)
{
    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->CreateTable();
            Add_Func(LUA, ents_Create, "Create");
            Add_Func(LUA, ents_FindEntityByName, "FindByName");
            
        LUA->SetField(-2, "ents");
    LUA->Pop();
}
