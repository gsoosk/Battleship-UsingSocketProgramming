all: Server

Server : Main.o Heartbeat.o Server.o
	gcc Main.o Heartbeat.o Server.o -o Server

Heartbeat.o : Heartbeat.c Server.h
	gcc -c Heartbeat.c

Main.o : Main.c Server.h
	gcc -c Main.c

Server.o : Server.c Server.h
	gcc -c Server.c

clean:
	rm *.o Server

