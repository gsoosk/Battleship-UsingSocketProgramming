all : Client

Client : Main.o Tools.o Broadcast.o Game.o Client.o Status.o
		gcc Main.o Tools.o Broadcast.o Game.o Client.o Status.o -o Client



Main.o : Main.c Client.h
	gcc -c Main.c

Tools.o : Tools.c Client.h
	gcc -c Tools.c

Broadcast.o : Broadcast.c Client.h
	gcc -c Broadcast.c

Game.o : Game.c Client.h
	gcc -c Game.c

Client.o : Client.c Client.h
	gcc -c Client.c

Status.o : Status.c Client.h
	gcc -c Status.c

clean:
	rm *.o Client

