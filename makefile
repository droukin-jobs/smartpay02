CC=gcc
CFLAGS=-c -Wall -DPACK -O
LDFLAGS=-lmicrohttpd
SRS=json.c url.c terminal.c main.c
OBJ=$(SRS:.c=.o)
EXE=sp_server

all: $(SRC) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

test:
	tests/test_scenarios.sh

clean : 
	$(RM) *.o sp_server
