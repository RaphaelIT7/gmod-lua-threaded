#include "CLuaObject.h"

void CLuaObject::Init()
{
	Error("What called this?");
}

void CLuaObject::Init(GarrysMod::Lua::ILuaBase* Lm_pLua)
{
	m_pLua = Lm_pLua;
}

void CLuaObject::UnReference()
{
	m_pLua->ReferenceFree(m_reference);
	m_reference = -1;
	// Should we also reset the Metatable?
	// m_pLua->ReferenceFree(m_metatable)
	// m_metatable = -1;
}

void CLuaObject::SetReference(int reference)
{
	m_reference = reference;
}

void CLuaObject::Set(ILuaObject* obj)
{
	UnReference();
	SetReference(((CLuaObject*)obj)->m_reference);
}

void CLuaObject::SetFromStack(int pos)
{
	m_pLua->Push(pos);

	if (m_pLua->GetType(-1) != GarrysMod::Lua::Type::Nil)
	{
		UnReference();
		m_reference = m_pLua->ReferenceCreate();
	}

	m_pLua->Pop(1);
}

void CLuaObject::Push()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_metatable != -1) {
			m_pLua->ReferencePush(m_metatable);
			m_pLua->SetMetaTable(-2);
		}
	} else {
		m_pLua->PushNil();
	}
}

int CLuaObject::GetType()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		int type = m_pLua->GetType(-1);
		m_pLua->Pop(1);
		return type;
	} else {
		return GarrysMod::Lua::Type::Nil;
	}
}

const char* CLuaObject::GetString()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		const char* str = m_pLua->GetString(-1);
		m_pLua->Pop(1);
		return str == NULL ? "" : str;
	} else {
		return "";
	}
}

float CLuaObject::GetFloat()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		float num = m_pLua->GetNumber(-1);
		m_pLua->Pop(1);
		return num;
	} else {
		return 0;
	}
}

int CLuaObject::GetInt()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		int num = m_pLua->GetNumber(-1);
		m_pLua->Pop(1);
		return num;
	} else {
		return 0;
	}
}

double CLuaObject::GetDouble()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		double num = m_pLua->GetNumber(-1);
		m_pLua->Pop(1);
		return num;
	} else {
		return 0;
	}
}

void* CLuaObject::GetUserData()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		void* data = m_pLua->GetUserdata(-1);
		m_pLua->Pop(1);
		return data;
	} else {
		return 0;
	}
}

