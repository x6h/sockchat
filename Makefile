cc = gcc
cflags = -std=gnu99 -Wall -Wextra -pedantic
objs = sockchat.o chat.o globals.o helpers.o
bin = sockchat

all: build

build: $(bin)

$(bin): $(objs)
	$(cc) $(cflags) $^ -o $(bin)

%.o: %.c
	$(cc) $(cflags) -c $^

clean:
	rm -f *.o $(bin)
