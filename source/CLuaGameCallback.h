#include <GarrysMod/Lua/LuaInterface.h>
#include <GarrysMod/Lua/LuaGameCallback.h>
#include <string>
#include <vector>
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

class CLuaGameCallback : public GarrysMod::Lua::ILuaGameCallback
{
public:
	GarrysMod::Lua::ILuaObject *CreateLuaObject();
	void DestroyLuaObject(GarrysMod::Lua::ILuaObject* pObject);
	void ErrorPrint(const char* error, bool);
	void Msg(const char* msg, bool);
	void MsgColour(const char* msg, const Color& color);
	void LuaError(const CLuaError* error);
	void InterfaceCreated(GarrysMod::Lua::ILuaInterface* iface);
};