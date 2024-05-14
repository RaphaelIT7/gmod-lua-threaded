#include "player.h"

// returns a CBaseEntity pointer to a player by index.  Only returns if the player is spawned and connected
// otherwise returns NULL
// Index is 1 based
CBasePlayer	*UTIL_PlayerByIndex( int playerIndex )
{
	CBasePlayer *pPlayer = NULL;

	if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients )
	{
		edict_t *pPlayerEdict = INDEXENT( playerIndex );
		if ( pPlayerEdict && !pPlayerEdict->IsFree() )
		{
			pPlayer = (CBasePlayer*)GetContainingEntity( pPlayerEdict );
		}
	}
	
	return pPlayer;
}