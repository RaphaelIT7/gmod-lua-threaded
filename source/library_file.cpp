#include <GarrysMod/InterfacePointers.hpp>
#include "lua_threaded.h"

IFileSystem* filesystem;
LUA_FUNCTION(file_AsyncRead) // ToDo
{
	
}

LUA_FUNCTION(file_CreateDir)
{
	filesystem->CreateDirHierarchy(LUA->CheckString(1), "DATA");

	return 0;
}

LUA_FUNCTION(file_Delete)
{
	filesystem->RemoveFile(LUA->CheckString(1), "DATA");

	return 0;
}

LUA_FUNCTION(file_Exists)
{
	LUA->PushBool(filesystem->FileExists(LUA->CheckString(1), LUA->CheckString(2)));

	return 1;
}

LUA_FUNCTION(file_Find)
{
	std::vector<std::string> files;
	std::vector<std::string> folders;

	const char* filepath = LUA->CheckString(1);
	const char* path = LUA->CheckString(2);
	const char* sorting = LUA->CheckString(3); // ToDo: Implement it later

	FileFindHandle_t findHandle;
	const char *pFilename = filesystem->FindFirstEx(filepath, path, &findHandle);
	while (pFilename)
	{
		if (filesystem->IsDirectory(((std::string)filepath + pFilename).c_str(), path)) {
			folders.push_back(pFilename);
		} else {
			files.push_back(pFilename);
		}

		pFilename = filesystem->FindNext(findHandle);
	}
	filesystem->FindClose(findHandle);

	if (files.size() > 0) {
		LUA->CreateTable();

		int i = 0;
		for (std::string file : files)
		{
			++i;
			LUA->PushString(file.c_str());
			LUA->SetField(-2, std::to_string(i).c_str());
		}
	} else {
		LUA->PushNil();
	}

	if (folders.size() > 0) {
		LUA->CreateTable();

		int i = 0;
		for (std::string folder : folders)
		{
			++i;
			LUA->PushString(folder.c_str());
			LUA->SetField(-2, std::to_string(i).c_str());
		}
	} else {
		LUA->PushNil();
	}

	return 2;
}

LUA_FUNCTION(file_IsDir)
{
	LUA->PushBool(filesystem->IsDirectory(LUA->CheckString(1), LUA->CheckString(2)));

	return 1;
}

LUA_FUNCTION(file_Open) // ToDo: Add the File class
{
	LUA->PushBool(true);

	return 1;
}

LUA_FUNCTION(file_Rename)
{
	const char* original = LUA->CheckString(1);
	const char* newname = LUA->CheckString(2);

	LUA->PushBool(filesystem->RenameFile(original, newname, "DATA"));

	return 1;
}

LUA_FUNCTION(file_Size)
{
	LUA->PushNumber(filesystem->Size(LUA->CheckString(1), "DATA"));

	return 1;
}

LUA_FUNCTION(file_Time)
{
	LUA->PushNumber(filesystem->GetFileTime(LUA->CheckString(1), "DATA"));

	return 1;
}

void InitFileLib(ILuaInterface* LUA)
{
	if (!filesystem)
	{
		filesystem = InterfacePointers::FileSystem();
	}

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, file_AsyncRead, "AsyncRead");
			Add_Func(LUA, file_CreateDir, "CreateDir");
			Add_Func(LUA, file_Delete, "Delete");
			Add_Func(LUA, file_Exists, "Exists");
			Add_Func(LUA, file_Find, "Find");
			Add_Func(LUA, file_IsDir, "IsDir");
			Add_Func(LUA, file_Open, "Open");
			Add_Func(LUA, file_Rename, "Rename");
			Add_Func(LUA, file_Size, "Size");
			Add_Func(LUA, file_Time, "Time");
			
		LUA->SetField(-2, "file");
	LUA->Pop();
}