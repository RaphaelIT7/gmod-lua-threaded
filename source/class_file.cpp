#include "lua_threaded.h"

static int32_t metatype = GarrysMod::Lua::Type::File;
static const char metaname[] = "File";
static const char invalid_error[] = "invalid File";
static const char table_name[] = "File_object";

void Push_File(ILuaBase* LUA, const char* filename, const char* fileMode, const char* path)
{
	LUA_File* udata = (LUA_File*)LUA->NewUserdata(sizeof(LUA_File));
	udata->filename = filename;
	udata->filemode = fileMode;
	udata->path = path;

	ILuaInterface* ILUA = (ILuaInterface*)LUA;
	ILUA->SetType(metatype);

	LUA->CreateMetaTableType(metaname, metatype);
	LUA->SetMetaTable(-2);
}

void File_CheckType(ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, Type::UserData))
		luaL_typerror(LUA->GetState(), index, metaname);
}

bool IsFile(ILuaBase* LUA, int index)
{
	if (LUA->IsType(index, Type::UserData))
	{
		LUA->GetMetaTable(index);
		LUA->GetField(-1, "MetaName");
		if (LUA->IsType(-1, Type::String))
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

LUA_File* File_GetUserdata(ILuaBase *LUA, int index)
{
	return (LUA_File*)LUA->GetUserdata(index);
}

LUA_File* File_Get(ILuaBase* LUA, int index)
{
	File_CheckType(LUA, index);

	LUA_File* vec = File_GetUserdata(LUA, index);
	if(vec == nullptr)
		LUA->ArgError(index, invalid_error);

	return vec;
}

void File_Destroy(ILuaBase *LUA, int index)
{
	LUA_File* file = File_GetUserdata(LUA, index);

	LUA->GetField(INDEX_REGISTRY, table_name);
	LUA->PushUserdata(file);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(File__gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	File_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(File__tostring)
{
	LUA_Vector* vec = Vector_Get(LUA, 1);
	char szBuf[64] = {};
	V_snprintf(szBuf, sizeof(szBuf),"%f %f %f", vec->x, vec->y, vec->z);
	LUA->PushString(szBuf);
	return 1;
}

void InitFileClass(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	LUA->CreateMetaTableType(metaname, metatype);

	LUA->Pop(1);
}