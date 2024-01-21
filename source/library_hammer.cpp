#include "lua_threaded.h"

LUA_FUNCTION(hammer_SendCommand)
{
	const char* cmd = LUA->CheckString(1);

	EditorSendResult_t res = func_Editor_SendCommand(cmd, false);
	if (res == Editor_OK) {
		LUA->PushString("ok");
	} else if(res == Editor_NotRunning) {
		LUA->PushString("notrunning");
	} else if(res == Editor_BadCommand) {
		LUA->PushString("badcommand");
	} else {
		LUA->PushString("error");
	}

	return 1;
}

void InitHammer(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, hammer_SendCommand, "SendCommand");

		LUA->SetField(-2, "hammer");
	LUA->Pop();
}