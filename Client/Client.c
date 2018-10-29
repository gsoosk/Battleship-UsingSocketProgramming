#include "Client.h"
void fillServerInformation(char* info)
{
    serverAddress.sin_family = AF_INET;
    char* address;
    int port;
    address = strtok(info, " ");
    port = atoi(strtok(NULL, " "));
    serverAddress.sin_addr.s_addr = inet_addr(address);
    serverAddress.sin_port = htons(port);
}
bool doesServerAlive()
{
    char recvString[MAXRECVSTRING+1];
    int recvStringLen;  
    if( (recvStringLen = recvfrom(broadcastFD, recvString, MAXRECVSTRING, 0, NULL , 0)) > 0)
    {
        recvString[recvStringLen] = '\0';
        write(1, "server information :", 21);
        write(1, recvString, strlen(recvString));
        write(1, "\n", 1);
        fillServerInformation(recvString);
        close(broadcastFD);
        sendLocalStatusesToServer();
        return true;
    }
    return false;
}


bool tryToBindClientSocket()
{
    clientAddress.sin_family = AF_INET;
    clientAddress.sin_addr.s_addr = inet_addr(CLIENT_ADDR);
    srand(8000);
    while(true)
    {
        int port = rand()%7000 + 1000;
        clientAddress.sin_port = htons(port); // auto assign a port
        if( bind(clientFD, (struct sockaddr *) &clientAddress , sizeof(clientAddress)) > -1) {
            break;
        }
    }
    return true;
}
void initializeClientSocketToListen()
{
    
    if( (clientFD = socket( AF_INET , SOCK_STREAM, 0)) < 0 )
    {
        write(1, "fail to open socket", 20);
        return ;
    }
    if(!tryToBindClientSocket())
    {
        write(1, "cannot bind", 12);
        return ;
    }
    if (listen(clientFD, 10) < 0)   
    {   
        write(1, "listen", 7);   
        exit(EXIT_FAILURE);   
    }  
    write(1, "know listening on ", 18);
    char* addr = inet_ntoa(clientAddress.sin_addr);
    char port[256]; itoa_simple(port, ntohs(clientAddress.sin_port));
    write(1, addr, strlen(addr));
    write(1, ":", 1);
    write(1, port, strlen(port));
    write(1, " for someone to connect!\n", 25);

}
struct sockaddr_in getAddressFromDetails(char* userData)
{
    struct sockaddr_in gameAddress;
    char data[128]; strcpy(data, userData);
    char *port;
    char *address;
    strtok(data, "$");
    address = strtok(NULL , "$");
    port = strtok(NULL , "$");
    gameAddress.sin_family = AF_INET;
    gameAddress.sin_addr.s_addr = inet_addr(address);
    gameAddress.sin_port  = htons(atoi(port));
    return gameAddress;
}

void connectToPlay(char* userData)
{
    struct sockaddr_in gameAddress;
    gameAddress = getAddressFromDetails(userData);
    if( (gameSocketFD = socket( AF_INET , SOCK_STREAM, 0)) < 0 )
    {
        write(1, "fail to open socket", 20);
        return ;
    }
    if (connect(gameSocketFD, (struct sockaddr*)&gameAddress, sizeof(gameAddress)) < 0) {
		write(1, "could not connect to server\n", 29);
        return ;
	}
    write(1, "Connected to other player ( ", 29);
    opponentName = strtok(userData, "$");
    write(1, opponentName, strlen(opponentName));
    write(1, " )\n", 4);
    send(gameSocketFD, name, 128, 0);
   
    game(false, true);
}
void acceptToPlay(bool broadcast)
{
    int connectionSocket;
    struct sockaddr_in cli; 
    int len = sizeof(cli);
    char buf[1025]; int numBytes;
    gameSocketFD = accept(clientFD, (struct sockaddr*)&cli, &len );
    if (gameSocketFD < 0) { 
        write(1, "server acccept failed...\n", 26); 
        return ;
    } 
    if(broadcast)
        kill(broadcastPID, SIGTERM);
    write(1, "\nYou acccepted the player ( ", 29);  
    numBytes = recv(gameSocketFD, buf, 1024, 0);
    buf[numBytes] = '\0';
    opponentName = strtok(buf, "$");
    write(1, opponentName, strlen(opponentName));
    write(1, " )\n", 4);
    int map[10][10]; 
    game(true, true);
}
void comiunicateWithServer()
{
    while(true)
    {
        int numbytes;
        char buf[1025];
        numbytes = recv(infoTransferFD, buf, 1024, 0);
        if(numbytes == 0)
        {
            close(infoTransferFD);
            break;
        }
        buf[numbytes] = '\0';

        if(buf[0] == '$') // server message    
        {
            write(1, buf, numbytes);
        } 
        else if(buf[0] == '*') // dc message 
        {
            write(1, buf, numbytes);
            close(infoTransferFD);
            acceptToPlay(SERVER_CONNECT);
            break ;
        }
        else
        {
            write(1, "*Someone finded to play!\n", 26);
            connectToPlay(buf);
            break ;
        }
    }
    
}

void sendInformationToServer(char* name, char* opponent, bool withOpponent)
{
    
    if( (infoTransferFD = socket( AF_INET , SOCK_STREAM, 0)) < 0 )
    {
        write (1, "fail to open socket", 20);
        return ;
    }
    if (connect(infoTransferFD, (struct sockaddr*)&serverAddress,
	            sizeof(serverAddress)) < 0) {
		write(1, "could not connect to server\n", 29);
        return ;
	}
    char dataToSend[256];
    char port[10];
    fillDataToSendStr(dataToSend, itoa_simple(port, ntohs(clientAddress.sin_port)), CLIENT_ADDR
            , opponent, withOpponent);
    send(infoTransferFD, dataToSend,  strlen(dataToSend), 0);
    comiunicateWithServer();
}
