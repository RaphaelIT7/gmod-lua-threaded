#include "lua_threaded.h"
#include <chrono>

double GetTime()
{
	return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() / 1000;
}

ILuaTimer* FindTimer(ILuaThread* thread, const char* name)
{
	for (ILuaTimer* timer : thread->timers)
	{
		if (!timer->simple && strcmp(name, timer->identifier) == 0)
		{
			return timer;
		}
	}

	return nullptr;
}

void RemoveTimers(ILuaThread* thread)
{
	std::vector<ILuaTimer*> timers;
	for (ILuaTimer* timer : thread->timers)
	{
		if (timer->markdelete)
		{
			delete timer;
		} else {
			timers.push_back(timer);
		}
	}

	thread->timers = timers;
}

LUA_FUNCTION(timer_Adjust)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);
	double delay = LUA->CheckNumber(2);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		timer->delay = delay;
		if (LUA->IsType(3, GarrysMod::Lua::Type::Number)) {
			timer->repetitions = LUA->GetNumber(3);
		}

		if (LUA->IsType(4, GarrysMod::Lua::Type::Function)) {
			LUA->ReferenceFree(timer->function);
			LUA->Push(4);
			timer->function = LUA->ReferenceCreate();
			LUA->Pop();
		}

		LUA->PushBool(true);
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

LUA_FUNCTION(timer_Check)
{
	return 0;
}

LUA_FUNCTION(timer_Create)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);
	double delay = LUA->CheckNumber(2);
	double repetitions = LUA->CheckNumber(3);
	LUA->CheckType(4, GarrysMod::Lua::Type::Function);

	ILuaTimer* timer = FindTimer(thread, name); // Reuse existing timer
	if (!timer)
	{
		timer = new ILuaTimer;
	}

	LUA->Push(4);
	timer->function = LUA->ReferenceCreate();
	LUA->Pop();

	timer->identifier = name;
	timer->delay = delay;
	timer->repetitions = repetitions;
	timer->next_run = delay;
	timer->next_run_time = GetTime() + delay;
	thread->timers.push_back(timer);

	return 0;
}

LUA_FUNCTION(timer_Exists)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	LUA->PushBool(FindTimer(thread, name) != nullptr);

	return 1;
}

LUA_FUNCTION(timer_Pause)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		if (timer->active) {
			timer->active = false;
			LUA->PushBool(true);
		} else {
			LUA->PushBool(false);
		}
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

LUA_FUNCTION(timer_Remove)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		timer->markdelete = true;
		RemoveTimers(thread);
	}

	return 0;
}

LUA_FUNCTION(timer_RepsLeft)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		if (timer->repetitions > 0) {
			LUA->PushNumber(timer->repetitions);
		} else {
			LUA->PushNumber(0);
		}
	} else {
		LUA->PushNumber(0);
	}

	return 1;
}

LUA_FUNCTION(timer_Simple)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	double delay = LUA->CheckNumber(1);
	LUA->CheckType(2, GarrysMod::Lua::Type::Function);

	ILuaTimer* timer = new ILuaTimer;
	LUA->Push(2);
	timer->function = LUA->ReferenceCreate();
	LUA->Pop();

	timer->simple = true;
	timer->delay = delay;
	timer->repetitions = 1;
	timer->next_run = delay;
	timer->next_run_time = GetTime() + delay;
	thread->timers.push_back(timer);

	return 0;
}

LUA_FUNCTION(timer_Start)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		if (!timer->active) {
			timer->active = false;
			timer->next_run_time = GetTime() + timer->delay;
			LUA->PushBool(true);
		} else {
			LUA->PushBool(false);
		}
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

LUA_FUNCTION(timer_Stop)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		if (timer->active) {
			timer->active = false;
			timer->next_run = timer->delay;
			LUA->PushBool(true);
		} else {
			LUA->PushBool(false);
		}
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

LUA_FUNCTION(timer_TimeLeft)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		LUA->PushNumber(timer->next_run);
	} else {
		LUA->PushNumber(0);
	}

	return 1;
}

LUA_FUNCTION(timer_Toggle)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		timer->active = !timer->active;
		timer->next_run_time = GetTime() + timer->next_run;
		LUA->PushBool(timer->active);
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

LUA_FUNCTION(timer_UnPause)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	const char* name = LUA->CheckString(1);

	ILuaTimer* timer = FindTimer(thread, name);
	if (timer) {
		timer->active = true;
		timer->next_run_time = GetTime() + timer->next_run;
		LUA->PushBool(true);
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

void InitTimer(GarrysMod::Lua::ILuaInterface* LUA)
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, timer_Adjust, "Adjust");
			Add_Func(LUA, timer_Check, "Check");
			Add_Func(LUA, timer_Create, "Create");
			Add_Func(LUA, timer_Remove, "Destroy");
			Add_Func(LUA, timer_Exists, "Exists");
			Add_Func(LUA, timer_Pause, "Pause");
			Add_Func(LUA, timer_Remove, "Remove");
			Add_Func(LUA, timer_RepsLeft, "RepsLeft");
			Add_Func(LUA, timer_Simple, "Simple");
			Add_Func(LUA, timer_Start, "Start");
			Add_Func(LUA, timer_Stop, "Stop");
			Add_Func(LUA, timer_TimeLeft, "TimeLeft");
			Add_Func(LUA, timer_Toggle, "Toggle");
			Add_Func(LUA, timer_UnPause, "UnPause");

		LUA->SetField(-2, "timer");
	LUA->Pop();
}

void TimerThink(ILuaThread* thread)
{
	double time = GetTime();
	GarrysMod::Lua::ILuaInterface* LUA = thread->IFace;
	for (ILuaTimer* timer : thread->timers)
	{
		if (!timer->active) { continue; }
		
		timer->next_run = timer->next_run_time - time;

		if (timer->next_run <= 0)
		{
			timer->next_run_time = time + timer->delay;

			LUA->ReferencePush(timer->function);
			LUA->PCall(0, 0, 0);

			if (timer->repetitions == 1)
			{
				timer->markdelete = true;
			} else {
				timer->repetitions--;
			}
		}
	}

	RemoveTimers(thread);
}