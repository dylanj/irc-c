#include "def.h"
#include "irc.h"
#include "luafunc.h"


botState *LUAGetBotState( lua_State *L ) {
	lua_getglobal( L, "botptr" );
	return lua_touserdata( L, -1 );
}

int LUASendMessage( lua_State *L ) {
	char* target = (char *) lua_tostring( L, 1 );
	char* msg = (char *) lua_tostring( L, 2 );
	
	// retreive ptr to bot.
	botState *bot =	LUAGetBotState( L );
	IRCSendMessage( bot, target, msg );
	return 1;
}

int LUASetNick( lua_State *L ) {
	char *nick = (char *) lua_tostring( L, 1 );
	
	botState *b = LUAGetBotState( L );
	IRCSetNick( b, nick );
	return 1;
}

int LUAGetNick( lua_State *L ) {
	return 0;
}

int LUAJoinChannel( lua_State *L ) {
	printf( "lua - join channel\n" );
	char *channel = (char *) lua_tostring( L, 1 );
	botState *b = LUAGetBotState( L );
	
	IRCJoinChannel( b, channel );
	return 1;
}

int LUAGetChannels( lua_State *L ) {
	return 0;
}

int LUAGetUsers( lua_State *L ) {
	return 0;
}

