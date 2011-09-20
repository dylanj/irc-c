-- server = "irc.freenode.org:6667"
server = "irc.nuclearfallout.net"
nick = "p3q-test"
altnick = "p3q-test`"
fullname = "a lua-able irc bot written in c"

plugins = {
	"foo.lua",
	"bar.lua"
}

autosendcmd = "/msg nickserv identify hallo"

print( "loaded freenode.lua bot config\n" )
