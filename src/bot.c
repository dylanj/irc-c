#include "bot.h"
#include "events.h"

botState BotStates[MAX_BOTS];
int botCount = 0;

// connect bot to irc network.
int BotConnect( botState *bot ) {
    
    BotInfo( bot );

    struct sockaddr_in server_addr;
    struct hostent *server_host;

    bot->socket = socket( AF_INET, SOCK_STREAM, 0 );

    if ( bot->socket < 0 ) {
        fprintf( stderr, "Failed to create socket [%i]\n", errno );
        perror( "socket" );
        return 0;
    }

    printf( "trying to resolve hostname: %s\n", bot->hostname );
    server_host = gethostbyname( bot->hostname );
    if ( server_host == NULL ) {
        fprintf( stderr, "failed to lookup hostname [%i]\n", errno );
        perror( "gethostbyname" );
        return 0;
    }
    
    server_addr.sin_addr = *(struct in_addr *) server_host->h_addr_list[0];
    server_addr.sin_port = htons( bot->port );
    server_addr.sin_family = AF_INET;
    
    if ( connect( bot->socket, (struct sockaddr*) &server_addr, sizeof( server_addr ) ) < 0 ) {
        fprintf( stderr, "error connecting[%i]", errno );
        perror( "connect" );
        return 0;
    }

    printf( "connected!\n" );

    return 1;
}

// these are events that can be called from within lua scripts
void BotGiveEvents( botState *bot, int pluginId ) {
    lua_register( bot->pluginStates[pluginId], "SendMessage", LUASendMessage );
    lua_register( bot->pluginStates[pluginId], "SetNick", LUASetNick );
    lua_register( bot->pluginStates[pluginId], "GetNick", LUAGetNick );
    lua_register( bot->pluginStates[pluginId], "JoinChannel", LUAJoinChannel );
    lua_register( bot->pluginStates[pluginId], "GetChannels", LUAGetChannels );
    lua_register( bot->pluginStates[pluginId], "GetUsers", LUAGetUsers );
    lua_register( bot->pluginStates[pluginId], "GetServerInfo", LUAGetServerInfo );
}

// print info about bot
void BotInfo( botState *bot ) {
	printf( "maxnicklength: %i\nawaylength: %i\ntopiclength: %i\nkicklength: %i\nchannellength: %i\nmaxchannellen: %i\nnetwork name: %s\n",	bot->maxnicklen, bot->awaylen, bot->topiclength, bot->kicklen, bot->channellen, bot->maxchannellen, bot->networkName );
    
	printf( "nick: %s\naltnick: %s\nhostname: %s\nfullname: %s\n server: %s\nautosendcmd: %s\nport: %i\nsocket: %i\nplugin count: %i\nplugins:\n", bot->nick, bot->altnick, bot->hostname, bot->fullname, bot->server, bot->autosendcmd, bot->port, bot->socket, bot->pluginCount );
   	 
    for( int i = 0; i < bot->pluginCount; i++ ) {
        printf( "%i: %s\n", i, bot->pluginNames[i] );
    }
}

// loads a new bot
int BotLoad( lua_State *L ) {
    char *filename = (char*) lua_tostring(L, 1);
    char *nick, *altnick, *fullname, *server, *autosendcmd;
    
    BotStates[botCount].log = fopen( "./raw.log", "w+" );
    BotStates[botCount].globalState = lua_open();

	if ( BotStates[botCount].log == NULL ) {
        printf( "problem opening dealing with logfile\n" );
    }
    
    Log( &BotStates[botCount], "!!!\nloading bot: %s\n", filename );

    if ( strlen( filename ) == 0 ) {
        #if DEBUG
        fprintf( stderr, "failed loading bot\n" );
        #endif
        lua_pushboolean( L, 0 );
        return 0;
    }
    
    char* botConfigPath = alloca( strlen( filename ) + strlen( CONFIG_PATH ) );
    sprintf( botConfigPath, "%s%s", CONFIG_PATH, filename );

    if ( luaL_loadfile( L, botConfigPath ) == LUA_ERRFILE ) {
        fprintf( stderr, "lua couldn't open %s\n", botConfigPath);
        return 0;
    }

    lua_pcall( L, 0, 0, 0 );

    // push values on to stack
    lua_getglobal(L, "nick");
    lua_getglobal(L, "altnick");
    lua_getglobal(L, "fullname");
    lua_getglobal(L, "server");
    lua_getglobal(L, "autosendcmd");

    // grab pointers to items on stack.
    nick = (char*)lua_tostring( L, -5 );
    altnick = (char*)lua_tostring( L, -4 );
    fullname = (char*)lua_tostring( L, -3 );
    server = (char*)lua_tostring( L, -2 );
    autosendcmd = (char*)lua_tostring( L, -1 );
    
    // allocate memory for items.
    BotStates[botCount].nick = malloc( strlen( nick ) + 1);
    BotStates[botCount].altnick = malloc( strlen( altnick ) + 1);
    BotStates[botCount].fullname = malloc( strlen( fullname ) + 1 );
    BotStates[botCount].server = malloc( strlen( server ) + 1 );
    BotStates[botCount].autosendcmd = malloc( strlen( autosendcmd ) + 1);
    
    BotStates[botCount].rawCarry = malloc( SOCKET_BUF_LEN );
    BotStates[botCount].msg = malloc( PROTOCOL_FIELDS * sizeof( char* ) );
        
    for( int i = 0; i < PROTOCOL_FIELDS; i++ )
        BotStates[botCount].msg[i] = malloc( SOCKET_BUF_LEN );

    memset( BotStates[botCount].rawCarry, 0, SOCKET_BUF_LEN );
    
    // copy items from lua
    strcpy( BotStates[botCount].nick, nick );
    strcpy( BotStates[botCount].altnick, altnick );
    strcpy( BotStates[botCount].fullname, nick );
    strcpy( BotStates[botCount].server, server );
    strcpy( BotStates[botCount].autosendcmd, autosendcmd );
    
    BotStates[botCount].pluginCount = 0;

    printf( "plugins to load: \n" );
    lua_getglobal( L, "plugins" ); 
    
    // iterate through "plugins" table
    int t = lua_gettop(L);
    lua_pushnil(L);  /* first key */    
    while (lua_next(L, t) != 0) {
        char *pluginName = (char*)lua_tostring( L, -1 );
        printf( "%s: ", pluginName );

        if ( BotLoadPlugin( &BotStates[botCount], pluginName ) ) {
            Log( &BotStates[botCount], "successully loaded %s\n", pluginName );
            printf( "success\n" );
        } else {
            Log( &BotStates[botCount], "could not load %s\n", pluginName );
            printf( "failed\n" );
        }

        lua_pop(L, 1);
    }
    
    // set up connection info
    if ( strstr( BotStates[botCount].server, ":" ) != NULL ) {
        // allocate memory for hostname
        printf( "this server has a port defined\n" );
        BotStates[botCount].hostname = malloc( strlen( BotStates[botCount].server ) );
        
        // seperate hostname and port
        sscanf( BotStates[botCount].server, "%[a-zA-Z0-9.]:%d", 
            BotStates[botCount].hostname,
            &BotStates[botCount].port );

        if ( BotStates[botCount].port == 0 )
            BotStates[botCount].port = DEFAULT_IRC_PORT;
        
    } else {
        BotStates[botCount].hostname = BotStates[botCount].server;
        BotStates[botCount].port = DEFAULT_IRC_PORT; // default port
    }

    Log( &BotStates[botCount], "finished loading %s\n", filename );

    return 1;
}

