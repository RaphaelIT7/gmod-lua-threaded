#include "lua_threaded.h"
#ifdef ARCHITECTURE_X86
#include "KeyValues.h"
#else
#include "keyvalues.h"
#endif

IGameEventManager2* eventmanager = nullptr;

ILuaValue* GetFloat(IGameEvent* event, const char* pKey)
{
	ILuaValue* val = new ILuaValue;
	val->type = Type::Number;
	val->number = event->GetFloat(pKey);

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

class CGameEventDescriptor;
class CGameEvent : public IGameEvent
{
public:

	CGameEvent( CGameEventDescriptor *descriptor );
	virtual ~CGameEvent();

	const char *GetName() const;
	bool  IsEmpty(const char *keyName = NULL);
	bool  IsLocal() const;
	bool  IsReliable() const;

	bool  GetBool( const char *keyName = NULL, bool defaultValue = false );
	int   GetInt( const char *keyName = NULL, int defaultValue = 0 );
	float GetFloat( const char *keyName = NULL, float defaultValue = 0.0f );
	const char *GetString( const char *keyName = NULL, const char *defaultValue = "" );

	void SetBool( const char *keyName, bool value );
	void SetInt( const char *keyName, int value );
	void SetFloat( const char *keyName, float value );
	void SetString( const char *keyName, const char *value );
	
	CGameEventDescriptor	*m_pDescriptor;
	KeyValues				*m_pDataKeys;
};

class GameEventListener : public IGameEventListener2
{
public:
	GameEventListener(ILuaThread* tthread)
	{
		thread = tthread;	
	};

	void FireGameEvent(IGameEvent* ev)
	{
		CGameEvent* event = (CGameEvent*)ev;
		ILuaAction* act = new ILuaAction;
		act->type = LuaAction::ACT_Gameevent;
		act->data = event->GetName();

		ILuaValue* val = new ILuaValue;
		val->type = Type::Table;

		std::unordered_map<ILuaValue*, ILuaValue*> tbl;

		KeyValues* subkey = event->m_pDataKeys->GetFirstSubKey();
		while (subkey)
		{
			const char* pName = subkey->GetName();
			KeyValues::types_t pType = subkey->GetDataType();
			if (pType == KeyValues::TYPE_STRING)
			{
				tbl[CreateValue(pName)] = GetString(ev, pName);
			} else if (pType == KeyValues::TYPE_UINT64 || pType == KeyValues::TYPE_INT)
			{
				tbl[CreateValue(pName)] = GetInt(ev, pName);
			} else if (pType == KeyValues::TYPE_FLOAT)
			{
				tbl[CreateValue(pName)] = GetFloat(ev, pName);
			} else {
				//GlobalLUA->PushNil();
				Msg("Invalid Type?!? (%s -> %s)\n", event->GetName(), subkey->GetName());
			}

			subkey = subkey->GetNextKey();
		}
		val->tbl = tbl;
		act->val = val;

		thread->mutex.Lock();
		thread->actions.push_back(act);
		thread->mutex.Unlock();
		Msg("Event %s was called!\n", act->data);
	}

private:
	ILuaThread* thread;
};

LUA_FUNCTION(gameevent_Listen) // ToDo: Look into CLuaLibrary and CLuaLibraryFunction and see how Gmod does it.
{
	const char* name = LUA->CheckString(1);
	ILuaThread* thread = GetValidThread(LUA, 1);
	if (!eventmanager->FindListener(thread->listener, name)) {
		Msg("Added Listener for %s\n", name);
		eventmanager->AddListener(thread->listener, name, false);
	}

	return 0;
}

void InitGameevent(ILuaThread* thread)
{
	ILuaInterface* LUA = thread->IFace;

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, gameevent_Listen, "Listen");

		LUA->SetField(-2, "gameevent");
	LUA->Pop();

	thread->listener = new GameEventListener(thread);

	if (eventmanager == nullptr) {
		SourceSDK::FactoryLoader engine_loader("engine");
		eventmanager = (IGameEventManager2*)engine_loader.GetFactory()(INTERFACEVERSION_GAMEEVENTSMANAGER2, nullptr);
		if (eventmanager == nullptr)
			LUA->ThrowError("unable to initialize IGameEventManager2");
	}
}