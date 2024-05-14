#include "networkstringtabledefs.h"
#include "lua_threaded.h"

typedef void (*AddResource)(const char*, bool);
AddResource func_AddResource;
const Symbol AddResourceSym = Symbol::FromName("_ZL11AddResourcePKcb");
static SourceSDK::ModuleLoader server_loader("server");

#define ressource_workaround

LUA_FUNCTION(resource_AddFile) // ToDo
{
	const char* file = LUA->CheckString(1);

	char cFile[255];
	V_strncpy(cFile, file, sizeof(file));
	V_FixSlashes(cFile, '/');

#ifdef ressource_workaround
	INetworkStringTable* g_StringTablesDownloadables = networkstringtables->FindTable("downloadables");

	int idx = g_StringTablesDownloadables->FindStringIndex(cFile);

	if (idx != INVALID_STRING_INDEX)
		return 0;

	g_StringTablesDownloadables->AddString(true, cFile); // ToDo: Add files like .mdl .dx90, dx80 etc
#else
	func_AddResource(cFile, true);
#endif

	return 0;
}

LUA_FUNCTION(resource_AddSingleFile)
{
	const char* file = LUA->CheckString(1);

	char cFile[255];
	V_strncpy(cFile, file, sizeof(file));
	V_FixSlashes(cFile, '/');

#ifdef ressource_workaround
	INetworkStringTable* g_StringTablesDownloadables = networkstringtables->FindTable("downloadables");

	int idx = g_StringTablesDownloadables->FindStringIndex(cFile);

	if (idx != INVALID_STRING_INDEX)
		return 0;

	g_StringTablesDownloadables->AddString(true, cFile);
#else
	func_AddResource(cFile, false);
#endif

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

void InitResource(GarrysMod::Lua::ILuaInterface* LUA)
{
	func_AddResource = (AddResource)GetFunction(server_loader, "AddResource", AddResourceSym);

	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, resource_AddFile, "AddFile"); // ToDo: Doesn't seem to work? Maybe add it manually
			Add_Func(LUA, resource_AddSingleFile, "AddSingleFile"); // ToDo: Same as above
			Add_Func(LUA, resource_AddWorkshop, "AddWorkshop");

		LUA->SetField(-2, "resource");
	LUA->Pop();
}