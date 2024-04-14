#include "lua_threaded.h"
#include <player.h>

static int32_t metatype = GarrysMod::Lua::Type::Entity;
static const char metaname[] = "Entity";
static const char invalid_error[] = "invalid Entity";
static const char table_name[] = "Entity_object";

void Push_Entity(GarrysMod::Lua::ILuaBase* LUA, CBaseEntity* ent)
{
	LUA_Entity* udata = (LUA_Entity*)LUA->NewUserdata(sizeof(LUA_Entity));
	udata->entity = ent;

	GarrysMod::Lua::ILuaInterface* ILUA = (GarrysMod::Lua::ILuaInterface*)LUA;
    ILUA->SetType(metatype);

	LUA->CreateMetaTableType(metaname, metatype);
	LUA->SetMetaTable(-2);
}

void Push_Entity(GarrysMod::Lua::ILuaBase* LUA, LUA_Entity* ent)
{
	Push_Entity(LUA, ent->entity);
}

void Entity_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, GarrysMod::Lua::Type::Entity))
		LUA->TypeError(index, metaname);
}

bool IsEntity(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	if (LUA->IsType(index, GarrysMod::Lua::Type::Entity))
	{
		LUA->GetMetaTable(index);
		LUA->GetField(-1, "MetaName");
		if (LUA->IsType(-1, GarrysMod::Lua::Type::String))
		{
			if (strcmp(LUA->GetString(-1), metaname))
			{
				LUA->Pop(2);
				return true;
			} else {
				LUA->Pop(2);
			}
		} else {
			LUA->Pop(2);
		}
	}

	return false;
}

LUA_Entity* Entity_GetUserdata(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	return (LUA_Entity*)LUA->GetUserdata(index);
}

LUA_Entity* Entity_Get(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	Entity_CheckType(LUA, index);

	LUA_Entity* ent = Entity_GetUserdata(LUA, index);

	return ent;
}

void Entity_Destroy(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	LUA_Entity* ent = Entity_GetUserdata(LUA, index);

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
	LUA->PushUserdata(ent);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(Entity__gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	Entity_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(Entity__tostring)
{
	LUA_Entity* ent = Entity_Get(LUA, 1);

	LUA->GetMetaTable(1);
	LUA->GetField(-1, "MetaName");
	const char* name = LUA->GetString(-1);
	LUA->Pop(1);

	char szBuf[64] = {};
	V_snprintf(szBuf, sizeof(szBuf),"%s [%i][%s]", name, engine->IndexOfEdict(ent->entity->edict()), ent->entity->edict()->GetClassName()); // EntityIndex
	LUA->PushString(szBuf);
	return 1;
}

LUA_FUNCTION_STATIC(Entity__index)
{
	LUA_Entity* ent = Entity_Get(LUA, 1);
	const char* key = LUA->GetString(2);

	if (key != NULL) 
	{
		int ref = -1;
		LUA->GetMetaTable(1);
			LUA->GetField(-1, key);
				ref = LUA->ReferenceCreate();

		LUA->Pop(2);

		LUA->ReferencePush(ref);
		LUA->ReferenceFree(ref);

		return 1;
	}

	LUA->GetMetaTable(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	if(!LUA->IsType(-1, GarrysMod::Lua::Type::NIL))
		return 1;

	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->RawGet(-2);
	return 1;
}

LUA_FUNCTION_STATIC(Entity__newindex)
{
	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);
	const char* key = LUA->GetString(2);
	if (key == NULL)
		return 0;

	LUA_Entity* ent = Entity_Get(LUA, 1);
	

	return 0;
}

LUA_FUNCTION_STATIC(Entity__eq)
{
	LUA->PushBool(Entity_Get(LUA, 1) == Entity_Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(Entity_EntIndex)
{
	LUA->PushBool(Entity_Get(LUA, 1) == Entity_Get(LUA, 2));
	return 1;
}


LUA_FUNCTION(Global_Entity)
{
	double id = LUA->CheckNumber(1);

	CBaseEntity* ent = CBaseEntity::Instance(engine.PEntityOfEntIndex((int)id));

	if (ent == NULL)
	{
		LUA->PushNil();
		return 1;
	}

	Push_Entity(LUA, ent);

	return 1;
}

void InitEntityClass(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	LUA->CreateMetaTableType(metaname, metatype);
		Add_Func(LUA, Entity__gc, "__gc"); // Gmod doesn't have __gc
		Add_Func(LUA, Entity__eq, "__eq");
		Add_Func(LUA, Entity__index, "__index");
		Add_Func(LUA, Entity__newindex, "__newindex");
		Add_Func(LUA, Entity__tostring, "__tostring");
	LUA->Pop(1);
}