// loads the plugin ( pluginPath ) in the bot state ( bot ).
int BotLoadPlugin( botState *bot, char* pluginName ) {
    
	int	pluginId = -1;
	for ( int i = 0; i < MAX_PLUGINS; i++ ) {
		if ( bot->pluginStates[i] == NULL ) {
			pluginId = i;
			break;
		}
	}

	if ( pluginId == -1 ) {
		printf( "can't load any more plugins\n" );
		return 0;
	}

	printf( "pluginID: %i\n", pluginId );

    lua_State *L = bot->pluginStates[pluginId] = lua_open();
   

   	assert( L != NULL );

    // create relative path for plugin
    char *pluginPath = alloca( strlen( PLUGIN_PATH ) + strlen( pluginName ) );
    sprintf( pluginPath, "%s%s", PLUGIN_PATH, pluginName );

    // copy plugin name to mem
    bot->pluginNames[pluginId] = malloc( strlen( pluginName ) + 1 );
    strcpy( bot->pluginNames[pluginId], pluginName );
    
    // load a new lua state
    luaL_openlibs( L );
    int loadStatus = luaL_loadfile( L, pluginPath );

    // some error checking
    switch( loadStatus ) {
        case LUA_ERRFILE:
            printf( "couldn't open %s.", pluginPath );
            return 0;
        case LUA_ERRMEM:
            printf( "memory error in %s.", pluginPath );
            return 0;
        case LUA_ERRSYNTAX:
            printf( "syntax error in %s.", pluginPath );
            lua_error( L );
            return 0;
        case 0: // success
            printf( "successfully loaded %s", pluginPath );
            break;
        default:
            printf( "unknown error %i in %s", loadStatus, pluginPath );
    }

    // this is the plugins ptr to the bot in which it sits in.
    lua_pushlightuserdata( L, bot );
    lua_setglobal( L, "botptr" );

    // let lua know about some functions
    BotGiveEvents( bot, pluginId );

    lua_pcall( L, 0, 0, 0 );
   
    OnLoad( bot, pluginId ); // let plugin know it's been loaded!
    
    bot->pluginCount++;

    return 1;
}

// disconnects bot, destroys lua instances and cleans up.
void BotUnload( botState *bot ) {

    // simply for debugging
    char* name = alloca( strlen( bot->nick ) + 1);
    strcpy( name, bot->nick );

    // unload plugins.
    for( int i = 0; i < bot->pluginCount; i++ ) {
        printf( "unloading %s\n", bot->pluginNames[i] );
        OnUnload( bot, i );
        lua_close( bot->pluginStates[i] );
        free( bot->pluginNames[i] );
    }
	
	// unload bot-wide lua state
	free( bot->globalState );

    free( bot->nick );
    free( bot->altnick );
    free( bot->fullname );
    free( bot->autosendcmd );
    free( bot->rawCarry );

    if ( bot->hostname == bot->server ) {
        free( bot->server );
    } else {
        free( bot->server );
        free( bot->hostname );
    }

	// free info grabbed from 005
	if ( bot->networkName ) free( bot->networkName );
	if ( bot->chanTypes ) free( bot->chanTypes );
	if ( bot->prefix ) free( bot->prefix );
	if ( bot->statusMsg ) free( bot->statusMsg );
	if ( bot->chanModes ) free( bot->chanModes );
	if ( bot->caseMapping ) free( bot->caseMapping );

    // free memory we use for dealing with individual lines
    for( int i = 0; i < PROTOCOL_FIELDS; i++ )
        free( bot->msg[i] );

    // the array
    free( bot->msg );

    Log( bot, "finished cleaning up\n" );

	fclose( bot->log );
	bot->pluginCount--;
    
	printf( "finished cleaning up bot [%s]\n", name );
}

