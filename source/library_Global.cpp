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

LUA_FUNCTION(LVector)
{
	int x = LUA->CheckNumber(1);
	int y = LUA->CheckNumber(2);
	int z = LUA->CheckNumber(3);

	LUA->PushVector(Vector(x, y, z));
}

LUA_FUNCTION(Angle)
{
	int x = LUA->CheckNumber(1);
	int y = LUA->CheckNumber(2);
	int z = LUA->CheckNumber(3);

	LUA->PushAngle(QAngle(x, y, z));
}

void InitGlobal(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		Add_Func(LUA, include, "include");
		Add_Func(LUA, LVector, "Vector");
		Add_Func(LUA, Angle, "Angle");

	LUA->Pop(1);
}