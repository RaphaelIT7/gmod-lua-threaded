#include "lua_threaded.h"
#include <team.h>

static int32_t metatype = GarrysMod::Lua::Type::RecipientFilter;
static const char metaname[] = "RecipientFilter";
static const char invalid_error[] = "invalid RecipientFilter";
static const char table_name[] = "RecipientFilter_object";

void Push_RecipientFilter(GarrysMod::Lua::ILuaBase* LUA, CRecipientFilter* filter, bool shoulddelete)
{
	LUA_RecipientFilter* udata = LUA->NewUserType<LUA_RecipientFilter>(metatype);
	udata->filter = filter;
	udata->shoulddelete = shoulddelete;

	GarrysMod::Lua::ILuaInterface* ILUA = (GarrysMod::Lua::ILuaInterface*)LUA;
	ILUA->SetType(GarrysMod::Lua::Type::RecipientFilter);

	LUA->CreateMetaTableType(metaname, metatype);
	LUA->SetMetaTable(-2);
}

void RecipientFilter_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, GarrysMod::Lua::Type::RecipientFilter))
		LUA->TypeError(index, metaname);
}

LUA_RecipientFilter* RecipientFilter_GetUserdata(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	return LUA->GetUserType<LUA_RecipientFilter>(index, metatype);
}

LUA_RecipientFilter* RecipientFilter_Get(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	RecipientFilter_CheckType(LUA, index);

	LUA_RecipientFilter* filter = RecipientFilter_GetUserdata(LUA, index);
	if(filter == nullptr)
		LUA->ArgError(index, invalid_error);

	return filter;
}

void RecipientFilter_Destroy(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	LUA_RecipientFilter* filter = RecipientFilter_GetUserdata(LUA, index);

	if (filter->shoulddelete)
		delete filter->filter;

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
	LUA->PushUserdata(filter);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(RecipientFilter__gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	RecipientFilter_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(RecipientFilter__tostring)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	char szBuf[64] = {};
	V_snprintf(szBuf, sizeof(szBuf),"RecipientFilter []", filter->filter != nullptr ? std::to_string(filter->filter->GetRecipientCount()).c_str() : "NULL");
	LUA->PushString(szBuf);
	return 1;
}

LUA_FUNCTION(RecipientFilter_AddAllPlayers)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->AddAllPlayers();

	return 0;
}

LUA_FUNCTION(RecipientFilter_AddPAS)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	LUA_Vector* origin = Vector_Get(LUA, 2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->AddRecipientsByPAS(Vector(origin->x, origin->y, origin->z));

	return 0;
}

LUA_FUNCTION(RecipientFilter_AddPVS)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	LUA_Vector* origin = Vector_Get(LUA, 2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->AddRecipientsByPVS(Vector(origin->x, origin->y, origin->z));

	return 0;
}

LUA_FUNCTION(RecipientFilter_AddPlayer)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// TODO
	//filter->filter->AddRecipient();

	return 0;
}

LUA_FUNCTION(RecipientFilter_AddPlayers)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// TODO
	switch (LUA->GetType(2))
	{
		case GarrysMod::Lua::Type::RecipientFilter:
			for (int i=0; i<filter->filter->GetRecipientCount(); ++i)
			{
			
			}
			break;
		case GarrysMod::Lua::Type::Table:
			break;
		default:
			break;
	}

	return 0;
}

LUA_FUNCTION(RecipientFilter_AddRecipientsByTeam)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	double teamid = LUA->GetNumber(2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// TODO
	// Gmod loops over all players and checks their team.
	//filter->filter->AddRecipientsByTeam(teamid);

	return 0;
}

LUA_FUNCTION(RecipientFilter_GetCount)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	LUA->PushNumber(filter->filter->GetRecipientCount());

	return 1;
}

LUA_FUNCTION(RecipientFilter_GetPlayers)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// ToDo

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemoveAllPlayers)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->RemoveAllRecipients();

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemoveMismatchedPlayers)
{
	LUA_RecipientFilter* filter1 = RecipientFilter_Get(LUA, 1);
	if (filter1->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	LUA_RecipientFilter* filter2 = RecipientFilter_Get(LUA, 1);
	if (filter2->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// ToDo

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemovePAS)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	LUA_Vector* origin = Vector_Get(LUA, 2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->GMOD_RemoveRecipientsByPAS(Vector(origin->x, origin->y, origin->z));

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemovePVS)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	LUA_Vector* origin = Vector_Get(LUA, 2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->RemoveRecipientsByPVS(Vector(origin->x, origin->y, origin->z));

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemovePlayer)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// ToDo
	//filter->filter->RemoveRecipient(Entity);

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemovePlayers)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	// ToDo

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemoveRecipientsByTeam)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	int teamnum = LUA->CheckNumber(2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->RemoveRecipientsByTeam(GetGlobalTeam(teamnum));

	return 0;
}

LUA_FUNCTION(RecipientFilter_RemoveRecipientsNotOnTeam)
{
	LUA_RecipientFilter* filter = RecipientFilter_Get(LUA, 1);
	int teamnum = LUA->CheckNumber(2);
	if (filter->filter == nullptr)
		LUA->ArgError(1, "Tried to use a NULL CRecipientFilter!");

	filter->filter->RemoveRecipientsNotOnTeam(GetGlobalTeam(teamnum));

	return 0;
}

LUA_FUNCTION(Global_RecipientFilter)
{
	bool unreliable = LUA->GetBool(1);
	CRecipientFilter* filter = new CRecipientFilter;
	if (!unreliable)
		filter->MakeReliable();

	Push_RecipientFilter(LUA, filter, true);

	return 1;
}

void InitRecipientFilterClass(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	LUA->CreateMetaTableType(metaname, metatype);
		Add_Func(LUA, RecipientFilter__gc, "__gc");
		Add_Func(LUA, RecipientFilter__tostring, "__tostring");

		Add_Func(LUA, RecipientFilter_AddAllPlayers, "AddAllPlayers");
		Add_Func(LUA, RecipientFilter_AddPAS, "AddPAS");
		Add_Func(LUA, RecipientFilter_AddPVS, "AddPVS");
		//Add_Func(LUA, RecipientFilter_AddPlayer, "AddPlayer");
		//Add_Func(LUA, RecipientFilter_AddPlayers, "AddPlayers");
		//Add_Func(LUA, RecipientFilter_AddRecipientsByTeam, "AddRecipientsByTeam");
		Add_Func(LUA, RecipientFilter_GetCount, "GetCount");
		//Add_Func(LUA, RecipientFilter_GetPlayers, "GetPlayers");
		Add_Func(LUA, RecipientFilter_RemoveAllPlayers, "RemoveAllPlayers");
		//Add_Func(LUA, RecipientFilter_RemoveMismatchedPlayers, "RemoveMismatchedPlayers");
		Add_Func(LUA, RecipientFilter_RemovePAS, "RemovePAS");
		Add_Func(LUA, RecipientFilter_RemovePVS, "RemovePVS");
		//Add_Func(LUA, RecipientFilter_RemovePlayer, "RemovePlayer");
		//Add_Func(LUA, RecipientFilter_RemovePlayers, "RemovePlayers");
		Add_Func(LUA, RecipientFilter_RemoveRecipientsByTeam, "RemoveRecipientsByTeam");
		Add_Func(LUA, RecipientFilter_RemoveRecipientsNotOnTeam, "RemoveRecipientsNotOnTeam");

	LUA->Pop(1);
}