#include <GarrysMod/InterfacePointers.hpp>
#include "lua_threaded.h"

LUA_FUNCTION(gmod_GetGamemode)
{
	int reference = -1;
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->GetField(-1, "GM");
			if (LUA->IsType(1, Type::Table))
			{
				reference = LUA->ReferenceCreate();
				LUA->Pop();
			} else {
				LUA->Pop();
				LUA->GetField(-1, "GAMEMODE");
				if (LUA->IsType(1, Type::Table))
				{
					reference = LUA->ReferenceCreate();
				}

				LUA->Pop();
			}
	LUA->Pop();

	if (reference > 0)
	{
		LUA->ReferencePush(reference);
	} else {
		LUA->PushNil();
	}

	return 1;
}

void InitGmodLib(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, gmod_GetGamemode, "GetGamemode");
			
		LUA->SetField(-2, "gmod");
	LUA->Pop();
}