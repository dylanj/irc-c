#include "irc.h"
#include "bot.h"

// log message to file
void Log( botState *bot, char *format, ... ) {
	va_list args;
	char buffer[SOCKET_BUF_LEN*2];

  	va_start( args, format );
	vsprintf( buffer, format, args );
	fprintf( bot->log, "%s", buffer );
	va_end( args );
}

void LoadGlobalConfig() {
	lua_State *global;
	global = lua_open();
	luaL_openlibs( global );
	
	char *globalConfigPath = alloca( strlen( CONFIG_PATH ) + strlen( GLOBAL_CONFIG ) );
	sprintf( globalConfigPath, "%s%s", CONFIG_PATH, GLOBAL_CONFIG );

	if ( luaL_loadfile( global, globalConfigPath ) == LUA_ERRFILE ) {
		printf( "Couldn't open global config at (%s).\n", globalConfigPath );
		return;
	}

	lua_register( global, "LoadBot", BotLoad );	
	lua_pcall( global, 0, 0, 0 ); // parse file

	#if DEBUG
	printf( "successfully loaded global.lua\n" );
	#endif

	lua_close( global );
}

// send buffer to network.
int IRCSendRaw( botState *bot, char* format, ... ) {
	va_list args;
	int len; // send() returns this
	char buffer[SOCKET_BUF_LEN];

  	va_start( args, format );
	vsprintf( buffer, format, args );
	
	len = send( bot->socket, buffer, strlen( buffer ), 0 );
	Log( bot, "-> %s\n", len, buffer );
	
	va_end( args );
	return len;
}

// deal with irc protocol
int IRCParse( botState *bot, char* line ) {
	
	//printf( "%s\n", line );
	Log( bot, "<- %s\n", line );
	int returns =  1; // success.

	// clear mem for fields.
	for( int i = 0; i < PROTOCOL_FIELDS; i++ )
		memset( bot->msg[i], 0, SOCKET_BUF_LEN );
	
	if ( strncmp( line, "PING", 4 ) == 0 ) {
		sscanf( line, "PING :%s", bot->msg[0] );
		IRCSendPong( bot, bot->msg[0] );
		return returns;
	}
	
	// split message up
	sscanf(	line, ":%s %s %s :%[^\n]", bot->msg[0], bot->msg[1], bot->msg[2], bot->msg[3] );
	
	// quick - if it's len 3 then it's a number, otherwise it's a string.
	int protoLength = strlen( bot->msg[1] );
	if ( protoLength == 3 ) {
		int protoNum = atoi( bot->msg[1] );
		switch( protoNum ) {
			case 1:
				IRCSendMode( bot, "+i" );
				break;
			case 2:
			case 3:
			case 4:
				printf( "really dont care about this stuff\n" );
				break;
			case 5:
				printf( "important server info:\n%s\n", bot->msg[3] );
				break;
			case 375:
				printf( "motd start: %s\n", bot->msg[3] );
				break;
			case 372:
				printf( "motd: %s\n", bot->msg[3] );
				break;
			case 376:
				printf( "motd end: %s\n", bot->msg[3] );
				OnMOTDFinish( bot );
				//IRCJoinChannel( bot, "#C++" );
				break;
			default:
				printf( "unknown(%i) \"%s\"\n", protoNum, bot->msg[1] );
		}
	}
	
	if ( strcmp( bot->msg[1], "PRIVMSG" ) == 0 ) {
		//printf( "private message from %s\nto: %s\nmsg: %s\n", bot->msg[0], bot->msg[2], bot->msg[3] );
		
		OnPrivateMessage( bot, bot->msg[0], bot->msg[2], bot->msg[3] );
		
		if ( strcmp( "!killbot", bot->msg[3] ) == 0 ) {
			printf( "asked to quit\n" );
			returns = -1;
		}

	} 
	else if ( strcmp( bot->msg[1], "NOTICE" ) == 0 ) {
		OnNotice( bot, bot->msg[0], bot->msg[2], bot->msg[3] );
	}
	else if ( strcmp( bot->msg[1], "PING" ) == 0 ) {
		IRCSendPong( bot, bot->msg[3] );
	}
	else if ( strcmp( bot->msg[1], "JOIN" ) == 0 ) {
		OnJoin( bot, bot->msg[0], bot->msg[2] );
	}
	else if ( strcmp( bot->msg[1], "PART" ) == 0 ) {
		OnPart( bot, bot->msg[0], bot->msg[2], bot->msg[3] );
	}
	else if ( strcmp( bot->msg[1], "INVITE" ) == 0 ) {
		OnInvite( bot, bot->msg[0], bot->msg[2] );
	}
	// from kick channel user :msg
	else if ( strcmp( bot->msg[1], "KICK" ) == 0 ) {
		sscanf( line, "%s %s %s %s :%[^\n]", bot->msg[0], bot->msg[1], bot->msg[2], bot->msg[3], bot->msg[4] );
		OnKick( bot, bot->msg[0], bot->msg[2], bot->msg[3], bot->msg[4] );
	}
	
	return returns;
}

