.SUFFIXES:
.SUFFIXES: .c .o

CC=gcc
PROGRAM=p3q
CPPFLAGS=-Wall -std=c99
LDLIBS=-pthread -llua
OBJDIR=obj
OUTDIR=bin

vpath %.c src
vpath %.h src

objects = $(addprefix $(OBJDIR)/, main.o bot.o events.o irc.o luafunc.o)
headers = $(bot.h events.h irc.h luafunc.h)

ku: $(objects)
	$(CC) $(CPPFLAGS) -g -o $(OUTDIR)/$(PROGRAM) $(objects) $(LDLIBS)

obj/%.o : %.c $(headers)
	$(CC) $(CPPFLAGS) -c -g -o $@ $<

