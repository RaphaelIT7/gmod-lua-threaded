#include "CLuaObject.h"

using namespace GarrysMod::Lua;

void CLuaObject::Init()
{
	Error("What called this?");
}

void CLuaObject::Init(ILuaInterface* LUA)
{
	this->LUA = LUA;
}

void CLuaObject::UnReference()
{
	LUA->ReferenceFree(m_reference);
	m_reference = -1;
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
	LUA->Push(pos);

	if (LUA->GetType(-1) != Type::Nil)
	{
		UnReference();
		m_reference = LUA->ReferenceCreate();
	}

	LUA->Pop(1);
}

void CLuaObject::Push()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (m_metatable != -1) {
			LUA->ReferencePush(m_metatable);
			LUA->SetMetaTable(-2);
		}
	} else {
		LUA->PushNil();
	}
}

int CLuaObject::GetType()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		int type = LUA->GetType(-1);
		LUA->Pop(1);
		return type;
	} else {
		return Type::Nil;
	}
}

const char* CLuaObject::GetString()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		const char* str = LUA->GetString(-1);
		LUA->Pop(1);
		return str == NULL ? "" : str;
	} else {
		return "";
	}
}

float CLuaObject::GetFloat()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		float num = LUA->GetNumber(-1);
		LUA->Pop(1);
		return num;
	} else {
		return 0;
	}
}

int CLuaObject::GetInt()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		int num = LUA->GetNumber(-1);
		LUA->Pop(1);
		return num;
	} else {
		return 0;
	}
}

double CLuaObject::GetDouble()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		double num = LUA->GetNumber(-1);
		LUA->Pop(1);
		return num;
	} else {
		return 0;
	}
}

void* CLuaObject::GetUserData()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		void* data = LUA->GetUserdata(-1);
		LUA->Pop(1);
		return data;
	} else {
		return 0;
	}
}

