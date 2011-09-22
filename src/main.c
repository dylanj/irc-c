#include <stdio.h>
#include <stdlib.h>

#include "irc.h"
#include "bot.h"
#include "list.h"

int strcnt( char *str, char ch ) {
	char *strc = str;
	int i = 0;
	while( *strc++ != NULL ) 
		if ( *strc == ch )
			i++;

	return i;
}

char **strsplit( char *str, char delim ) {
	
	char *splitstr = alloca( strlen( str ) + 1 );
	char *individualstr;
	char **split; // result.
	int pos = 0;

	split = malloc( ( strcnt( str, delim ) + 1 ) * sizeof( char * ) );
	
	strcpy( splitstr, str );

	individualstr = strtok( splitstr, " " );
	
	while( individualstr != NULL ) {
		printf( "str: %s\n", individualstr );

		split[pos] = malloc( strlen( individualstr ) + 1 );
		strcpy(	split[pos++], individualstr );

		individualstr = strtok( NULL, " ," );
	}

	return split;
}

int main( void ) {
	
	char *teststr = "test";
	list *templist = ListCreate( teststr );
	
	for( int i = 0; i < 26; i++ ) {
		char *test = alloca( 25 );;
		sprintf( test, "listtest: %i ~_~", i );
		ListInsertAtEnd( templist, test );
	}

	list *sorted = ListSortStrings( templist );
	ListFree( templist );


	if ( ListContains( sorted, teststr ) ) {
		printf( "string found\n" );
	}

	ListPrint( sorted );
	ListFree( sorted );
		
	return 1;
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

