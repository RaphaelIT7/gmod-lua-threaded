#include "lua_threaded.h"

SourceSDK::FactoryLoader engine_loader("engine");
IGameEventManager2* eventmanager = nullptr;

ILuaValue* GetBool(IGameEvent* event, const char* pKey)
{
	ILuaValue* val = new ILuaValue;
	val->type = Type::Bool;
	val->number = event->GetBool(pKey);

	return val;
}

ILuaValue* GetInt(IGameEvent* event, const char* pKey)
{
	ILuaValue* val = new ILuaValue;
	val->type = Type::Number;
	val->number = event->GetInt(pKey);

	return val;
}

ILuaValue* GetString(IGameEvent* event, const char* pKey)
{
	ILuaValue* val = new ILuaValue;
	val->type = Type::String;
	val->string = event->GetString(pKey);

	return val;
}

class GameEventListener : public IGameEventListener2
{
public:
	GameEventListener() = default;

	void FireGameEvent(IGameEvent* event)
	{
		ILuaAction* act = new ILuaAction;
		act->type = LuaAction::ACT_Gameevent;
		act->data = event->GetName();

		ILuaValue* val = new ILuaValue;
		std::unordered_map<std::string, ILuaValue*> tbl;

		if (strcmp(act->data, "OnRequestFullUpdate") == 0)
		{
			tbl["networkid"] = GetString(event, "networkid");
			tbl["name"] = GetString(event, "name");
			tbl["userid"] = GetInt(event, "userid");
			tbl["index"] = GetInt(event, "index");
		}
		else if (strcmp(act->data, "achievement_earned") == 0)
		{
			tbl["achievement"] = GetInt(event, "achievement");
			tbl["player"] = GetInt(event, "player");
		}
		else if (strcmp(act->data, "achievement_event") == 0)
		{
			tbl["achievement_name"] = GetString(event, "achievement_name");
			tbl["cur_val"] = GetInt(event, "cur_val");
			tbl["max_val"] = GetInt(event, "max_val");
		}
		else if (strcmp(act->data, "break_breakable") == 0)
		{
			tbl["entindex"] = GetString(event, "entindex");
			tbl["userid"] = GetInt(event, "userid");
			tbl["material"] = GetInt(event, "material");
		}
		else if (strcmp(act->data, "break_prop") == 0)
		{
			tbl["entindex"] = GetString(event, "entindex");
			tbl["userid"] = GetInt(event, "userid");
		}
		else if (strcmp(act->data, "client_disconnect") == 0)
		{
			tbl["message"] = GetString(event, "message");
		}
		else if (strcmp(act->data, "entity_killed") == 0)
		{
			tbl["entindex_inflictor"] = GetInt(event, "entindex_inflictor");
			tbl["entindex_attacker"] = GetInt(event, "entindex_attacker");
			tbl["damagebits"] = GetInt(event, "damagebits");
			tbl["entindex_killed"] = GetInt(event, "entindex_killed");
		}
		else if (strcmp(act->data, "flare_ignite_npc") == 0)
		{
			tbl["entindex"] = GetString(event, "entindex");
		}
		else if (strcmp(act->data, "host_quit") == 0)
		{
		}
		else if (strcmp(act->data, "player_activate") == 0)
		{
			tbl["userid"] = GetInt(event, "userid");
		}
		else if (strcmp(act->data, "player_changename") == 0)
		{
			tbl["userid"] = GetInt(event, "userid");
			tbl["oldname"] = GetString(event, "oldname");
			tbl["newname"] = GetString(event, "newname");
		}
		else if (strcmp(act->data, "player_connect") == 0)
		{
			tbl["bot"] = GetInt(event, "bot");
			tbl["networkid"] = GetString(event, "networkid");
			tbl["name"] = GetString(event, "name");
			tbl["userid"] = GetInt(event, "userid");
			tbl["index"] = GetInt(event, "index");
			tbl["address"] = GetString(event, "address");
		}
		else if (strcmp(act->data, "player_connect_client") == 0)
		{
			tbl["bot"] = GetInt(event, "bot");
			tbl["networkid"] = GetString(event, "networkid");
			tbl["name"] = GetString(event, "name");
			tbl["userid"] = GetInt(event, "userid");
			tbl["index"] = GetInt(event, "index");
		}
		else if (strcmp(act->data, "player_disconnect") == 0)
		{
			tbl["bot"] = GetInt(event, "bot");
			tbl["networkid"] = GetString(event, "networkid");
			tbl["name"] = GetString(event, "name");
			tbl["userid"] = GetInt(event, "userid");
			tbl["reason"] = GetString(event, "reason");
		}
		else if (strcmp(act->data, "player_hurt") == 0)
		{
			tbl["health"] = GetInt(event, "health");
			tbl["userid"] = GetInt(event, "userid");
			tbl["attacker"] = GetInt(event, "attacker");
		}
		else if (strcmp(act->data, "player_info") == 0)
		{
			tbl["friendsid"] = GetInt(event, "friendsid");
			tbl["index"] = GetInt(event, "index");
			tbl["bot"] = GetInt(event, "bot");
			tbl["userid"] = GetInt(event, "userid");
			tbl["name"] = GetString(event, "name");
			tbl["networkid"] = GetString(event, "networkid");
		}
		else if (strcmp(act->data, "player_say") == 0)
		{
			tbl["userid"] = GetInt(event, "userid");
			tbl["text"] = GetString(event, "text");
		}
		else if (strcmp(act->data, "player_spawn") == 0)
		{
			tbl["userid"] = GetInt(event, "userid");
		}
		else if (strcmp(act->data, "ragdoll_dissolved") == 0)
		{
			tbl["entindex"] = GetInt(event, "entindex");
		}
		else if (strcmp(act->data, "server_addban") == 0)
		{
			tbl["networkid"] = GetString(event, "networkid");
			tbl["name"] = GetString(event, "name");
			tbl["userid"] = GetInt(event, "userid");
			tbl["ip"] = GetString(event, "ip");
			tbl["duration"] = GetString(event, "duration");
			tbl["by"] = GetString(event, "by");
			tbl["kicked"] = GetBool(event, "kicked");
		}
		else if (strcmp(act->data, "server_cvar") == 0)
		{
			tbl["cvarname"] = GetString(event, "cvarname");
			tbl["cvarvalue"] = GetString(event, "cvarvalue");
		}
		else if (strcmp(act->data, "server_removeban") == 0)
		{
			tbl["networkid"] = GetString(event, "networkid");
			tbl["ip"] = GetString(event, "ip");
			tbl["by"] = GetString(event, "by");
		}
		val->tbl = tbl;
		
		act->val = val;
	}
};

