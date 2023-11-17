#include "detours.h"
#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/Lua/LuaObject.h>
#include <unordered_map>
#include <lua.h>

int interfaces_count = 0;
std::unordered_map<double, ILuaInterface*> interfaces;

static int32_t metatype = GarrysMod::Lua::Type::NONE;
static const char metaname[] = "ILuaInterface";
static const char invalid_error[] = "invalid ILuaInterface";
static const char table_name[] = "ILuaInterface_object";

struct LUA_ILuaInterface
{
	ILuaInterface* IFace;
	int ID;
};

inline void CheckType(GarrysMod::Lua::ILuaBase* LUA, int32_t index)
{
	if(!LUA->IsType(index, metatype))
		luaL_typerror(LUA->GetState(), index, metaname);
}

inline LUA_ILuaInterface *GetUserdata(GarrysMod::Lua::ILuaBase *LUA, int index)
{
	return LUA->GetUserType<LUA_ILuaInterface>(index, metatype);
}

static ILuaInterface* Get(GarrysMod::Lua::ILuaBase* LUA, int32_t index)
{
	CheckType(LUA, index);
	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if(udata == nullptr)
		LUA->ArgError(index, invalid_error);

	return udata->IFace;
}

void Push(GarrysMod::Lua::ILuaBase* LUA, ILuaInterface* Interface, int ID)
{
	if(Interface == nullptr)
	{
		LUA->PushNil();
		return;
	}

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
	LUA->PushUserdata(Interface);
	LUA->GetTable(-2);
	if(LUA->IsType(-1, metatype))
	{
		LUA->Remove(-2);
		return;
	}

	LUA->Pop(1);

	LUA_ILuaInterface *udata = LUA->NewUserType<LUA_ILuaInterface>(metatype);
	udata->IFace = Interface;
	udata->ID = ID;

	LUA->PushMetaTable(metatype);
	LUA->SetMetaTable(-2);

	LUA->CreateTable();
	LUA->SetFEnv(-2);

	LUA->PushUserdata(Interface);
	LUA->Push(-2);
	LUA->SetTable(-4);
	LUA->Remove(-2);
}

static void Destroy(GarrysMod::Lua::ILuaBase *LUA, int32_t index)
{
	LUA_ILuaInterface *udata = GetUserdata(LUA, index);
	if (udata == nullptr)
		return;

	ILuaInterface* IFace = udata->IFace;

	LUA->GetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);
	LUA->PushUserdata(IFace);
	LUA->PushNil();
	LUA->SetTable(-3);
	LUA->Pop(1);
	
	LUA->SetUserType(index, nullptr);
}

LUA_FUNCTION_STATIC(gc)
{
	if (!LUA->IsType(1, metatype))
		return 0;

	Destroy(LUA, 1);
	return 0;
}

LUA_FUNCTION_STATIC(tostring)
{
	LUA->PushFormattedString("%s: %p", metaname, Get(LUA, 1));
	return 1;
}

LUA_FUNCTION_STATIC(eq)
{
	LUA->PushBool(Get(LUA, 1) == Get(LUA, 2));
	return 1;
}

LUA_FUNCTION_STATIC(index)
{
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

LUA_FUNCTION_STATIC(newindex)
{
	LUA->GetFEnv(1);
	LUA->Push(2);
	LUA->Push(3);
	LUA->RawSet(-3);
	return 0;
}

/*
	ILuaInterface functions
*/
LUA_FUNCTION(ILuaInterface_RunString)
{
	ILuaInterface* IFace = Get(LUA, 1);

	IFace->RunString("RunString", "RunString", LUA->CheckString(2), true, true); // ToDo: Change this. The Interface will be on another Thread so fix this!

	return 0;
}

/*
	Module Table
*/
LUA_FUNCTION(LuaThread_GetInterface)
{


	return 0;
}

LUA_FUNCTION(LuaThread_CreateInterface)
{
	ILuaInterface* IFace = func_CreateLuaInterface(true);

	interfaces_count += 1;
	interfaces[interfaces_count] = IFace;

	Push(LUA, IFace, interfaces_count);

	if (IFace->GetState() == nullptr) {
		Msg("ILuaInterface got no State!\n");
	}

	return 1;
}

LUA_FUNCTION(LuaThread_CloseInterface)
{
	ILuaInterface* ILUA = (ILuaInterface*)LUA;
	ILuaObject* obj = ILUA->GetObject(0);

	double id = obj->GetMemberDouble("ID", -1);
	if (id == -1) 
	{
		LUA->ThrowError("Failed to get a Valid ID!");
	}

	ILuaInterface* IFace = interfaces[id];
	func_CloseLuaInterface(IFace);

	interfaces[id] = nullptr;

	return 0;
}

void Add_Func(GarrysMod::Lua::ILuaBase* LUA, CFunc Func, const char* Name) {
	LUA->PushCFunction(Func);
	LUA->SetField(-2, Name);
}

GMOD_MODULE_OPEN()
{
	Symbols_Init();

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, LuaThread_GetInterface, "GetInterface");
			Add_Func(LUA, LuaThread_CreateInterface, "Createinterface");
			Add_Func(LUA, LuaThread_CloseInterface, "CloseInterface");

			LUA->SetField(-2, "LuaThreaded");
	LUA->Pop(2);

	LUA->CreateTable();
	LUA->SetField( GarrysMod::Lua::INDEX_REGISTRY, table_name );

	metatype = LUA->CreateMetaTable(metaname);

	LUA->PushCFunction(gc);
	LUA->SetField(-2, "__gc");

	LUA->PushCFunction(tostring);
	LUA->SetField(-2, "__tostring");

	LUA->PushCFunction(eq);
	LUA->SetField(-2, "__eq");

	LUA->PushCFunction(index);
	LUA->SetField(-2, "__index");

	LUA->PushCFunction(newindex);
	LUA->SetField(-2, "__newindex");

	LUA->Pop(1);

	return 0;
}

GMOD_MODULE_CLOSE()
{
	LUA->PushNil();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, metaname);

	return 0;
}