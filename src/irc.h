#ifndef IRC_H
#define IRC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <alloca.h>
#include <netdb.h>
#include <errno.h>

#include "def.h"
#include "events.h"

int IRCSendRaw( botState *bot, char *format, ... );
int IRCParse( botState *bot, char *line );
int IRCParseRaw( botState *bot, char *buffer ); 
void IRCHandleConnection( botState *bot );

void IRCJoinChannel( botState *bot, char *channel );
void IRCSendMessage( botState *bot, char *to, char *msg );
void IRCSetNick( botState *bot, char *nick );
void IRCSendUser( botState *bot );
void IRCSendPong( botState *bot, char *msg );
void IRCSendPing( botState *bot, char *msg );
void IRCSendMode( botState *bot, char *mode );

// USER username "username.com" "hostnames" :nick
void LoadGlobalConfig();

#endif
