#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/Lua/LuaGameCallback.h>
#include <string>
#include <vector>

using namespace GarrysMod::Lua;

struct CLuaError
{
	struct StackEntry
	{
		std::string source;
		std::string function;
		int line;
	};

	std::string message;
	std::string side;
	std::vector<StackEntry> stack;
};

class CLuaGameCallback : public ILuaGameCallback
{
public:
	IILuaObject *CreateLuaObject();
	void DestroyLuaObject(IILuaObject* pObject);
	void ErrorPrint(const char* error, bool);
	void Msg(const char* msg, bool);
	void MsgColour(const char* msg, const Color& color);
	void LuaError(const CLuaError* error);
	void InterfaceCreated(ILuaInterface* iface);
};