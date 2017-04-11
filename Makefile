CC = g++
CFLAGS = -W -Wall -O3
LDFLAGS = -lSDL2main -lSDL2 -lstdc++
EXEC = gameOfLife

all: $(EXEC)

$(EXEC): $(EXEC).cpp
	$(CC) $^ $(CFLAGS) -o $(EXEC) $(LDFLAGS)