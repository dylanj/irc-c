#include "def.h"
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
	Log( bot, "-> %s\n", buffer );
	
	va_end( args );
	return len;
}

// deal with irc protocol
int IRCParse( botState *bot, char* line ) {
	
	int returns =  1; // success.
	char *fromstr, *protostr, *tostr, *msgstr, *chanstr, *userstr;
	
	// makes this a lot easier to understand
	fromstr = bot->msg[0];
	protostr = bot->msg[1];
	chanstr = bot->msg[1]; // used in KICK
	userstr = bot->msg[1]; // used in KICK 
	tostr = bot->msg[2];
	msgstr = bot->msg[3];

	Log( bot, "<- %s\n", line );

	// clear mem for fields.
	for( int i = 0; i < PROTOCOL_FIELDS; i++ )
		memset( bot->msg[i], 0, SOCKET_BUF_LEN );
	
	// quickly check if ping
	if ( strncmp( line, "PING", 4 ) == 0 ) {
		sscanf( line, "PING :%s", msgstr );
		IRCSendPong( bot, msgstr );
		return returns;
	}
	
	// split message up
	sscanf(	line, ":%s %s %s :%[^\n]", fromstr, protostr, tostr, msgstr );
	
	// quick - if it's len 3 then it's a number, otherwise it's a string.
	int protoLength = strlen( protostr );
	if ( protoLength == 3 ) {
		int protoNum = atoi( protostr );
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
				printf( "important server info:\n%s\n", msgstr );
				break;
			case 375:
				printf( "motd start: %s\n", msgstr );
				break;
			case 372:
				printf( "motd: %s\n", msgstr );
				break;
			case 376:
				printf( "motd end: %s\n", msgstr );
				OnMOTDFinish( bot );
				//IRCJoinChannel( bot, "#C++" );
				break;
			default:
				printf( "unknown(%i) \"%s\"\n", protoNum, protostr );
		}
	}
	
	if ( strcmp( protostr, "PRIVMSG" ) == 0 ) {
		OnPrivateMessage( bot, fromstr, tostr, msgstr );
		
		if ( strcmp( msgstr, "!killbot" ) == 0 ) {
			printf( "asked to quit\n" );
			returns = -1;
		}
	} 
	else if ( strcmp( protostr, "NOTICE" ) == 0 ) {
		OnNotice( bot, fromstr, tostr, msgstr );
	}
	else if ( strcmp( protostr, "PING" ) == 0 ) {
		IRCSendPong( bot, msgstr );
	}
	else if ( strcmp( protostr, "JOIN" ) == 0 ) {
		OnJoin( bot, fromstr, tostr );
	}
	else if ( strcmp( protostr, "PART" ) == 0 ) {
		OnPart( bot, fromstr, tostr, msgstr );
	}
	else if ( strcmp( protostr, "INVITE" ) == 0 ) {
		OnInvite( bot, fromstr, tostr );
	}
	// from kick channel user :msg
	else if ( strcmp( protostr, "KICK" ) == 0 ) {
		chanstr = bot->msg[2]; // used in KICK
		userstr = bot->msg[3]; // used in KICK 
		sscanf( line, "%s %s %s %s :%[^\n]", fromstr, protostr, chanstr, userstr, msgstr );
		OnKick( bot, fromstr, chanstr, userstr, msgstr );
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
	
	// check for errors.
	if ( returns == -1 )
		return returns;

	while( 1 ) {
		memset( line, 0, SOCKET_BUF_LEN );
		sscanf( buftemp, "%[^\n]", line );
		 
		int lineLength = strlen( line );
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

