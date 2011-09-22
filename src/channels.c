#include "channels.h"

void AddChannel( botState *bot, char* channel ) {
	lua_State *L = bot->globalState;

	lua_getglobal( L, "Channels" );

	if ( lua_type( L, -1 ) != LUA_TTABLE ) {
		lua_newtable( L );
		lua_setglobal( L, "Channels" );
	}

	lua_getglobal( L, "Channels") ;
	
	// add new channel
	lua_pushstring( L, channel ); // key for new table
	lua_newtable( L );
	lua_settable( L, -3 );
	
	lua_pushstring( L, "users" );
	lua_pushnumber( L, 52 );
	lua_settable( L, -3 );
	
	lua_pushstring( L, "topic" );
	lua_pushstring( L, "example topic goes here" );
	lua_settable( L, -3 );	
}

void RemoveChannel( botState *bot, char* channel ) {
	printf( "removing %s from %s\n", channel, bot->nick );
}

