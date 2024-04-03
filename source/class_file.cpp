#include "lua_threaded.h"

static int32_t metatype = GarrysMod::Lua::Type::File;
static const char metaname[] = "File";
static const char invalid_error[] = "invalid File";
static const char table_name[] = "File_object";

void Push_File(ILuaBase* LUA, const char* filename, const char* fileMode, const char* path)
{
	LUA_File* udata = (LUA_File*)LUA->NewUserdata(sizeof(LUA_File));
	udata->filename = filename;
	udata->handle = gpFileSystem->Open(filename, fileMode, path);

	ILuaInterface* ILUA = (ILuaInterface*)LUA;
	ILUA->SetType(metatype);

	LUA->CreateMetaTableType(metaname, metatype);
	LUA->SetMetaTable(-2);
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

void File_CheckType(ILuaBase* LUA, int index)
{
	if(!IsFile(LUA, index))
		luaL_typerror(LUA->GetState(), index, metaname);
}

LUA_File* File_GetUserdata(ILuaBase *LUA, int index)
{
	return (LUA_File*)LUA->GetUserdata(index);
}

LUA_File* File_Get(ILuaBase* LUA, int index, bool error)
{
	File_CheckType(LUA, index);

	LUA_File* file = File_GetUserdata(LUA, index);
	if(file == nullptr)
		LUA->ArgError(index, invalid_error);

	if (!file->handle && error)
		LUA->ThrowError("Tried to use a NULL File!");

	return file;
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
	LUA_File* file = File_Get(LUA, 1, false);
	char szBuf[270] = {};
	V_snprintf(szBuf, sizeof(szBuf),"File [%s]", file->handle ? file->filename : "NULL");
	LUA->PushString(szBuf);
	return 1;
}

LUA_FUNCTION_STATIC(File__index)
{
	File_CheckType(LUA, 1);
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

LUA_FUNCTION(File_Close)
{
	LUA_File* file = File_Get(LUA, 1);

	gpFileSystem->Close(file->handle);
	
	return 0;
}

LUA_FUNCTION(File_EndOfFile)
{
	LUA_File* file = File_Get(LUA, 1);

	LUA->PushBool(gpFileSystem->Tell(file->handle) == gpFileSystem->Size(file->handle));

	return 1;
}

LUA_FUNCTION(File_Flush)
{
	LUA_File* file = File_Get(LUA, 1);

	gpFileSystem->Flush(file->handle);

	return 0;
}

LUA_FUNCTION(File_Read)
{
	LUA_File* file = File_Get(LUA, 1);
	int length = LUA->GetNumber(2);

	if (length == 0)
		length = gpFileSystem->Size(file->handle);

	if ((gpFileSystem->Tell(file->handle) + length) > gpFileSystem->Size(file->handle))
		length = (gpFileSystem->Tell(file->handle) + length) - gpFileSystem->Size(file->handle); // Limit what we can read to not read memory?

	char* buffer = new char[length + 1];
	gpFileSystem->Read(buffer, length, file->handle);
	buffer[length] = 0;
	LUA->PushString(buffer);
	delete[] buffer;

	return 1;
}

LUA_FUNCTION(File_ReadBool)
{
	LUA_File* file = File_Get(LUA, 1);

	bool buffer = false;
	gpFileSystem->Read(&buffer, sizeof(bool), file->handle);
	LUA->PushBool(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadByte)
{
	LUA_File* file = File_Get(LUA, 1);

	uint8 buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(uint8), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadDouble)
{
	LUA_File* file = File_Get(LUA, 1);

	double buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(double), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadFloat)
{
	LUA_File* file = File_Get(LUA, 1);

	float buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(float), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadLine)
{
	LUA_File* file = File_Get(LUA, 1);

	int max_size = gpFileSystem->Size(file->handle) - gpFileSystem->Tell(file->handle);
	char* buffer = new char[max_size + 1];
	gpFileSystem->ReadLine(buffer, max_size, file->handle);
	buffer[max_size] = 0;
	LUA->PushString(buffer);
	delete[] buffer;

	return 1;
}

LUA_FUNCTION(File_ReadLong)
{
	LUA_File* file = File_Get(LUA, 1);

	long buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(long), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadShort)
{
	LUA_File* file = File_Get(LUA, 1);

	short buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(short), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadUInt64)
{
	LUA_File* file = File_Get(LUA, 1);

	uint64 buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(uint64), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadULong)
{
	LUA_File* file = File_Get(LUA, 1);

	unsigned long buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(unsigned long), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_ReadUShort)
{
	LUA_File* file = File_Get(LUA, 1);

	unsigned short buffer = 0;
	gpFileSystem->Read(&buffer, sizeof(unsigned short), file->handle);
	LUA->PushNumber(buffer);

	return 1;
}

LUA_FUNCTION(File_Seek)
{
	LUA_File* file = File_Get(LUA, 1);
	double pos = LUA->CheckNumber(2);

	gpFileSystem->Seek(file->handle, pos, FILESYSTEM_SEEK_HEAD);

	return 0;
}

LUA_FUNCTION(File_Size)
{
	LUA_File* file = File_Get(LUA, 1);

	LUA->PushNumber(gpFileSystem->Size(file->handle));

	return 1;
}

LUA_FUNCTION(File_Skip)
{
	LUA_File* file = File_Get(LUA, 1);
	double pos = LUA->CheckNumber(2);

	gpFileSystem->Seek(file->handle, pos, FILESYSTEM_SEEK_CURRENT);

	return 0;
}

LUA_FUNCTION(File_Tell)
{
	LUA_File* file = File_Get(LUA, 1);

	LUA->PushNumber(gpFileSystem->Tell(file->handle));

	return 1;
}

LUA_FUNCTION(File_Write)
{
	LUA_File* file = File_Get(LUA, 1);
	const char* data = LUA->CheckString(2);

	gpFileSystem->Write(data, sizeof(data), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteBool)
{
	LUA_File* file = File_Get(LUA, 1);
	bool data = LUA->GetBool(2);

	gpFileSystem->Write(&data, sizeof(bool), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteByte)
{
	LUA_File* file = File_Get(LUA, 1);
	uint8 data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(uint8), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteDouble)
{
	LUA_File* file = File_Get(LUA, 1);
	double data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(double), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteFloat)
{
	LUA_File* file = File_Get(LUA, 1);
	float data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(float), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteLong)
{
	LUA_File* file = File_Get(LUA, 1);
	long data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(long), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteShort)
{
	LUA_File* file = File_Get(LUA, 1);
	short data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(short), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteUInt64)
{
	LUA_File* file = File_Get(LUA, 1);
	uint64 data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(uint64), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteULong)
{
	LUA_File* file = File_Get(LUA, 1);
	unsigned long data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(unsigned long), file->handle);

	return 0;
}

LUA_FUNCTION(File_WriteUShort)
{
	LUA_File* file = File_Get(LUA, 1);
	unsigned short data = LUA->CheckNumber(2);

	gpFileSystem->Write(&data, sizeof(unsigned short), file->handle);

	return 0;
}

void InitFileClass(ILuaInterface* LUA)
{
	LUA->CreateTable();
	LUA->SetField(GarrysMod::Lua::INDEX_REGISTRY, table_name);

	LUA->CreateMetaTableType(metaname, metatype);
		Add_Func(LUA, File__gc, "__gc");
		Add_Func(LUA, File__tostring, "__tostring");
		Add_Func(LUA, File__index, "__index");

		Add_Func(LUA, File_Close, "Close");
		Add_Func(LUA, File_EndOfFile, "EndOfFile");
		Add_Func(LUA, File_Flush, "Flush");
		Add_Func(LUA, File_Read, "Read");
		Add_Func(LUA, File_ReadBool, "ReadBool");
		Add_Func(LUA, File_ReadByte, "ReadByte");
		Add_Func(LUA, File_ReadDouble, "ReadDouble");
		Add_Func(LUA, File_ReadFloat, "ReadFloat");
		Add_Func(LUA, File_ReadLine, "ReadLine");
		Add_Func(LUA, File_ReadLong, "ReadLong");
		Add_Func(LUA, File_ReadShort, "ReadShort");
		Add_Func(LUA, File_ReadUInt64, "ReadUInt64");
		Add_Func(LUA, File_ReadULong, "ReadULong");
		Add_Func(LUA, File_ReadUShort, "ReadUShort");
		Add_Func(LUA, File_Seek, "Seek");
		Add_Func(LUA, File_Size, "Size");
		Add_Func(LUA, File_Skip, "Skip");
		Add_Func(LUA, File_Tell, "Tell");
		Add_Func(LUA, File_Write, "Write");
		Add_Func(LUA, File_WriteBool, "WriteBool");
		Add_Func(LUA, File_WriteByte, "WriteByte");
		Add_Func(LUA, File_WriteDouble, "WriteDouble");
		Add_Func(LUA, File_WriteFloat, "WriteFloat");
		Add_Func(LUA, File_WriteLong, "WriteLong");
		Add_Func(LUA, File_WriteShort, "WriteShort");
		Add_Func(LUA, File_WriteUInt64, "WriteUInt64");
		Add_Func(LUA, File_WriteULong, "WriteULong");
		Add_Func(LUA, File_WriteUShort, "WriteUShort");
	LUA->Pop(1);
}