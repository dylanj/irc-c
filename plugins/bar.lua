function OnPrivateMessage( user, msg ) 
	print( "BAR:PRIVMSG ", user, " :", msg );
end

function OnPing( msg )
	print( "ping'd ", msg );
end

print ("bar.lua loaded");
