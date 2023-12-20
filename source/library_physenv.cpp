#include "vphysics_interface.h"
#include "vphysics/performance.h"
#include "lua_threaded.h"

IPhysics* phys;
IPhysicsEnvironment* physenv;

SourceSDK::FactoryLoader physics_loader("vphysics");

LUA_FUNCTION(physenv_AddSurfaceData) // ToDo
{
	return 0;
}

LUA_FUNCTION(physenv_GetAirDensity)
{
	if (!physenv)
		return 0;

	LUA->PushNumber(physenv->GetAirDensity());

	return 1;
}

LUA_FUNCTION(physenv_GetGravity)
{
	if (!physenv)
		return 0;

	Vector gravity;
	physenv->GetGravity(&gravity);

	Push_Vector(LUA, gravity);

	return 1;
}

LUA_FUNCTION(physenv_GetLastSimulationTime) // ToDo
{
	return 0;
}

LUA_FUNCTION(physenv_GetPerformanceSettings)
{
	if (!physenv)
		return 0;

	physics_performanceparams_t settings;
	physenv->GetPerformanceSettings(&settings);

	LUA->CreateTable();
		LUA->PushNumber(settings.maxCollisionsPerObjectPerTimestep);
		LUA->SetField(-2, "MaxCollisionsPerObjectPerTimestep");

		LUA->PushNumber(settings.maxCollisionChecksPerTimestep);
		LUA->SetField(-2, "MaxCollisionChecksPerTimestep");

		LUA->PushNumber(settings.maxVelocity);
		LUA->SetField(-2, "MaxVelocity");

		LUA->PushNumber(settings.maxAngularVelocity);
		LUA->SetField(-2, "MaxAngularVelocity");

		LUA->PushNumber(settings.lookAheadTimeObjectsVsWorld);
		LUA->SetField(-2, "LookAheadTimeObjectsVsWorld");

		LUA->PushNumber(settings.lookAheadTimeObjectsVsObject);
		LUA->SetField(-2, "LookAheadTimeObjectsVsObject");

		LUA->PushNumber(settings.minFrictionMass);
		LUA->SetField(-2, "MinFrictionMass");

		LUA->PushNumber(settings.maxFrictionMass);
		LUA->SetField(-2, "MaxFrictionMass");

	return 1;
}

LUA_FUNCTION(physenv_SetAirDensity)
{
	if (!physenv)
		return 0;

	int density = LUA->CheckNumber(1);
	physenv->SetAirDensity(density);

	return 0;
}

LUA_FUNCTION(physenv_SetGravity)
{
	if (!physenv)
		return 0;

	Vector gravity = Vector_Get(LUA, 1);
	physenv->SetGravity(gravity);

	return 0;
}

LUA_FUNCTION(physenv_SetPerformanceSettings)
{
	if (!physenv)
		return 0;

	LUA->CheckType(Type::Table, 1);

	physics_performanceparams_t settings;
	LUA->GetField(1, "MaxCollisionsPerObjectPerTimestep");
	settings.maxCollisionsPerObjectPerTimestep = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "MaxCollisionChecksPerTimestep");
	settings.maxCollisionChecksPerTimestep = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "MaxVelocity");
	settings.maxVelocity = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "MaxAngularVelocity");
	settings.maxAngularVelocity = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "LookAheadTimeObjectsVsWorld");
	settings.lookAheadTimeObjectsVsWorld = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "LookAheadTimeObjectsVsObject");
	settings.lookAheadTimeObjectsVsObject = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "MinFrictionMass");
	settings.minFrictionMass = LUA->CheckNumber(-1);
	LUA->Pop();

	LUA->GetField(1, "MaxFrictionMass");
	settings.maxFrictionMass = LUA->CheckNumber(-1);
	LUA->Pop();

	physenv->SetPerformanceSettings(&settings);

	return 0;
}

void InitPhysEnv(ILuaInterface* LUA)
{
	if (physenv == nullptr) {
		phys = (IPhysics*)physics_loader.GetFactory()(VPHYSICS_INTERFACE_VERSION, nullptr);
		if (phys == nullptr)
			LUA->ThrowError("unable to initialize IPhysics");


		physenv = phys->GetActiveEnvironmentByIndex(0);
		if (physenv == NULL)
			LUA->ThrowError("unable to get IPhysicsEnvironment");
	}

	// ToDo: Find out why does this break stuff :<

	LUA->PushSpecial(SPECIAL_GLOB);
		LUA->CreateTable();
			Add_Func(LUA, physenv_AddSurfaceData, "AddSurfaceData");
			Add_Func(LUA, physenv_GetAirDensity, "GetAirDensity");
			Add_Func(LUA, physenv_GetGravity, "GetGravity");
			Add_Func(LUA, physenv_GetLastSimulationTime, "GetLastSimulationTime");
			Add_Func(LUA, physenv_GetPerformanceSettings, "GetPerformanceSettings");
			Add_Func(LUA, physenv_SetAirDensity, "SetAirDensity");
			Add_Func(LUA, physenv_SetGravity, "SetGravity");
			Add_Func(LUA, physenv_SetPerformanceSettings, "SetPerformanceSettings");

		LUA->SetField(-2, "physenv");
	LUA->Pop();
}