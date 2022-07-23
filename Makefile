# compiler
cc = gcc
# compiler flags
cflags = -std=gnu99 -Wall -Wextra -pedantic
# object files to build
objs = sockchat.o chat.o globals.o helpers.o
# output binary name
bin = sockchat
# installation directory
installdir = /usr/local/bin

all: build

build: $(bin)

$(bin): $(objs)
	$(cc) $(cflags) $^ -o $@

%.o: %.c
	$(cc) $(cflags) -c $^

install: $(bin)
	cp -f $^ $(installdir)

uninstall: $(bin)
	rm -f $(installdir)/$(bin)

clean:
	rm -f $(objs) $(bin)

.PHONY: all build clean
