#include "events.h"
#include "channels.h"

void OnInvite( botState *bot, char *from, char *channel ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
        
		lua_getglobal( bot->pluginStates[i], "OnInvite" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;

        lua_pushstring( bot->pluginStates[i], from );
        lua_pushstring( bot->pluginStates[i], channel );
        lua_pcall( bot->pluginStates[i], 2, 0, 0 );
    }
}

void OnJoin( botState *bot, char* channel, char* user ) {
    
	AddChannel( bot, channel );
	
	for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
        
        lua_getglobal( bot->pluginStates[i], "OnJoin" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;

        lua_pushstring( bot->pluginStates[i], channel );
        lua_pushstring( bot->pluginStates[i], user );
        lua_pcall( bot->pluginStates[i], 2, 0, 0 );
    }

}

void OnKick( botState *bot, char *from, char *channel, char *user, char *msg ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;

        lua_getglobal( bot->pluginStates[i], "OnKick" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;

        lua_pushstring( bot->pluginStates[i], from );
        lua_pushstring( bot->pluginStates[i], channel );
        lua_pushstring( bot->pluginStates[i], user );
        lua_pushstring( bot->pluginStates[i], msg );
        lua_pcall( bot->pluginStates[i], 4, 0, 0 ); 
    }
}

void OnLoad( botState *bot, int pluginId ) {
	if ( bot->pluginStates[pluginId] == NULL )
		return;
    
	lua_getglobal( bot->pluginStates[pluginId], "OnLoad" );

    if ( lua_type( bot->pluginStates[pluginId], -1 ) != LUA_TFUNCTION )
        return;

    lua_pcall( bot->pluginStates[pluginId], 0, 0, 0 ); 
}

void OnMOTDFinish( botState *bot ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
        
		lua_getglobal( bot->pluginStates[i], "OnMOTDFinish" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;

        lua_pcall( bot->pluginStates[i], 0, 0, 0 );
    }
}

void OnNotice( botState *bot, char* from, char* to, char *msg ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
        
		lua_getglobal( bot->pluginStates[i], "OnNotice" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;

        lua_pushstring( bot->pluginStates[i], from );
        lua_pushstring( bot->pluginStates[i], to );
        lua_pushstring( bot->pluginStates[i], msg );
        lua_pcall( bot->pluginStates[i], 3, 0, 0 );
    }
}

void OnPart( botState *bot, char *user, char *channel, char *msg ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
	
		lua_getglobal( bot->pluginStates[i], "OnPart" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;

        lua_pushstring( bot->pluginStates[i], user );
        lua_pushstring( bot->pluginStates[i], channel );
        lua_pushstring( bot->pluginStates[i], msg );
        lua_pcall( bot->pluginStates[i], 3, 0, 0 );
    }
}

void OnPing( botState *bot, char* msg ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;

        lua_getglobal( bot->pluginStates[i], "OnPing" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;
        
        lua_pushstring( bot->pluginStates[i], msg );
        lua_pcall( bot->pluginStates[i], 1, 0, 0 );
    }
}

void OnPrivateMessage( botState *bot, char *from, char *to, char *msg ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
        
        lua_getglobal( bot->pluginStates[i], "OnPrivateMessage" );
        
        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;
		
		if ( from == NULL )
			printf( "from null\n" );
		if ( to == NULL )
			printf( "to null\n" );
		if ( msg == NULL )
			printf( "msg null\n" );
		if ( bot->pluginStates[i] == NULL )
			printf( "plugstate[%i] null\n", i );

        lua_pushstring( bot->pluginStates[i], from );
        lua_pushstring( bot->pluginStates[i], to );
        lua_pushstring( bot->pluginStates[i], msg );

		printf( "0x%p 0x%p (%s) 0x%p (%s) 0x%p (%s) \n", bot->pluginStates[i], from, from, to, to, msg, msg );

        lua_pcall( bot->pluginStates[i], 3, 0, 0 );
    }
}

void OnQuit( botState *bot, char *from, char *user, char *msg ) {
    for( int i = 0; i < bot->pluginCount; i++ ) {
		if ( bot->pluginStates[i] == NULL )
			continue;
        
        lua_getglobal( bot->pluginStates[i], "OnQuit" );

        if ( lua_type( bot->pluginStates[i], -1 ) != LUA_TFUNCTION )
            continue;
    
        lua_pushstring( bot->pluginStates[i], from );
        lua_pushstring( bot->pluginStates[i], user );
        lua_pushstring( bot->pluginStates[i], msg );
        lua_pcall( bot->pluginStates[i], 3, 0, 0 );
    }
}

void OnUnload( botState *bot, int pluginId ) {
	if ( bot->pluginStates[pluginId] == NULL )
		return;
        
    lua_getglobal( bot->pluginStates[pluginId], "OnUnload" );
    
    if ( lua_type( bot->pluginStates[pluginId], -1 ) != LUA_TFUNCTION )
        return;

    lua_pcall( bot->pluginStates[pluginId], 0, 0, 0 );
}
