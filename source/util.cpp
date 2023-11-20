#include "util.h"
#include <string>

GarrysMod::Lua::ILuaBase* GlobalLUA;

void Start_Table() {
	GlobalLUA->PushSpecial(SPECIAL_GLOB);
	GlobalLUA->CreateTable();
}

void Add_Func(ILuaInterface* LUA, CFunc Func, const char* Name) {
	LUA->PushCFunction(Func);
	LUA->SetField(-2, Name);
}

void Add_Func(CFunc Func, const char* Name) {
	GlobalLUA->PushCFunction(Func);
	GlobalLUA->SetField(-2, Name);
}

void Add_Func(CFunc Func, std::string Name) {
	GlobalLUA->PushCFunction(Func);
	GlobalLUA->SetField(-2, Name.c_str());
}

void Finish_Table(const char* Name) {
	GlobalLUA->SetField(-2, Name);
	GlobalLUA->Pop();
}

void Finish_Table(std::string Name) {
	GlobalLUA->SetField(-2, Name.c_str());
	GlobalLUA->Pop();
}

// should never be used outside of main thread!!! what happends: memory access violation
void LuaPrint(const char* Text) {
	GlobalLUA->PushSpecial(SPECIAL_GLOB);
	GlobalLUA->GetField(-1, "print");
	GlobalLUA->PushString(Text);
	GlobalLUA->Call(1, 0);
}

// should never be used outside of main thread!!! what happends: memory access violation
void LuaPrint(std::string Text) {
	GlobalLUA->PushSpecial(SPECIAL_GLOB);
	GlobalLUA->GetField(-1, "print");
	GlobalLUA->PushString(Text.c_str());
	GlobalLUA->Call(1, 0);
}