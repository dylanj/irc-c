#ifndef EVENTS_H
#define EVENTS_H

#include "def.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

// irc events
void OnInvite( botState *bot, char *from, char *channel );
void OnJoin( botState *Bot, char *user, char *channel );
void OnKick( botState *bot, char *from, char* channel, char *user, char* msg );
void OnMOTDFinish( botState *bot );
void OnNotice( botState *bot, char *from, char *to, char *msg );
void OnPart( botState *bot, char *user, char *channel, char* msg );
void OnPing( botState *bot, char *msg );
void OnPrivateMessage( botState *bot, char *from, char *to, char *msg );
void OnQuit( botState *bot, char *from, char *user, char* msg );

// core events
void OnLoad( botState *bot, int plugin );
void OnUnload( botState *bot, int plugin );
	
#endif
