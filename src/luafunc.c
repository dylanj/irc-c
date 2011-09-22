#include "def.h"
#include "irc.h"
#include "plugin.h"
#include "luafunc.h"

botState *LUAGetBotState( lua_State *L ) {
    lua_getglobal( L, "botptr" );
    return lua_touserdata( L, -1 );
}

int LUAGetServerInfo( lua_State *L ) {
	botState *bot = LUAGetBotState( L );

	lua_newtable( L );

	lua_pushstring( L, "Nick" );
	lua_pushstring( L, bot->currentNick );
	lua_settable( L, -3 );
	
	lua_pushstring( L, "HostName" );
	lua_pushstring( L, bot->hostname );
	lua_settable( L, -3 );
	
	lua_pushstring( L, "FullName" );
	lua_pushstring( L, bot->fullname );
	lua_settable( L, -3 );
	
	lua_pushstring( L, "NetworkName" );
	lua_pushstring( L, bot->networkName );
	lua_settable( L, -3 );
	return 1;
}

void* LUAGetChannelsPointer( botState *bot ) {
	lua_State *botlua = bot->globalState;
	
	printf( "get channels pointer\n" );
	lua_getglobal( botlua, "Channels" );
	return ( void* ) lua_topointer( botlua, -1 );
}

int LUAReload( lua_State *L ) {
    botState *bot = LUAGetBotState( L );
    
	for( int i = 0; i < MAX_PLUGINS; i++ ) {
		if ( bot->pluginStates[i] == L ) {
			char *pluginPath;
			pluginPath = alloca( strlen( bot->pluginNames[i] ) + strlen( PLUGIN_PATH ) + 1 );

			sprintf( pluginPath, "%s%s", PLUGIN_PATH, bot->pluginNames[i] );
			printf( "reloading %s\n", pluginPath );
    		int result = luaL_dofile( L, pluginPath );
			printf( "result: %i\n", result );
			return 1;
		}
	}
		
    return 0;
}

int LUASendMessage( lua_State *L ) {
    char* target = (char *) lua_tostring( L, 1 );
    char* msg = (char *) lua_tostring( L, 2 );
    
    // retreive ptr to bot.
    botState *bot = LUAGetBotState( L );
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
	botState *b = LUAGetBotState( L );
	
	lua_pushstring( L, b->nick );
	printf( "pushed nick: %s\n", b->nick );
	return 1;
}

int LUAJoinChannel( lua_State *L ) {
    char *channel = (char *) lua_tostring( L, 1 );
    botState *b = LUAGetBotState( L );
    
    IRCJoinChannel( b, channel );
    return 1;
}

int LUAGetChannels( lua_State *L ) {
    botState *b = LUAGetBotState( L );
	void *channels = LUAGetChannelsPointer( b );
	
	if ( channels != NULL ) {
		lua_pushlightuserdata( L, channels );
		return 1;
	}

	return 0;
}

int LUAGetUsers( lua_State *L ) {
    return 0;
}