// deals with buffer directly from recv. if a message is cut off it
// saves the portion that was sent to bot->rawCarry and deals with
// it on the next call of this function.
int IRCParseRaw( botState *bot, char* buffer ) {
	char line[SOCKET_BUF_LEN];
	int returns = 1; //success
	char* buftemp;
	int lnum = 0;
	buftemp = buffer;

	// deal with left over message
	if ( strlen( bot->rawCarry ) > 0 ) {
		char splicedMessage[SOCKET_BUF_LEN];
	
		memset( line, 0, SOCKET_BUF_LEN );
		sscanf( buftemp, "%[^\n]", line );

		int lineLength = strlen( line );

		if ( lineLength > 0 ) {
			strcpy( splicedMessage, bot->rawCarry );
			strcat( splicedMessage, line ); 
			buftemp += lineLength + 1; // +2 (\r\n)
			returns = IRCParse( bot, splicedMessage );
		}

		memset( bot->rawCarry, 0, SOCKET_BUF_LEN );
	}

	if ( returns == -1 ) {
		return returns;
	}


	while( 1 ) {
		lnum++;
		memset( line, 0, SOCKET_BUF_LEN );
		//printf( "0x%x - 0x%x\n", buffer, buftemp );
		sscanf( buftemp, "%[^\n]", line );
		 
		int lineLength = strlen( line );
		// if line doesn't end with an \r than the message was not received
		// in full. save the str and parse it and next time this function
		// is called glue it to the the front of the buffer.
		//printf( "linelenth: %i", lineLength );

		if ( lineLength == 0 )
			break;
		
		// line not complete. save to rawCarry
		if ( line[lineLength-1] != '\r' ) {
			memset( bot->rawCarry, 0, SOCKET_BUF_LEN );
			strcpy( bot->rawCarry, line );
			break;
		} else {
			// get rid of carriage return in message
			line[lineLength-1] = '\0';
		}
		
		// move position to read to next line.
		buftemp += lineLength + 1; // +1 for \n

		returns = IRCParse( bot, line );
	}

	return returns;
}

// listens for messages sent from server.
void IRCHandleConnection( botState *bot ) {
	char buffer[SOCKET_BUF_LEN];
	int recv_length;
	
	while( 1 ) {
		memset( buffer, 0, SOCKET_BUF_LEN ); // zero out buffer
		recv_length = recv( bot->socket, buffer, SOCKET_BUF_LEN, 0 );
		//Log( "<- %s", buffer );
		
		if ( recv_length == 0 ) 
			break;

		if ( IRCParseRaw( bot, buffer ) == -1 )
			break;
	}
	
}

// join the channel
void IRCJoinChannel( botState *bot, char* channel ) {
	if ( channel != NULL )
		IRCSendRaw( bot, "JOIN %s\n\n", channel );
}

// change nickname
void IRCSetNick( botState *bot, char* nick ) {
	if ( nick == NULL || strlen( nick ) < 2 )
		nick = bot->nick;
	IRCSendRaw( bot, "NICK %s\r\n", nick );
}

// send user info ( used on connect )
void IRCSendUser( botState *bot ) {
	IRCSendRaw( bot, "USER %s %s %s :%s\r\n", bot->nick, bot->nick, bot->hostname, bot->fullname ); 
}

void IRCSendPong( botState *bot, char* pong ) {
	IRCSendRaw( bot, "PONG :%s\r\n", pong );
}

void IRCSendPing( botState *bot, char* ping ) {
	IRCSendRaw( bot, "PING :%s\r\n", ping );
}

void IRCSendMode( botState *bot, char *mode ) {
	IRCSendRaw( bot, "MODE %s %s\r\n", bot->nick, mode );
}

void IRCSendMessage( botState *bot, char* to, char* msg ) {
	IRCSendRaw( bot, "PRIVMSG %s :%s\r\n", to, msg );
}

