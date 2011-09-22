#ifndef BOTSTATE_H
#define BOTSTATE_H

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include <stdarg.h>
#include <assert.h>

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
	// irc info
    int   port;				// port to connect to
    char *nick;             // nick name
    char *altnick;          // alt nick name
    char *hostname;         // hostname of server
    char *fullname;         // full name of bot
    char *server;           // server string to connect to
    char *autosendcmd;      // commands to send to server on connect.

	// storage
    char  *rawCarry;         // the contents of an incomplete irc message
    char **msg;             // memory to store parsed info from messages
	
	char *currentNick;

	// important info from server
	int maxnicklen;
	int awaylen;
	int topiclength;
	int kicklen;
	int channellen;
	int maxchannellen;

	char *networkName;
	char *chanTypes;
	char *prefix;
	char *statusMsg;
	char *chanModes;
	char *caseMapping;

	// system info
    FILE *log;
    int   socket;           // server socket
    
	// plugin variables
	int   	   pluginCount;
	lua_State *globalState; // used to store channel and user info
    lua_State *pluginStates[MAX_PLUGINS];   // lua states.
    char      *pluginNames[MAX_PLUGINS];
} botState;

void Log( botState *bot, char *format, ... ); 

#endif
