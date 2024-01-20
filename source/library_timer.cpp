#include "lua_threaded.h"
#include <chrono>

double GetTime()
{
	return std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
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
		if (timer->simple)
		{
			timers.push_back(timer);
			continue;
		}

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
		if (LUA->IsType(3, Type::Number)) {
			timer->repetitions = LUA->GetNumber(3);
		}

		if (LUA->IsType(4, Type::Function)) {
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
	LUA->CheckType(4, Type::Function);

	ILuaTimer* timer = new ILuaTimer;
	LUA->Push(4);
	timer->function = LUA->ReferenceCreate();
	LUA->Pop();

	timer->identifier = name;
	timer->delay = delay;
	timer->repetitions = repetitions;
	timer->next_run = delay;
	timer->next_run_time = GetTime() + delay;

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
		LUA->PushNumber(timer->repetitions);
	} else {
		LUA->PushNumber(0);
	}

	return 1;
}

LUA_FUNCTION(timer_Simple)
{
	ILuaThread* thread = GetValidThread(LUA, 1);
	double delay = LUA->CheckNumber(1);
	LUA->CheckType(2, Type::Function);

	ILuaTimer* timer = new ILuaTimer;
	LUA->Push(2);
	timer->function = LUA->ReferenceCreate();
	LUA->Pop();

	timer->simple = true;
	timer->delay = delay;
	timer->repetitions = 1;
	timer->next_run = delay;
	timer->next_run_time = GetTime() + delay;

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
		LUA->PushNumber(timer->next_run - GetTime());
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
		LUA->PushBool(true);
	} else {
		LUA->PushBool(false);
	}

	return 1;
}

void InitTimer(ILuaInterface* LUA)
{
	LUA->PushSpecial(SPECIAL_GLOB);
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
	ILuaInterface* LUA = thread->IFace;
	Msg("1\n");
	for (ILuaTimer* timer : thread->timers)
	{
		Msg("1.1\n");
		if (!timer->active) { continue; }
		
		timer->next_run = timer->next_run_time - time;

		Msg("1.2 Time: %f %f %f %f\n", timer->next_run, timer->next_run_time, time, timer->delay);
		if (timer->next_run <= 0)
		{
			Msg("1.2.1 Function called\n");
			timer->next_run_time = time + timer->delay;
			if (timer->repetitions > 0) {
				timer->repetitions = timer->repetitions - 1;
				if (timer->repetitions == 0)
				{
					Msg("1.3.1 Marked for Delete\n");
					timer->markdelete = true;
				}
			}

			LUA->ReferencePush(timer->function);
			LUA->PCall(0, 0, 0);
		}
	}

	Msg("2\n");
	RemoveTimers(thread);
}