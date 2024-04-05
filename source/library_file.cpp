#include "lua_threaded.h"
#include <algorithm>

std::unordered_map<const FileAsyncRequest_t*, IAsyncFile*> async_list;
void AsyncCallback(const FileAsyncRequest_t &request, int nBytesRead, FSAsyncStatus_t err)
{
	IAsyncFile* async = (IAsyncFile*)request.pContext;
	if (async)
	{
		async->finished = true;
		async->nBytesRead = nBytesRead;
		async->content = new char[nBytesRead];
		std::memcpy((void*)async->content, request.pData, nBytesRead);
	} else {
		Msg("[Luathreaded] file.AsyncRead Invalid request?\n");
	}
}

LUA_FUNCTION(file_AsyncRead)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* fileName = LUA->CheckString(1);
	const char* gamePath = LUA->CheckString(2);
	LUA->CheckType(3, GarrysMod::Lua::Type::Function);
	LUA->Push(3);
	int reference = LUA->ReferenceCreate();
	LUA->Pop();
	bool sync = LUA->GetBool(4);

	FileAsyncRequest_t* request = new FileAsyncRequest_t;
	request->pszFilename = fileName;
	request->pszPathID = gamePath;
	request->pfnCallback = AsyncCallback;
	request->flags = sync ? FSASYNC_FLAGS_SYNC : 0;

	IAsyncFile* file = new IAsyncFile;
	file->thread = thread;
	file->callback = reference;
	file->req = request;

	request->pContext = file;
	thread->async.push_back(file);

	LUA->PushNumber(filesystem->AsyncReadMultiple(request, 1));

	return 1;
}

void FileLibThink(ILuaThread* thread)
{
	std::vector<IAsyncFile*> files;
	GarrysMod::Lua::ILuaInterface* LUA = thread->IFace;
	for(IAsyncFile* file : thread->async) {
		if (!file->finished) { continue; }
		LUA->ReferencePush(file->callback);
		LUA->PushString(file->req->pszFilename);
		LUA->PushString(file->req->pszPathID);
		LUA->PushNumber(file->Status);
		LUA->PushString(file->content);
		LUA->CallFunctionProtected(4, 0, true);
		LUA->ReferenceFree(file->callback);
		files.push_back(file);
	}

	for(IAsyncFile* file : files) {
		delete file->content;
		thread->async.erase(find(thread->async.begin(),thread->async.end(), file)); // This is probably shit.
	}
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

std::string extractDirectoryPath(const std::string& filepath) {
    size_t lastSlashPos = filepath.find_last_of('/');
    if (lastSlashPos != std::string::npos) {
        return filepath.substr(0, lastSlashPos + 1);
    } else {
        return "";
    }
}

std::vector<std::string> SortByDate(std::vector<std::string> files, const char* filepath, const char* path, bool ascending)
{
	std::string str_filepath = extractDirectoryPath((std::string)filepath);
	std::unordered_map<std::string, long> dates;
	for (std::string file : files) {
		dates[file] = filesystem->GetFileTime((str_filepath + file).c_str(), path);
	}

	std::sort(files.begin(), files.end(), [&dates](const std::string& a, const std::string& b) {
        return dates[a] < dates[b];
    });

	if (!ascending) {
        std::reverse(files.begin(), files.end());
    }

	return files;
}

LUA_FUNCTION(file_Find)
{
	std::vector<std::string> files;
	std::vector<std::string> folders;

	const char* filepath = LUA->CheckString(1);
	const char* path = LUA->CheckString(2);
	const char* sorting = LUA->CheckString(3);

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
		if (strcmp(sorting, "namedesc") == 0) { // sort the files descending by name.
			std::sort(files.begin(), files.end(), std::greater<std::string>());
			std::sort(folders.begin(), folders.end(), std::greater<std::string>());
		} else if (strcmp(sorting, "dateasc") == 0) { // sort the files ascending by date.
			SortByDate(files, filepath, path, true);
			SortByDate(folders, filepath, path, true);
		} else if (strcmp(sorting, "datedesc") == 0) { // sort the files descending by date.
			SortByDate(files, filepath, path, false);
			SortByDate(folders, filepath, path, false);
		} else { // Fallback to default: nameasc | sort the files ascending by name.
			std::sort(files.begin(), files.end());
			std::sort(folders.begin(), folders.end());
		}

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
	const char* filename = LUA->CheckString(1);
	const char* fileMode = LUA->CheckString(2);
	const char* path = LUA->GetString(3);

	if (path == NULL)
		path = "GAME";

	Push_File(LUA, filename, fileMode, path);

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
	const char* path = LUA->GetString(2);
	if (path == NULL)
		path = "GAME";

	LUA->PushNumber(filesystem->Size(LUA->CheckString(1), path));

	return 1;
}

LUA_FUNCTION(file_Time)
{
	const char* path = LUA->GetString(2);
	if (path == NULL)
		path = "GAME";

	LUA->PushNumber(filesystem->GetFileTime(LUA->CheckString(1), path));

	return 1;
}

void InitFileLib(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
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