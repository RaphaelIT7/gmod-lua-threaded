#include "lua_threaded.h"
#include <player.h>
#include <game/server/ientityinfo.h>

/* class IEntityInfo; */
static const char entityinfomanager_name[] = "EntityInfoManager001";

IEntityInfoManager* GetInfoManager()
{
    Msg("WILL DEFINE VARS ###############\n");

	static IEntityInfoManager *ientityinfo_pointer = nullptr;
	if (ientityinfo_pointer == nullptr)
	{
		SourceSDK::FactoryLoader server_loader("server");
        
		auto entity_info_manager = (IEntityInfoManager *)server_loader(INTERFACEVERSION_ENTITYINFOMANAGER,NULL);

        if (entity_info_manager != nullptr) {
            Msg("EntityInfoManager found\n");
            ientityinfo_pointer = entity_info_manager;

        }
        else
        {
            Msg("EntityInfoManager not found\n");
        }
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
        const char* name = LUA->GetString(1);
        if (entityInfoManager != NULL) {
            CBaseEntity* entity = CBaseEntity::Instance(entityInfoManager->FindEntityByName(NULL, name));
            if (entity != NULL) {
                Push_Entity(LUA, entity);
                
                return 1;
            } else {
                // Entity not found
            }
        }
    } else {
        LUA->ThrowError("Invalid name");
    }
    
    Push_Entity(LUA, NULL);

    return  1;
}

LUA_FUNCTION(ents_FindByClass)
{
    if (LUA->CheckString(1)) {
        LUA->CreateTable();

        const char* classname = LUA->GetString(1);
        if (entityInfoManager != NULL) {
            edict_t* entity = entityInfoManager->FindEntityByClassname(NULL, classname);

            Msg("iterating through entities\n");
            int i = 1;
            while (entity != NULL) {
                Push_Entity(LUA, CBaseEntity::Instance(entity));
                //LUA->SetField(-2, i);
                i++;
                Msg("iterating through entities");
                Msg(entity->GetClassName());
                Msg("\n");
                entity = entityInfoManager->FindEntityByClassname(entity, classname);
            }

            return 1;
        }

        Msg("no entity info manager\n");


        return 1;
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
            Add_Func(LUA, ents_FindByClass, "FindByClass");
            
        LUA->SetField(-2, "ents");
    LUA->Pop();
}