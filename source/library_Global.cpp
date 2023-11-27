#include "lua_threaded.h"

LUA_FUNCTION(include)
{
	ILuaThread* thread = GetValidThread(LUA, NULL);

	RunFile(thread, LUA->CheckString(1));

	return 0;
}

LUA_FUNCTION(require)
{
	const char* file = LUA->CheckString(1);

	bool exists = gpFileSystem->FileExists((((std::string)"lua/bin/") + file).c_str(), "GAME");
	if (exists)
	{
		func_GMOD_LoadBinaryModule(LUA->GetState(), file); // Unfinished
	} else {
		ILuaThread* thread = GetValidThread(LUA, NULL);

		RunFile(thread, ("lua/includes/modules/" + ((std::string)LUA->CheckString(1)) + ".lua").c_str());
	}

	return 0;
}

LUA_FUNCTION(Angle)
{
	int x = LUA->CheckNumber(1);
	int y = LUA->CheckNumber(2);
	int z = LUA->CheckNumber(3);

	LUA->PushAngle(QAngle(x, y, z));

	return 1;
}

LUA_FUNCTION(FindMetaTable)
{
	const char* meta = LUA->CheckString(1);

	int reference = -1;
	LUA->PushSpecial(SPECIAL_REG);
		LUA->GetField(-1, meta);

		Msg("Metatable has type %i\n", LUA->GetType(-1));
		if (LUA->IsType(-1, Type::Table))
		{
			reference = LUA->ReferenceCreate();
		}
	LUA->Pop(2);

	if (reference != -1) {
		LUA->ReferencePush(reference);
		LUA->ReferenceFree(reference);
	} else {
		LUA->PushNil();
	}

	return 1;
}

void InitGlobal(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		Add_Func(LUA, include, "include");
		Add_Func(LUA, require, "require");
		Add_Func(LUA, FindMetaTable, "FindMetaTable");


		Add_Func(LUA, _Vector, "Vector");
		Add_Func(LUA, Angle, "Angle");
	LUA->Pop(1);
}