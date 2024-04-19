
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

        if (g_pEntityList != nullptr) {
            CBaseHandle hEntity = g_pEntityList->FirstHandle();
            CBaseEntity* entity = static_cast<CBaseEntity*>(hEntity.Get());
            while (entity != nullptr) {
                if (strcmp(entity->GetEntityName().ToCStr(), name) == 0) {
                    Push_Entity(LUA, entity);
                    
                    return 1;
                }
                hEntity = g_pEntityList->NextHandle(hEntity);
                entity = static_cast<CBaseEntity*>(hEntity.Get());
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

        if (g_pEntityList != nullptr) {
            CBaseHandle hEntity = g_pEntityList->FirstHandle();
            CBaseEntity* entity = static_cast<CBaseEntity*>(hEntity.Get());
            int i = 1;
            while (entity != nullptr) {
                Msg("classname: %s\n", entity->GetClassname());
                if (strcmp(entity->GetClassname(), classname) == 0) {
                    Push_Entity(LUA, entity);
                    const char* index = std::to_string(i).c_str();
                    LUA->SetField(-2, index);
                    i++;
                }
                hEntity = g_pEntityList->NextHandle(hEntity);
                entity = static_cast<CBaseEntity*>(hEntity.Get());
            }

            Msg("entity listr, i = %i\n", i)

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
    if (g_pEntityList == nullptr)
	{
		g_pEntityList = GetEntityList();
	}

    LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
        LUA->CreateTable();
            Add_Func(LUA, ents_Create, "Create");
            Add_Func(LUA, ents_FindEntityByName, "FindByName");
            Add_Func(LUA, ents_FindByClass, "FindByClass");
            
        LUA->SetField(-2, "ents");
    LUA->Pop();
}