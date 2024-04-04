#include "lua_threaded.h"
#include <ehandle.h>
#include <entitylist_base.h>
#include <baseentity.h>

LUA_FUNCTION(ents_Create)
{
	const char* classname = LUA->CheckString(1);
	
	
	//LUA->PushMetaTable(1);

	return  0; // 1;
}

LUA_FUNCTION(ents_FindEntityByName)
{
	// Use the FindEntityByName function
    CBaseEntity* entity = gEntList.FindEntityByName(NULL, classname);
    if (entity != NULL) {
        // Entity found, do something with it...
		printf("Entity found: %s\n", entity->GetClassname());
	} else {
		// Entity not found
    }

	return  0;
}

/* void FileLibThink(ILuaThread* thread)
{
	std::vector<IAsyncFile*> files;
	ILuaInterface* LUA = thread->IFace;
	for(IAsyncFile* file : thread->async) {
		if (!file->finished) { continue; }
		LUA->ReferencePush(file->callback);
		LUA->PushString(file->req->pszFilename);
		LUA->PushString(file->req->pszPathID);
		LUA->PushNumber(file->Status);
		LUA->PushString(static_cast<char*>(file->req->pData));
		LUA->PCall(4, 0, 0);
		LUA->ReferenceFree(file->callback);
		files.push_back(file);
	}

	for(IAsyncFile* file : files) {
		thread->async.erase(find(thread->async.begin(),thread->async.end(), file)); // This is probably shit.
	}
} */

void InitEntsLib(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, ents_Create, "Create");
			Add_Func(LUA, ents_FindEntityByName, "FindEntityByName");
			
		LUA->SetField(-2, "ents");
	LUA->Pop();
}