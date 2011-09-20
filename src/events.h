#ifndef EVENTS_H
#define EVENTS_H

#include "def.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void OnPrivateMessage( botState *bot, char *from, char *to, char *msg );
void OnNotice( botState *bot, char *from, char *to, char *msg );
void OnPing( botState *bot, char *msg );
void OnJoin( botState *Bot, char *user, char *channel );
void OnPart( botState *bot, char *user, char *channel, char* msg );
void OnQuit( botState *bot, char *from, char *user, char* msg );
void OnInvite( botState *bot, char *from, char *channel );
void OnKick( botState *bot, char *from, char* channel, char *user, char* msg );
void OnMOTDFinish( botState *bot );
	
#endif
