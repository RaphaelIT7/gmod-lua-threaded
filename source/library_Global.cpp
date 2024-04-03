#include "lua_threaded.h"
#include <sstream>

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

LUA_FUNCTION(FindMetaTable)
{
	const char* meta = LUA->CheckString(1);

	int reference = -1;
	LUA->PushSpecial(SPECIAL_REG);
		LUA->GetField(-1, meta);

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

std::unordered_map<std::string, ILuaThread*> commands;
void LuaConCommand(const CCommand& cmd)
{
	// How tf do I now get the ILuaInterface that created the command. Create a list and add all commands from a state?

	auto it = commands.find(cmd.Arg(1));
	if (it != commands.end())
	{
		ILuaThread* thread = it->second;
		if (thread->threaded)
		{
			ILuaAction* action = new ILuaAction;
			action->type = LuaAction::ACT_RunCommand;
			action->cmd = cmd;
			action->ply = func_UTIL_GetCommandClient();

			thread->mutex.Lock();
			thread->actions.push_back(action);
			thread->mutex.Unlock();
		} else {
			RunCommand(thread->IFace, cmd, func_UTIL_GetCommandClient()); // UTIL_GetCommandClient returns void* because I don't want to define CBasePlayer.
		}
	} else {
		Msg("Failed to find State for command %s!\n", cmd.Arg(0));
	}
}

/*
	ToDo: Look someday again to these values. They seem to be raised in Gmod.
	I won't implement this for now.
*/
int LuaConCommandAutocomplete(const char *partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ])
{
	return 0;
}

ILuaConVars* luaconvars;
LUA_FUNCTION(AddConsoleCommand)
{
	const char* name = LUA->CheckString(1);
	const char* helpText = LUA->CheckString(2);
	int flags = 0;
	if (LUA->IsType(3, Type::Number)) {
		flags = LUA->CheckNumber(3);
	} else if (LUA->IsType(3, Type::Table)) {
		flags = 131072; // Seems to always be FCVAR_DONTRECORD
	} else {
		LUA->ArgError(3, ((std::string)"number expected, got " + LUA->GetTypeName(LUA->GetType(3))).c_str()); // ToDo: Make it better someday.
	}

	bool blocked = func_ConCommand_IsBlocked(name);
	if (!blocked) 
	{
		blocked = (V_stricmp(name, "lua_cookieclear") == 0);
	}

	if (blocked) 
	{
		//Verify: LUA->ThrowError("AddConsoleCommand: Command name is blocked! (%s)");
		Msg("AddConsoleCommand: Command name is blocked! (%s)", name);
	}

	/*
		Experimental.
		Getting ILuaConVars and calling CreateConCommand.

		lua_shared contains this line:
		EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CLuaConVars, ILuaConVars, "LUACONVARS001", g_CLuaConVars);
	*/
	if (!luaconvars) {
		SourceSDK::FactoryLoader luashared_loader("lua_shared");
		luaconvars = luashared_loader.GetInterface<ILuaConVars>("LUACONVARS001");

		if (!luaconvars) {
			Msg("Failed to get ILuaConVars!\n");
			ThreadSleep(100);
		}
	}

	luaconvars->CreateConCommand(name, helpText, flags, LuaConCommand, LuaConCommandAutocomplete); // LuaConCommandAutocomplete doesn't have the values gmod uses. Idk but this could cause problems.
	Msg("Added concommand %s\n", name);

	return 0;
}

LUA_FUNCTION(AddCSLuaFile) // Implemented so that Gmod won't complain. ToDo: How should I get the current file it's called from?!? IDK
{
	ConDMsg("Don't call AddCSLuaFile from another Thread!\n");

	return 0;
}

LUA_FUNCTION(isangle)
{
	LUA->PushBool(IsAngle(LUA, 1));

	return 1;
}

LUA_FUNCTION(isbool)
{
	LUA->PushBool(LUA->IsType(1, Type::Bool));

	return 1;
}

LUA_FUNCTION(isfunction)
{
	LUA->PushBool(LUA->IsType(1, Type::Function));

	return 1;
}

LUA_FUNCTION(isentity)
{
	LUA->PushBool(LUA->IsType(1, Type::Entity));

	return 1;
}

LUA_FUNCTION(ismatrix)
{
	LUA->PushBool(LUA->IsType(1, Type::Matrix));

	return 1;
}

LUA_FUNCTION(isnumber)
{
	LUA->PushBool(LUA->IsType(1, Type::Number));

	return 1;
}

LUA_FUNCTION(ispanel)
{
	LUA->PushBool(LUA->IsType(1, Type::Panel));

	return 1;
}

LUA_FUNCTION(isstring)
{
	LUA->PushBool(LUA->IsType(1, Type::String));

	return 1;
}

LUA_FUNCTION(istable)
{
	LUA->PushBool(LUA->IsType(1, Type::Table));

	return 1;
}

LUA_FUNCTION(isvector)
{
	LUA->PushBool(IsVector(LUA, 1));

	return 1;
}

LUA_FUNCTION(Global_Msg)
{
	std::stringstream ss;
	for (int i=1; i <= LUA->Top(); ++i)
	{
		const char* arg_str = func_lua_tostring(LUA->GetState(), i, NULL);
		if (arg_str)
		{
			ss << arg_str;
		} else {
		ss << "<Something Unknown. Scary>";
		}
	}
	ss << "\n";
	Msg(ss.str().c_str());

	return 0;
}

void InitGlobal(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		Add_Func(LUA, include, "include");
		Add_Func(LUA, require, "require");
		Add_Func(LUA, FindMetaTable, "FindMetaTable");
		Add_Func(LUA, AddConsoleCommand, "AddConsoleCommand");
		Add_Func(LUA, AddCSLuaFile, "AddCSLuaFile");
		Add_Func(LUA, Global_Msg, "Msg");

		Add_Func(LUA, isangle, "isangle");
		Add_Func(LUA, isbool, "isbool");
		Add_Func(LUA, isfunction, "isfunction");
		Add_Func(LUA, isentity, "isentity");
		Add_Func(LUA, ismatrix, "ismatrix");
		Add_Func(LUA, isnumber, "isnumber");
		Add_Func(LUA, ispanel, "ispanel");
		Add_Func(LUA, isstring, "isstring");
		Add_Func(LUA, istable, "istable");
		Add_Func(LUA, isvector, "isvector");

		Add_Func(LUA, Global_Vector, "Vector");
		Add_Func(LUA, Global_LerpVector, "LerpVector");
		Add_Func(LUA, Global_OrderVectors, "OrderVectors");

		Add_Func(LUA, Global_Angle, "Angle");
		Add_Func(LUA, Global_LerpAngle, "LerpAngle");
	LUA->Pop(1);
}