#ifndef BOTSTATE_H
#define BOTSTATE_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <stdarg.h>

#define DEBUG 1
#define BUFLEN 2048
#define MSGLEN 128
#define CONFIG_PATH "./configs/"
#define GLOBAL_CONFIG "global.lua"
#define PLUGIN_PATH "./plugins/"
#define DEFAULT_IRC_PORT 6667 
#define SOCKET_BUF_LEN 4096
#define PROTOCOL_FIELDS 5
#define MAX_PLUGINS 10
#define MAX_BOTS 5

typedef struct botstate_s {
	int  port;
	char *nick;				// nick namee
	char *altnick;				// nick name
	char *hostname;			// host
	char *fullname;			// full name of bot
	char *server;			// server to connect to
	char *autosendcmd;			// commands to send to server on connect.
	char *rawCarry;			// the contents of an incomplete irc message
	char **msg;				// memory to store parsed info from messages
	FILE *log;
	int   socket;			 	// server socket
	int   pluginCount;

	lua_State *pluginStates[MAX_PLUGINS];	// lua states.
} botState;

void Log( botState *bot, char *format, ... ); 

#endif
