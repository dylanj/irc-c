function string.explode ( str , seperator , plain )
	assert ( type ( seperator ) == "string" and seperator ~= "" , "Invalid seperator (need string of length >= 1)" )
 
	local t , nexti = { } , 1
	local pos = 1
	while true do
		local st , sp = str:find ( seperator , pos , plain )
		if not st then break end -- No more seperators found
 
		if pos ~= st then
			t [ nexti ] = str:sub ( pos , st - 1 ) -- Attach chars left of current divider
			nexti = nexti + 1
		end
		pos = sp + 1 -- Jump past current divider
	end
	t [ nexti ] = str:sub ( pos ) -- Attach chars right of last divider
	return t
end

function OnPrivateMessage( from, to, msg ) 
	print( "message from " .. from .. " to " .. to .. " -> " .. msg );
	
	command = string.explode( msg, " " );
	print( "prvmsg first arg: (" .. command[1] .. ")\n" );

	if command[1] == "!join" then
		print( "joining channel" );
		JoinChannel( command[2] );

	elseif command[1] == "!say" then
		print( "say message!" );	
		out = string.sub( msg, string.len( command[1] ) 
		    + string.len( command[2] ) + 3, string.len( msg ) );
		print( "say " .. out );
		SendMessage( command[2], out );

	elseif command[1] == "!nick" then
		SetNick( command[2] );	
	end
end

function OnPing( msg )
	print( "ping'd ", msg );
end

print( "foo.lua loaded" );
