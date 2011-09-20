#include <stdio.h>
#include <stdlib.h>

#include "irc.h"
#include "bot.h"

int main( int argc, char** argv ) {
    
    LoadGlobalConfig();

    if ( BotConnect ( &BotStates[0] ) ) {
        // init
        IRCSetNick( &BotStates[0], NULL );
        IRCSendUser( &BotStates[0] );

        IRCHandleConnection( &BotStates[0] );
    }
    
    BotUnload( &BotStates[0] ); //only one bot loaded so far.

    return EXIT_SUCCESS;
}

