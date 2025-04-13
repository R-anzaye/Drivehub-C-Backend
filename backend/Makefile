CC = gcc
CFLAGS = -I./mongoose -I./src -Wall -g
LDFLAGS = -ljansson -lsqlite3 -pthread

all: backend

backend: src/server.o src/routes.o src/database.o mongoose/mongoose.o
	$(CC) -o backend src/server.o src/routes.o src/database.o mongoose/mongoose.o $(LDFLAGS)

src/server.o: src/server.c src/app.h
	$(CC) $(CFLAGS) -c src/server.c -o src/server.o

src/routes.o: src/routes.c src/app.h
	$(CC) $(CFLAGS) -c src/routes.c -o src/routes.o

src/database.o: src/database.c src/app.h
	$(CC) $(CFLAGS) -c src/database.c -o src/database.o

mongoose/mongoose.o: mongoose/mongoose.c mongoose/mongoose.h
	$(CC) $(CFLAGS) -c mongoose/mongoose.c -o mongoose/mongoose.o

clean:
	rm -f src/*.o mongoose/*.o backend
	

.PHONY: all clean