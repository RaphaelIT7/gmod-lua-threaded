#include "CLuaGameCallback.h"
#include "Color.h"
#include "detours.h"

GarrysMod::Lua::ILuaObject* CLuaGameCallback::CreateLuaObject()
{
	return func_CLuaGameCallback_CreateLuaObject(this);
}

void CLuaGameCallback::DestroyLuaObject(GarrysMod::Lua::ILuaObject* pObject)
{
	func_CLuaGameCallback_DestroyLuaObject(this, pObject);
}

void CLuaGameCallback::ErrorPrint(const char* error, bool print)
{
	func_CLuaGameCallback_ErrorPrint(this, error, print);
}

void CLuaGameCallback::Msg(const char* msg, bool unknown)
{
	func_CLuaGameCallback_Msg(this, msg, unknown);
}

void CLuaGameCallback::MsgColour(const char* msg, const Color& color)
{
	func_CLuaGameCallback_MsgColour(this, msg, color);
}

void CLuaGameCallback::LuaError(const CLuaError* error)
{
	func_CLuaGameCallback_LuaError(this, (void*)error);
}

void CLuaGameCallback::InterfaceCreated(GarrysMod::Lua::ILuaInterface* iface) {} // Unused