LUA_FUNCTION(gameevent_Listen) // ToDo: Look into CLuaLibrary and CLuaLibraryFunction and see how Gmod does it.
{
	const char* name = LUA->CheckString(1);
	ILuaThread* thread = GetValidThread(LUA, 1);
	if (!eventmanager->FindListener(thread->listener, name)) {
		eventmanager->AddListener(thread->listener, name, false);
	}

	return 0;
}

void RunGameevent(ILuaInterface* LUA, const char* name, ILuaValue* tbl) // ToDo: Check if hook and hook.Run exist before trying to use it
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->GetField(-1, "hook");
			LUA->GetField(-1, "Run");
			LUA->PushString(name);
			PushValue(LUA, tbl);

			LUA->Call(2, 0);
	LUA->Pop(2);

	SafeDelete(tbl);
}

void InitGameevent(ILuaThread* thread)
{
	ILuaInterface* LUA = thread->IFace;

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, gameevent_Listen, "Listen");

			LUA->SetField(-2, "gameevent");
	LUA->Pop(2);

	thread->listener = new GameEventListener;

	if (eventmanager == nullptr) {
		eventmanager = (IGameEventManager2*)engine_loader.GetFactory()(INTERFACEVERSION_GAMEEVENTSMANAGER2, nullptr);
		if (eventmanager == nullptr)
			LUA->ThrowError("unable to initialize IGameEventManager2");
	}
}