#include "lua_threaded.h"

static int32_t metatype = GarrysMod::Lua::Type::ConVar;
static const char metaname[] = "ConVar";
static const char invalid_error[] = "invalid ConVar";
static const char table_name[] = "ConVar_object";

void Push_ConVar(GarrysMod::Lua::ILuaBase* LUA, ConVar* convar)
{
	LUA_ConVar* udata = LUA->NewUserType<LUA_ConVar>(metatype);
	udata->cvar = convar;

	GarrysMod::Lua::ILuaInterface* ILUA = (GarrysMod::Lua::ILuaInterface*)LUA;
	ILUA->SetType(metatype);

	LUA->CreateMetaTableType(metaname, metatype);
	LUA->SetMetaTable(-2);
}

bool IsConVar(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	if (LUA->IsType(index, GarrysMod::Lua::Type::UserData))
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

void ConVar_CheckType(GarrysMod::Lua::ILuaBase* LUA, int index)
{
	if(!LUA->IsType(index, GarrysMod::Lua::Type::UserData)) // ToDo: Make a better check.
		LUA->TypeError(index, metaname);
}

LUA_ConVar* ConVar_GetUserdata(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	return (LUA_ConVar*)LUA->GetUserdata(index);
}

LUA_ConVar* ConVar_Get(GarrysMod::Lua::ILuaBase* LUA, int index, bool error)
{
	ConVar_CheckType(LUA, index);

	LUA_ConVar* cvar = ConVar_GetUserdata(LUA, index);
	if(cvar == nullptr)
		LUA->ArgError(index, invalid_error);

	if (!cvar->cvar && error)
		LUA->ThrowError("Tried to use a NULL ConVar!");

	return cvar;
}

void ConVar_Destroy(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	LUA_ConVar* cvar = ConVar_GetUserdata(LUA, index);

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
	LUA->PushUserdata(cvar);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(ConVar__gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	ConVar_Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(ConVar__tostring)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, false);
	char szBuf[270] = {};
	V_snprintf(szBuf, sizeof(szBuf),"ConVar [%s]", cvar->cvar ? cvar->cvar->GetName() : "NULL");
	LUA->PushString(szBuf);
	return 1;
}

LUA_FUNCTION_STATIC(ConVar__index)
{
	ConVar_CheckType(LUA, 1);
	const char* pKey = LUA->CheckString(2);

	int ref = -1;
	LUA->GetMetaTable(1);
		LUA->GetField(-1, pKey);
			ref = LUA->ReferenceCreate();

	LUA->Pop(2);

	LUA->ReferencePush(ref);
	LUA->ReferenceFree(ref);

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetBool)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushBool(cvar->cvar->GetBool());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetDefault)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushString(cvar->cvar->GetDefault());

	return 1;
}

class ConCommandBasee // How else should I get access to m_nFlags?
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	friend void ConVar_Register( int nCVarFlag, IConCommandBaseAccessor *pAccessor );
	friend void ConVar_PublishToVXConsole();
	friend class CDefaultCvar;
public:
								ConCommandBasee( void );
								ConCommandBasee( const char *pName, const char *pHelpString = 0, 
									int flags = 0 );
	virtual						~ConCommandBasee( void );
	virtual	bool				IsCommand( void ) const;
	virtual bool				IsFlagSet( int flag ) const;
	virtual void				AddFlags( int flags );
	virtual const char			*GetName( void ) const;
	virtual const char			*GetHelpText( void ) const;
	const ConCommandBase		*GetNext( void ) const;
	ConCommandBase				*GetNext( void );
	virtual bool				IsRegistered( void ) const;
	virtual CVarDLLIdentifier_t	GetDLLIdentifier() const;

protected:
	virtual void				CreateBase( const char *pName, const char *pHelpString = 0, 
									int flags = 0 );
	virtual void				Init();
	void						Shutdown();
	char						*CopyString( const char *from );
public:
	ConCommandBase				*m_pNext;
	bool						m_bRegistered;
	const char 					*m_pszName;
	const char 					*m_pszHelpString;
	int							m_nFlags;
protected:
	static ConCommandBase		*s_pConCommandBases;
	static IConCommandBaseAccessor	*s_pAccessor;
};

LUA_FUNCTION_STATIC(ConVar_GetFlags)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	ConCommandBasee* base = (ConCommandBasee*)cvar->cvar;
	LUA->PushNumber(base->m_nFlags); // ToDo

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetFloat)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushNumber(cvar->cvar->GetFloat());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetHelpText)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushString(cvar->cvar->GetHelpText());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetInt)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushNumber(cvar->cvar->GetInt());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetMax)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	float max;
	cvar->cvar->GetMax(max);

	LUA->PushNumber(max);

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetMin)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	float min;
	cvar->cvar->GetMin(min);

	LUA->PushNumber(min);

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetName)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushString(cvar->cvar->GetName());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_GetString)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	LUA->PushString(cvar->cvar->GetString());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_IsFlagSet)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);
	int flag = LUA->CheckNumber(2);

	LUA->PushBool(cvar->cvar->IsFlagSet(flag));

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_Revert)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);

	cvar->cvar->Revert();

	return 0;
}

LUA_FUNCTION_STATIC(ConVar_SetBool)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);
	bool value = LUA->GetBool(2);

	cvar->cvar->SetValue(value ? "1" : "0");

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_SetFloat)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);
	float value = LUA->CheckNumber(2);

	cvar->cvar->SetValue(std::to_string(value).c_str());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_SetInt)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);
	int value = LUA->CheckNumber(2);

	cvar->cvar->SetValue(std::to_string(value).c_str());

	return 1;
}

LUA_FUNCTION_STATIC(ConVar_SetString)
{
	LUA_ConVar* cvar = ConVar_Get(LUA, 1, true);
	const char* value = LUA->CheckString(2);

	cvar->cvar->SetValue(value);

	return 1;
}

void InitConVarClass(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	LUA->CreateMetaTableType(metaname, metatype);
		Add_Func(LUA, ConVar__gc, "__gc");
		Add_Func(LUA, ConVar__tostring, "__tostring");
		Add_Func(LUA, ConVar__index, "__index");

		Add_Func(LUA, ConVar_GetBool, "GetBool");
		Add_Func(LUA, ConVar_GetDefault, "GetDefault");
		Add_Func(LUA, ConVar_GetFlags, "GetFlags");
		Add_Func(LUA, ConVar_GetHelpText, "GetHelpText");
		Add_Func(LUA, ConVar_GetInt, "GetInt");
		Add_Func(LUA, ConVar_GetMax, "GetMax");
		Add_Func(LUA, ConVar_GetMin, "GetMin");
		Add_Func(LUA, ConVar_GetName, "GetName");
		Add_Func(LUA, ConVar_GetString, "GetString");
		Add_Func(LUA, ConVar_IsFlagSet, "IsFlagSet");
		Add_Func(LUA, ConVar_Revert, "Revert");
		Add_Func(LUA, ConVar_SetBool, "SetBool");
		Add_Func(LUA, ConVar_SetFloat, "SetFloat");
		Add_Func(LUA, ConVar_SetInt, "SetInt");
		Add_Func(LUA, ConVar_SetString, "SetString");
	LUA->Pop(1);
}