#ifndef CLIENT_H
#define CLIENT_H
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define MAXRECVSTRING 255  
#define ADDR "239.255.255.250"
#define CLIENT_ADDR "127.0.0.1"
#define TIMEOUT 4
#define BROADCAST_TIMEOUT 0.1 // about 5 sec
#define SERVER_CONNECT false
#define BROADCAST_CONNECT true
#define HEARTBEAT true
#define BROADCAST false


char* portForListening;
int broadcastFD;
int clientFD;
int infoTransferFD;
int gameSocketFD;

struct sockaddr_in clientAddress;
struct sockaddr_in serverAddress;
char name[128];
char* opponentName;
int map[10][10];

int broadcastingFD;
bool serverAvailability;
pid_t broadcastPID;
char dataToBroadcast[256];
struct sockaddr_in addressOfBroadcasting;
extern bool broadcasting ;

// IO and Tools :
void input(char* inputStr);
static char *itoa_simple_helper(char *dest, int i);
char *itoa_simple(char *dest, int i);
bool doesItHaveInputError(int argc, char* argv[]);
bool getOpponent(char* opponent);
void fillDataToSendStr(char* dataToSend, char* port, char* addr, char* opponent,  bool withOpponent);
void fillTime(char* _time);

// Broadcast send and recive :
void initializeBroadcastSocket(char* port, bool broadcastOrHeartbeat);
bool doesAnyoneWantToPlayWithNoRule(char* recvString, bool withOpponent, char* opponent);
bool doesAnyoneWantToPlayWithRule(char* recvString, bool withOpponent, char* opponent);
bool doesAnyoneWantToPlay(bool withOpponent, char* opponent);
void broadcastingInformaitions();
void initializingBroadcasting(char* port, char* name, char* opponent, bool withOpponent);


// Game :
void loadMap();
bool amILost();
void getXY(int *x, int *y, char destination[128]);
void fire();
void defense();
void game(bool turn, bool start);


// Client :
void fillServerInformation(char* info);
bool doesServerAlive();
bool tryToBindClientSocket();
void initializeClientSocketToListen();
struct sockaddr_in getAddressFromDetails(char* userData);
void connectToPlay(char* userData);
void acceptToPlay(bool broadcast);
void comiunicateWithServer();
void sendInformationToServer(char* name, char* opponent, bool withOpponent);

//Status :
void sendStatusToServer(char status[256]);
void handleNewStatus();
void saveStatusLocal(char status[256]);
void fillStatus(char status[256]);
void sendLocalStatusesToServer();
void clearStatusLocalFile();


#endif