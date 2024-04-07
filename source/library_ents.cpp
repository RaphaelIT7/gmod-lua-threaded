#include "lua_threaded.h"
#include <player.h>
#include <game/server/ientityinfo.h>

/* class IEntityInfo; */
static const char entityinfomanager_name[] = "EntityInfoManager001";

IEntityInfoManager* GetInfoManager()
{
	static IEntityInfoManager *ientityinfo_pointer = nullptr;
	if (ientityinfo_pointer == nullptr)
	{
		SourceSDK::FactoryLoader server_loader("server");
		auto entity_info_manager = server_loader.GetInterface<IEntityInfoManager>(
			entityinfomanager_name
		);

        if (entity_info_manager != nullptr)
            ientityinfo_pointer = entity_info_manager;
	}

	return ientityinfo_pointer;
}

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
        CBaseEntity* entity = CBaseEntity::Instance(entityInfoManager->FindEntityByName(NULL, classname));
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
    if (entityInfoManager == nullptr)
	{
		entityInfoManager = GetInfoManager();
	}

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->CreateTable();
            Add_Func(LUA, ents_Create, "Create");
            Add_Func(LUA, ents_FindEntityByName, "FindByName");
            
        LUA->SetField(-2, "ents");
    LUA->Pop();
}