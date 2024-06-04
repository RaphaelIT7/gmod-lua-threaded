#include <GarrysMod/Lua/LuaObject.h>
#include <GarrysMod/Lua/LuaInterface.h>

class CLuaObject : GarrysMod::Lua::ILuaObject
{
public:
	virtual void Set( ILuaObject *obj );
	virtual void SetFromStack( int i );
	virtual void UnReference( );

	virtual int GetType( );
	virtual const char *GetString( );
	virtual float GetFloat( );
	virtual int GetInt( );
	virtual void *GetUserData( );

	virtual void SetMember( const char *name );
	virtual void SetMember( const char *name, ILuaObject *obj );
	virtual void SetMember( const char *name, float val );
	virtual void SetMember( const char *name, bool val );
	virtual void SetMember( const char *name, const char *val );
	virtual void SetMember( const char *name, GarrysMod::Lua::CFunc f );

	virtual bool GetMemberBool( const char *name, bool b = true );
	virtual int GetMemberInt( const char *name, int i = 0 );
	virtual float GetMemberFloat( const char *name, float f = 0.0f );
	virtual const char *GetMemberStr( const char *name, const char *s = "" );
	virtual void *GetMemberUserData( const char *name, void *u = 0 );
	virtual void *GetMemberUserData( float name, void *u = 0 );
	virtual ILuaObject *GetMember( const char *name, ILuaObject *obj );
	virtual ILuaObject *GetMember( ILuaObject *key, ILuaObject *obj );

	virtual void SetMetaTable( ILuaObject *obj );
	virtual void SetUserData( void *obj );

	virtual void Push( );

	virtual bool isNil( );
	virtual bool isTable( );
	virtual bool isString( );
	virtual bool isNumber( );
	virtual bool isFunction( );
	virtual bool isUserData( );

	virtual ILuaObject *GetMember( float fKey, ILuaObject* obj );

	virtual void *Remove_Me_1( const char *name, void * = 0 );

	virtual void SetMember( float fKey );
	virtual void SetMember( float fKey, ILuaObject *obj );
	virtual void SetMember( float fKey, float val );
	virtual void SetMember( float fKey, bool val );
	virtual void SetMember( float fKey, const char *val );
	virtual void SetMember( float fKey, GarrysMod::Lua::CFunc f );

	virtual const char *GetMemberStr( float name, const char *s = "" );

	virtual void SetMember( ILuaObject *k, ILuaObject *v );
	virtual bool GetBool( );

	virtual bool PushMemberFast( int iStackPos );
	virtual void SetMemberFast( int iKey, int iValue );

	virtual void SetFloat( float val );
	virtual void SetString( const char *val );

	virtual double GetDouble( );

	virtual void SetMember_FixKey( const char *, float );
	virtual void SetMember_FixKey( const char *, const char * );
	virtual void SetMember_FixKey( const char *, ILuaObject * );
	virtual void SetMember_FixKey( const char *, double );
	virtual void SetMember_FixKey( const char *, int );

	virtual bool isBool( );

	virtual void SetMemberDouble( const char *, double );

	virtual void SetMemberNil( const char * );
	virtual void SetMemberNil( float );

	virtual bool RemoveMe( );

	virtual void Init( );

	virtual void SetFromGlobal( const char * );

	virtual int GetStringLen( unsigned int * );

	virtual unsigned int GetMemberUInt( const char *, unsigned int );

	virtual void SetMember( const char *, unsigned long long );
	virtual void SetMember( const char *, int );
	virtual void SetReference( int );

	virtual void RemoveMember( const char * );
	virtual void RemoveMember( float );

	virtual bool MemberIsNil( const char * );

	virtual void SetMemberDouble( float, double );
	virtual double GetMemberDouble( const char *, double );

	virtual BaseEntity *GetMemberEntity( const char *, BaseEntity * );
	virtual void SetMemberEntity( float, BaseEntity * );
	virtual void SetMemberEntity( const char *, BaseEntity * );
	virtual bool isEntity( );
	virtual BaseEntity *GetEntity( );
	virtual void SetEntity( BaseEntity * );

	virtual void SetMemberVector( const char *, Vector * );
	virtual void SetMemberVector( const char *, Vector & );
	virtual void SetMemberVector( float, Vector * );
	virtual Vector *GetMemberVector( const char *, const Vector * );
	virtual Vector *GetMemberVector( int );
	virtual Vector *GetVector( );
	virtual bool isVector( );

	virtual void SetMemberAngle( const char *, QAngle * );
	virtual void SetMemberAngle( const char *, QAngle & );
	virtual QAngle *GetMemberAngle( const char *, QAngle * );
	virtual QAngle *GetAngle( );
	virtual bool isAngle( );

	virtual void SetMemberMatrix( const char *, VMatrix const * );
	virtual void SetMemberMatrix( const char *, VMatrix const & );
	virtual void SetMemberMatrix( float, VMatrix const * );
	virtual void SetMemberMatrix( int, VMatrix const * );

	virtual void SetMemberPhysObject( const char *, IPhysicsObject * );
public:
	void Init(GarrysMod::Lua::ILuaBase*);
protected:
	bool m_bUserData;
	int m_iLUA_TYPE;
	int m_reference;
	GarrysMod::Lua::ILuaBase* m_pLua;
protected:
	int m_metatable;
};