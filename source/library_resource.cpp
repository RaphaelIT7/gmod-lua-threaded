#include "networkstringtabledefs.h"
#include "lua_threaded.h"

SourceSDK::ModuleLoader server_loader("server");

typedef void (*AddResource)(const char*, bool);
AddResource func_AddResource;
const Symbol AddResourceSym = Symbol::FromName("_ZL11AddResourcePKcb");

INetworkStringTableContainer* networkstringtables;

LUA_FUNCTION(resource_AddFile) // ToDo
{
	const char* file = LUA->CheckString(1);

	char cFile[255];
	V_strncpy(cFile, file, sizeof(file));
	V_FixSlashes(cFile, '/');

	func_AddResource(cFile, true);

	return 0;
}

LUA_FUNCTION(resource_AddSingleFile)
{
	const char* file = LUA->CheckString(1);

	char cFile[255];
	V_strncpy(cFile, file, sizeof(file));
	V_FixSlashes(cFile, '/');

	func_AddResource(cFile, false);

	return 0;
}

LUA_FUNCTION(resource_AddWorkshop)
{
	const char* workshopId = LUA->CheckString(1);

	INetworkStringTable* g_StringTablesDownloadables = networkstringtables->FindTable("downloadables");

	std::string entry_name = ((std::string)workshopId) + ".gma";
	int idx = g_StringTablesDownloadables->FindStringIndex(entry_name.c_str());

	if (idx != INVALID_STRING_INDEX)
		return 0;

	g_StringTablesDownloadables->AddString(true, entry_name.c_str());

	return 0;
}

void InitResource(ILuaInterface* LUA)
{
	if (networkstringtables == nullptr) {
		SourceSDK::FactoryLoader engine_loader("engine");
		networkstringtables = (INetworkStringTableContainer*)engine_loader.GetFactory()(INTERFACENAME_NETWORKSTRINGTABLESERVER, nullptr);
		if (networkstringtables == nullptr)
			LUA->ThrowError("unable to initialize INetworkStringTableContainer");
	}

	func_AddResource = (AddResource)GetFunction(server_loader, "AddResource", AddResourceSym);

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, resource_AddFile, "AddFile");
			Add_Func(LUA, resource_AddSingleFile, "AddSingleFile");
			Add_Func(LUA, resource_AddWorkshop, "AddWorkshop");

			LUA->SetField(-2, "resource");
	LUA->Pop(2);
}