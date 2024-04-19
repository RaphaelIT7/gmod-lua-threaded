#include "lua_threaded.h"
#include <player.h>
#include <entitylist_base.h>

CBaseEntityList* GetEntityList()
{
	static CBaseEntityList *EntityList_pointer = nullptr;
	if (EntityList_pointer == nullptr) {
		SourceSDK::FactoryLoader server_loader( "server" );
        EntityList_pointer = ResolveSymbol<CBaseEntityList>(
            server_loader, Symbol::FromName("g_pEntityList")
        );
		if (EntityList_pointer == nullptr)
			Msg("unable to initialize IEntityInfoManager");
	}

	return EntityList_pointer;
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

        if (gpEntityList != nullptr) {
            CBaseEntity* entity = gpEntityList->FirstEntity();
            while (entity != nullptr) {
                if (strcmp(entity->GetEntityName(), name) == 0) {
                    Push_Entity(LUA, entity);
                    
                    return 1;
                }
                entity = gpEntityList->NextEntity(entity);
            }
        } else {
            LUA->ThrowError("Entity list is null");
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

        if (gpEntityList != nullptr) {
            CBaseEntity* entity = gpEntityList->FirstEntity();
            int i = 1;
            while (entity != nullptr) {
                if (strcmp(entity->GetClassname(), classname) == 0) {
                    Push_Entity(LUA, entity);
                    LUA->SetField(-2, i);
                    i++;
                }
                entity = gpEntityList->NextEntity(entity);
            }

            return 1;
        }

        Msg("no entity listr\n");


        return 1;
    } else {
        LUA->ThrowError("Invalid classname");
    }

    return  0;
}

void InitEntsLib(GarrysMod::Lua::ILuaInterface* LUA)
{
    if (gpEntityList == nullptr)
	{
		gpEntityList = GetEntityList();
	}

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->CreateTable();
            Add_Func(LUA, ents_Create, "Create");
            Add_Func(LUA, ents_FindEntityByName, "FindByName");
            Add_Func(LUA, ents_FindByClass, "FindByClass");
            
        LUA->SetField(-2, "ents");
    LUA->Pop();
}