void CLuaObject::SetMember(const char* name)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->Push(-2);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, ILuaObject *obj)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			obj->Push();
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, float val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, bool val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushBool(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, const char* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushString(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, GarrysMod::Lua::CFunc val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushCFunction(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

bool CLuaObject::GetMemberBool(const char* name, bool b)
{
	bool val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Bool)) {
				val = m_pLua->GetBool(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

int CLuaObject::GetMemberInt(const char* name, int b)
{
	int val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Number)) {
				val = m_pLua->GetNumber(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

float CLuaObject::GetMemberFloat(const char* name, float b)
{
	float val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Number)) {
				val = m_pLua->GetNumber(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

const char* CLuaObject::GetMemberStr(const char* name, const char* b)
{
	const char* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::String)) {
				val = m_pLua->GetString(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void* CLuaObject::GetMemberUserData(const char* name, void* b)
{
	void* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::UserData)) {
				val = m_pLua->GetUserdata(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void* CLuaObject::GetMemberUserData(float name, void* b)
{
	void* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(name);
			m_pLua->GetTable(-1);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::UserData)) {
				val = m_pLua->GetUserdata(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

GarrysMod::Lua::ILuaObject* CLuaObject::GetMember(const char* name, ILuaObject* b)
{
	ILuaObject* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table)) {
				CLuaObject* obj = new CLuaObject;
				obj->Init(m_pLua);
				obj->SetFromStack(-1);
				val = obj;
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

GarrysMod::Lua::ILuaObject* CLuaObject::GetMember(ILuaObject* name, ILuaObject* b)
{
	ILuaObject* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			name->Push();
			m_pLua->GetTable(-1);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table)) {
				CLuaObject* obj = new CLuaObject;
				obj->Init(m_pLua);
				obj->SetFromStack(-1);
				val = obj;
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void CLuaObject::SetMetaTable(ILuaObject* meta)
{
	m_metatable = ((CLuaObject*)meta)->m_reference;
}

void CLuaObject::SetUserData(void* data)
{
	UnReference();
	m_pLua->PushUserdata(data);
	SetFromStack(-1);
}

bool CLuaObject::isNil()
{
	return GetType() == GarrysMod::Lua::Type::Nil;
}

bool CLuaObject::isTable()
{
	return GetType() == GarrysMod::Lua::Type::Table;
}

bool CLuaObject::isString()
{
	return GetType() == GarrysMod::Lua::Type::String;
}

bool CLuaObject::isNumber()
{
	return GetType() == GarrysMod::Lua::Type::Number;
}

bool CLuaObject::isFunction()
{
	return GetType() == GarrysMod::Lua::Type::Function;
}

bool CLuaObject::isUserData()
{
	return GetType() == GarrysMod::Lua::Type::UserData;
}

GarrysMod::Lua::ILuaObject* CLuaObject::GetMember(float name, ILuaObject* b)
{
	ILuaObject* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(name);
			m_pLua->GetTable(-1);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table)) {
				CLuaObject* obj = new CLuaObject;
				obj->Init(m_pLua);
				obj->SetFromStack(-1);
				val = obj;
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void* CLuaObject::Remove_Me_1(const char* name, void*)
{
	Error("Now u made me Angry :< (Don't call Im_pLuaObject->Remove_Me_1)");
	return nullptr;
}

void CLuaObject::SetMember(float name)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->Push(-2);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(float name, ILuaObject *obj)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			obj->Push();
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(float name, float val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

/*void CLuaObject::SetMember(float name, float val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}*/

void CLuaObject::SetMember(float name, bool val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushBool(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(float name, const char* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushString(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(float name, GarrysMod::Lua::CFunc val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushCFunction(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

const char* CLuaObject::GetMemberStr(float name, const char* b)
{
	const char* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(name);
			m_pLua->GetTable(-1);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::String)) {
				val = m_pLua->GetString(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void CLuaObject::SetMember(ILuaObject* name, ILuaObject* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			name->Push();
			val->Push();
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

bool CLuaObject::GetBool()
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		bool val = m_pLua->GetBool(-1);
		m_pLua->Pop(1);
		return val;
	} else {
		return false;
	}
}

bool CLuaObject::PushMemberFast( int iStackPos )
{
	Error("What calls this? What does it do? (CLuaObject::PushMemberFast)");
	return false;
}

void CLuaObject::SetMemberFast(int name, int val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(name);
			m_pLua->PushNumber(val);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetFloat(float val)
{
	UnReference();
	m_pLua->PushNumber(val);
	SetFromStack(-1);
}

void CLuaObject::SetString(const char* val)
{
	UnReference();
	m_pLua->PushString(val);
	SetFromStack(-1);
}

void CLuaObject::SetMember_FixKey(const char* key, float val)
{
	Error("No.1");
}

void CLuaObject::SetMember_FixKey(const char* key, const char* val)
{
	Error("No.2");
}

void CLuaObject::SetMember_FixKey(const char* key, ILuaObject* val)
{
	Error("No.3");
}

void CLuaObject::SetMember_FixKey(const char* key, double val)
{
	Error("No.4");
}

void CLuaObject::SetMember_FixKey(const char* key, int val)
{
	Error("No.5");
}

bool CLuaObject::isBool()
{
	return GetType() == GarrysMod::Lua::Type::Bool;
}

void CLuaObject::SetMemberDouble(const char* name, double val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMemberNil(const char* name)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNil();
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMemberNil(float name)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(name);
			m_pLua->PushNil();
			m_pLua->SetTable(-2);
		}
		m_pLua->Pop(1);
	}
}

bool CLuaObject::RemoveMe()
{
	return false;
}

void CLuaObject::SetFromGlobal(const char*)
{
	Error("Look into it later. (Im_pLuaObject::SetFromGlobal)");
}

int CLuaObject::GetStringLen(unsigned int* idk)
{
	m_pLua->ReferencePush(m_reference);
	const char* str = m_pLua->GetString(-1);
	m_pLua->Pop(1);

	return strlen(str);
}

unsigned int CLuaObject::GetMemberUInt(const char* name, unsigned int b)
{
	unsigned int val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Number)) {
				val = m_pLua->GetNumber(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void CLuaObject::SetMember(const char* name, unsigned long long val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, int val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::RemoveMember(const char* name)
{
	SetMemberNil(name);
}

void CLuaObject::RemoveMember(float name)
{
	SetMemberNil(name);
}

bool CLuaObject::MemberIsNil(const char* name)
{
	bool val = true;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->GetType(-1) != GarrysMod::Lua::Type::Nil) {
				val = false;
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void CLuaObject::SetMemberDouble(float name, double val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

double CLuaObject::GetMemberDouble(const char* name, double b)
{
	double val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->GetType(-1) == GarrysMod::Lua::Type::Number) {
				val = m_pLua->GetNumber(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

BaseEntity* CLuaObject::GetMemberEntity(const char* name, BaseEntity* b)
{
	BaseEntity* val = b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->GetType(-1) == GarrysMod::Lua::Type::Entity) {
				val = (BaseEntity*)m_pLua->GetUserdata();
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return val;
}

void CLuaObject::SetMemberEntity(float name, BaseEntity* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushUserdata(val);
			m_pLua->PushNumber(name);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMemberEntity(const char* name, BaseEntity* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushUserdata(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

bool CLuaObject::isEntity()
{
	return GetType() == GarrysMod::Lua::Type::Entity;
}

BaseEntity* CLuaObject::GetEntity()
{
	m_pLua->ReferencePush(m_reference);
	BaseEntity* ent = (BaseEntity*)m_pLua->GetUserdata(-1);
	m_pLua->Pop(1);
	return ent;
}

void CLuaObject::SetEntity(BaseEntity* ent)
{
	UnReference();
	m_pLua->PushUserdata(ent);
	m_reference = m_pLua->ReferenceCreate();
	m_pLua->Pop(1);
}

void CLuaObject::SetMemberVector(const char* name, Vector* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushUserdata(val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMemberVector(const char* name, Vector& val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushUserdata(&val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMemberVector(float name, Vector* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushNumber(name);
			m_pLua->PushUserdata(&val);
			m_pLua->SetTable(-3);
		}
		m_pLua->Pop(1);
	}
}

Vector* CLuaObject::GetMemberVector(const char* name, const Vector* b)
{
	Vector val = *b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->GetType(-1) == GarrysMod::Lua::Type::Vector) {
				val = m_pLua->GetVector(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return &val;
}

Vector* CLuaObject::GetMemberVector(int what)
{
	Error("What? (CLuaObject::GetMemberVector)");

	return nullptr;
}

bool CLuaObject::isVector()
{
	return GetType() == GarrysMod::Lua::Type::Vector;
}

Vector* CLuaObject::GetVector()
{
	m_pLua->ReferencePush(m_reference);
	Vector vec = m_pLua->GetVector();
	m_pLua->Pop(1);

	return &vec;
}

void CLuaObject::SetMemberAngle(const char* name, QAngle& val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushUserdata(&val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

void CLuaObject::SetMemberAngle(const char* name, QAngle* val)
{
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->PushUserdata(&val);
			m_pLua->SetField(-2, name);
		}
		m_pLua->Pop(1);
	}
}

QAngle* CLuaObject::GetMemberAngle(const char* name, QAngle* b)
{
	QAngle val = *b;
	if (m_reference != -1)
	{
		m_pLua->ReferencePush(m_reference);
		if (m_pLua->IsType(-1, GarrysMod::Lua::Type::Table))
		{
			m_pLua->GetField(-1, name);
			if (m_pLua->GetType(-1) == GarrysMod::Lua::Type::Angle) {
				val = m_pLua->GetAngle(-1);
			}
			m_pLua->Pop(1);
		}
		m_pLua->Pop(1);
	}

	return &val;
}

bool CLuaObject::isAngle()
{
	return GetType() == GarrysMod::Lua::Type::Angle;
}

QAngle* CLuaObject::GetAngle()
{
	m_pLua->ReferencePush(m_reference);
	QAngle ang = m_pLua->GetAngle();
	m_pLua->Pop(1);

	return &ang;
}

void CLuaObject::SetMemberMatrix(const char* name, const VMatrix* val)
{
	Error("Not implemented! (CLuaObject::SetMemberMatrix)");
}

void CLuaObject::SetMemberMatrix(const char* name, const VMatrix& val)
{
	Error("Not implemented! (CLuaObject::SetMemberMatrix)");
}

void CLuaObject::SetMemberMatrix(float name, const VMatrix* val)
{
	Error("Not implemented! (CLuaObject::SetMemberMatrix)");
}

void CLuaObject::SetMemberMatrix(int name, const VMatrix* val)
{
	Error("Not implemented! (CLuaObject::SetMemberMatrix)");
}

void CLuaObject::SetMemberPhysObject(const char* name, IPhysicsObject* val)
{
	Error("Not implemented! (CLuaObject::SetMemberPhysObject)");
}