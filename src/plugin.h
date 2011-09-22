#ifndef PLUGIN_H
#define PLUGIN_H

#include "def.h"

int  BotLoad( lua_State *L );
int  BotLoadPlugin( botState *bot, char *pluginName );
void BotUnload( botState *bot );

#endif
