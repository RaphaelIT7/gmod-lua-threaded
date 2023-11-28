#include <convar.h>

class ILuaConVars
{
public:
	//virtual ~ILuaConVars( ) = 0;
	virtual void Init( ) = 0;
	virtual ConVar *CreateConVar( const char *, const char *, const char *, int ) = 0;
	virtual ConCommand *CreateConCommand( const char *, const char *, int, FnCommandCallback_t, FnCommandCompletionCallback) = 0;
	virtual void DestroyManaged( ) = 0;
	virtual void Cache( const char *, const char * ) = 0;
	virtual void ClearCache( ) = 0;
};