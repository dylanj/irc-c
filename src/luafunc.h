#ifndef LUAFUNC_H
#define LUAFUNC_H

#include "def.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// functions called from within lua.
int LUAJoinChannel( lua_State *L );
int LUAGetChannels( lua_State *L );
int LUAGetNick( lua_State *L );
int LUAGetServerInfo( lua_State *L );
int LUAGetUsers( lua_State *L );
int LUASendMessage( lua_State *L );
int LUASetNick( lua_State *L );
int LUAReload( lua_State *L );

botState *LUAGetBotState( lua_State *L );
void	 *LUAGetChannelsPointer( botState *bot );
	
#endif