void CLuaObject::SetMember(const char* name)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->Push(-2);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, ILuaObject *obj)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			obj->Push();
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, float val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, bool val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushBool(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, const char* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushString(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, CFunc val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushCFunction(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

bool CLuaObject::GetMemberBool(const char* name, bool b)
{
	bool val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::Bool)) {
				val = LUA->GetBool(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

int CLuaObject::GetMemberInt(const char* name, int b)
{
	int val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::Number)) {
				val = LUA->GetNumber(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

float CLuaObject::GetMemberFloat(const char* name, float b)
{
	float val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::Number)) {
				val = LUA->GetNumber(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

const char* CLuaObject::GetMemberStr(const char* name, const char* b)
{
	const char* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::String)) {
				val = LUA->GetString(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void* CLuaObject::GetMemberUserData(const char* name, void* b)
{
	void* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::UserData)) {
				val = LUA->GetUserdata(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void* CLuaObject::GetMemberUserData(float name, void* b)
{
	void* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(name);
			LUA->GetTable(-1);
			if (LUA->IsType(-1, Type::UserData)) {
				val = LUA->GetUserdata(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

ILuaObject* CLuaObject::GetMember(const char* name, ILuaObject* b)
{
	ILuaObject* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::Table)) {
				CLuaObject* obj = new CLuaObject;
				obj->Init(LUA);
				obj->SetFromStack(-1);
				val = obj;
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

ILuaObject* CLuaObject::GetMember(ILuaObject* name, ILuaObject* b)
{
	ILuaObject* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			name->Push();
			LUA->GetTable(-1);
			if (LUA->IsType(-1, Type::Table)) {
				CLuaObject* obj = new CLuaObject;
				obj->Init(LUA);
				obj->SetFromStack(-1);
				val = obj;
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
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
	LUA->PushUserdata(data);
	SetFromStack(-1);
}

bool CLuaObject::isNil()
{
	return GetType() == Type::Nil;
}

bool CLuaObject::isTable()
{
	return GetType() == Type::Table;
}

bool CLuaObject::isString()
{
	return GetType() == Type::String;
}

bool CLuaObject::isNumber()
{
	return GetType() == Type::Number;
}

bool CLuaObject::isFunction()
{
	return GetType() == Type::Function;
}

bool CLuaObject::isUserData()
{
	return GetType() == Type::UserData;
}

ILuaObject* CLuaObject::GetMember(float name, ILuaObject* b)
{
	ILuaObject* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(name);
			LUA->GetTable(-1);
			if (LUA->IsType(-1, Type::Table)) {
				CLuaObject* obj = new CLuaObject;
				obj->Init(LUA);
				obj->SetFromStack(-1);
				val = obj;
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void* CLuaObject::Remove_Me_1(const char* name, void*)
{
	Error("Now u made me Angry :< (Don't call ILuaObject->Remove_Me_1)");
}

void CLuaObject::SetMember(float name)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->Push(-2);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(float name, ILuaObject *obj)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			obj->Push();
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(float name, float val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

/*void CLuaObject::SetMember(float name, float val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}*/

void CLuaObject::SetMember(float name, bool val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushBool(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(float name, const char* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushString(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(float name, CFunc val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushCFunction(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

const char* CLuaObject::GetMemberStr(float name, const char* b)
{
	const char* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(name);
			LUA->GetTable(-1);
			if (LUA->IsType(-1, Type::String)) {
				val = LUA->GetString(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void CLuaObject::SetMember(ILuaObject* name, ILuaObject* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			name->Push();
			val->Push();
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

bool CLuaObject::GetBool()
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		bool val = LUA->GetBool(-1);
		LUA->Pop(1);
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
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(name);
			LUA->PushNumber(val);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetFloat(float val)
{
	UnReference();
	LUA->PushNumber(val);
	SetFromStack(-1);
}

void CLuaObject::SetString(const char* val)
{
	UnReference();
	LUA->PushString(val);
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
	return GetType() == Type::Bool;
}

void CLuaObject::SetMemberDouble(const char* name, double val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMemberNil(const char* name)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNil();
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMemberNil(float name)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(name);
			LUA->PushNil();
			LUA->SetTable(-2);
		}
		LUA->Pop(1);
	}
}

bool CLuaObject::RemoveMe()
{
	return false;
}

void CLuaObject::SetFromGlobal(const char*)
{
	Error("Look into it later. (ILuaObject::SetFromGlobal)");
}

int CLuaObject::GetStringLen(unsigned int* idk)
{
	Error("Look into it later. (ILuaObject::GetStringLen)");
}

unsigned int CLuaObject::GetMemberUInt(const char* name, unsigned int b)
{
	unsigned int val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->IsType(-1, Type::Number)) {
				val = LUA->GetNumber(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void CLuaObject::SetMember(const char* name, unsigned long long val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMember(const char* name, int val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
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
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->GetType(-1) != Type::Nil) {
				val = false;
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void CLuaObject::SetMemberDouble(float name, double val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

double CLuaObject::GetMemberDouble(const char* name, double b)
{
	double val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->GetType(-1) == Type::Number) {
				val = LUA->GetNumber(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

BaseEntity* CLuaObject::GetMemberEntity(const char* name, BaseEntity* b)
{
	BaseEntity* val = b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->GetType(-1) == Type::Entity) {
				val = (BaseEntity*)LUA->GetUserdata();
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return val;
}

void CLuaObject::SetMemberEntity(float name, BaseEntity* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushUserdata(val);
			LUA->PushNumber(name);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMemberEntity(const char* name, BaseEntity* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushUserdata(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

bool CLuaObject::isEntity()
{
	return GetType() == Type::Entity;
}

BaseEntity* CLuaObject::GetEntity()
{
	LUA->ReferencePush(m_reference);
	BaseEntity* ent = (BaseEntity*)LUA->GetUserdata(-1);
	LUA->Pop(1);
	return ent;
}

void CLuaObject::SetEntity(BaseEntity* ent)
{
	UnReference();
	LUA->PushUserdata(ent);
	m_reference = LUA->ReferenceCreate();
	LUA->Pop(1);
}

void CLuaObject::SetMemberVector(const char* name, Vector* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushUserdata(val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMemberVector(const char* name, Vector& val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushUserdata(&val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMemberVector(float name, Vector* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushNumber(name);
			LUA->PushUserdata(&val);
			LUA->SetTable(-3);
		}
		LUA->Pop(1);
	}
}

Vector* CLuaObject::GetMemberVector(const char* name, const Vector* b)
{
	Vector val = *b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->GetType(-1) == Type::Vector) {
				val = LUA->GetVector(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
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
	return GetType() == Type::Vector;
}

Vector* CLuaObject::GetVector()
{
	LUA->ReferencePush(m_reference);
	Vector vec = LUA->GetVector();
	LUA->Pop(1);
	return &vec;
}

void CLuaObject::SetMemberAngle(const char* name, QAngle& val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushUserdata(&val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

void CLuaObject::SetMemberAngle(const char* name, QAngle* val)
{
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->PushUserdata(&val);
			LUA->SetField(-2, name);
		}
		LUA->Pop(1);
	}
}

QAngle* CLuaObject::GetMemberAngle(const char* name, QAngle* b)
{
	QAngle val = *b;
	if (m_reference != -1)
	{
		LUA->ReferencePush(m_reference);
		if (LUA->IsType(-1, Type::Table))
		{
			LUA->GetField(-1, name);
			if (LUA->GetType(-1) == Type::Angle) {
				val = LUA->GetAngle(-1);
			}
			LUA->Pop(1);
		}
		LUA->Pop(1);
	}

	return &val;
}

bool CLuaObject::isAngle()
{
	return GetType() == Type::Angle;
}

QAngle* CLuaObject::GetAngle()
{
	LUA->ReferencePush(m_reference);
	QAngle vec = LUA->GetAngle();
	LUA->Pop(1);
	return &vec;
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