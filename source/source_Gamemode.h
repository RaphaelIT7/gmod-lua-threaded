#pragma once

#include <string>
#include <list>
#include <cstdint>

namespace New_IGamemodeSystem
{

struct Information
{
	bool placeholder1;
	bool menusystem;
	std::string title;
	std::string name;
	std::string maps;
	std::string basename;
	std::string category;
	uint64_t workshopid;
};

}

namespace New_Gamemode
{

class System
{
public:
	virtual void OnJoinServer( const std::string & ) = 0;
#ifndef GMOD_DEDICATED
	virtual void OnLeaveServer( ) = 0;
#endif
	virtual void Refresh( ) = 0;
	virtual void Clear( ) = 0;
	virtual const IGamemodeSystem::Information &Active( ) = 0;
	virtual IGamemodeSystem::Information FindByName( const std::string & ) = 0; // Guessing that it returns IGamemodeSystem::Information
	virtual void SetActive( const std::string & ) = 0;
	virtual const std::list<IGamemodeSystem::Information> &GetList( ) const = 0;
#ifndef GMOD_DEDICATED
	virtual bool IsServerBlacklisted( char const* address, char const* hostname, char const* description, char const* gm, char const* map ) = 0;
#endif
};

}