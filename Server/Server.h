#ifndef SERVER_H
#define SERVER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

// #include "Globals.h"

#define HEARTBEAT_MSG "127.0.0.1 8000"
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 8000
#define ADDR "239.255.255.250"

struct sockaddr_in addressOfHearbeat;
int heartbeatSocketFD;
extern bool heartbeating;
int serverSocketFD;
int clientSockets[10];
extern int maxNumberOfClients;
int connectReqeusts[10];
char clientDatas[10][1025];
fd_set readFDs;  //set of socket descriptors  
int activity; //using to store select

// IO functions :
bool doesItHaveInputError(int argc, char* argv[]);
void commandHandler();
static char *itoa_simple_helper(char *dest, int i);
char *itoa_simple(char *dest, int i);
// Hearbeat functions :
void initializeHeartbeatSocket(char* heartbeatPort);
void sendingHeartbeat();
// Server functions :
void initializeServerSocket();
void handleIncomingConnections();
void closeSocket(int i);
void handleIncomingInformations();
void addNewStatus(char* status);
void printStatus();
// Matching functions :
void matchSpecefic(int k);
bool checkRequests(int k);
void matchAnyone(int k);
void matchSomeone(int k);
#endif
