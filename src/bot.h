#ifndef BOT_H
#define BOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <alloca.h>
#include <netdb.h>

#include "def.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luafunc.h"

extern botState BotStates[MAX_BOTS];
extern int botCount;

int  BotConnect( botState *bot );
void BotGiveEvents( botState *bot, int pluginId );
void BotInfo( botState *bot );
int  BotLoad( lua_State *L );
int  BotLoadPlugin( botState *bot, char *pluginName );
void BotUnload( botState *bot );

#endif
