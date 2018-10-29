#include "Server.h"
void initializeServerSocket()
{
    int i;
    for(int i = 0 ; i < maxNumberOfClients ; i++)
    {
        clientSockets[i] = 0;
        connectReqeusts[i] = 0;
    }
    //Creating a socket using SOCK_DGRAM for UDP 
    if( (serverSocketFD = socket( AF_INET , SOCK_STREAM, 0)) < 0 )
    {
        write(1, "fail to open socket", 20);
        return ;
    }

    int opt = 1;   
    if( setsockopt(serverSocketFD, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,  
          sizeof(opt)) < 0 )   
    {   
        write(1, "setsockopt", 11);   
        exit(EXIT_FAILURE);   
    }   

    // socket address used for the server
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    
     // binding server 
    if( bind(serverSocketFD, (struct sockaddr *) &serverAddr , sizeof(serverAddr)) < 0) {
        write(1, "fail to bind", 13);
        return ;
    }

    if (listen(serverSocketFD, 10) < 0)   
    {   
        write(1, "listen", 6);   
        exit(EXIT_FAILURE);   
    }
}

void handleIncomingConnections()
{
    int newSocket;
    int addrlen;
    struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
    addrlen = sizeof(address);   
    if (FD_ISSET(serverSocketFD, &readFDs))   
    {   
        if ((newSocket= accept(serverSocketFD,  
                (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)   
        {   
            write(1, "accept", 7);   
            exit(EXIT_FAILURE);   
        }   
            
        //inform user of socket number - used in send and receive commands  
        char* addr = inet_ntoa(address.sin_addr);
        char port[256]; itoa_simple(port, ntohs(address.sin_port));
        write(1, "New Connection, address :", 26);
        write(1, addr, strlen(addr));
        write(1, ":", 1);   
        write(1, port, strlen(port));
        write(1, "\n", 1);   
                
        char* message = "$You connected to Server...\n";
        //send new connection greeting message  
        if( send(newSocket, message, strlen(message), 0) != strlen(message) )   
        {   
            write(1, "send", 5);   
        }       
        write(1, "Connected message sent to client. \n", 36);

        int i;
        //add new socket to array of sockets  
        for (i = 0; i < maxNumberOfClients; i++)   
        {   
            //if position is empty  
            if( clientSockets[i] == 0 )   
            {   
                clientSockets[i] = newSocket;   
                write(1, "Adding to list of sockets as ", 30);
                char msg[250]; itoa_simple(msg, i);
                write(1, msg, strlen(msg));
                write(1, "\n" , 1);   
                break;   
            }   
        }   
    } 
}
void closeSocket(int i)
{
    if(clientDatas[i][0] == '#')
    {
        char *name;
        strtok(clientDatas[i], "@");
        name = strtok(NULL, "@");
        name = strtok(name, "$");
        write(1, name, strlen(name));
    }
    else
    {
        char *name;
        name = strtok(clientDatas[i], "$");
        write(1, name, strlen(name));
    }
    write(1, " Disconnected!\n", 15);
    //Close the socket and mark as 0 in list for reuse  
    close( clientSockets[i] );   
    clientSockets[i] = 0;   
    strcpy(clientDatas[i], "");
    connectReqeusts[i] = 0;
    return ;
}
void matchSpecefic(int k)
{
    bool online = false;
    int onlineUserId, i;
    char willConnectMessage[1025] = "*You will connect to a player!\n";
    char waitMessage[1025] = "$This user is offline.Wait for user to connect.\n";
    for(i = 0 ; i < maxNumberOfClients ; i++)
    {
        if(clientSockets[i] == 0)
            continue;
        else
        {
            char name[128];
            strcpy(name , clientDatas[i]);
            strtok(name, "$");
            char opponent[128]; char* opponentName;
            strcpy(opponent, clientDatas[k]);
            opponentName = strtok(opponent, "#");
            opponentName = strtok(opponentName , "@");
            if(strcmp(opponentName, name) == 0)
            {
                send(clientSockets[k], clientDatas[i], strlen(clientDatas[i]), 0);
                send(clientSockets[i], willConnectMessage, strlen(willConnectMessage), 0);
                closeSocket(i);
                closeSocket(k);
                return ;
            }
        }
    }
    send(clientSockets[k], waitMessage, strlen(waitMessage), 0);
    connectReqeusts[k] = 1;
}
bool checkRequests(int k)
{
    char willConnectMessage[1025] = "*You will connect to a player!\n";
    for(int i = 0 ; i < maxNumberOfClients ; i++)
    {
        if(connectReqeusts[i] != 0)
        {
            char name[128];
            strcpy(name , clientDatas[k]);
            strtok(name, "$");
            char opponent[128]; char* opponentName; char* token;
            strcpy(opponent, clientDatas[i]);
            token = strtok(opponent, "#");
            opponentName = strtok(token , "@");
           
            if(strcmp(opponentName, name) == 0)
            {
                send(clientSockets[k], willConnectMessage, strlen(willConnectMessage), 0);
                send(clientSockets[i], clientDatas[k], strlen(clientDatas[k]), 0);
                closeSocket(i);
                closeSocket(k);
                return true;
            }
                
        }
    }
    return false;
}
void matchAnyone(int k)
{
    if(checkRequests(k))
        return  ;
    char waitMessage[1025] = "$Wait for someone to connect.\n";
    char willConnectMessage[1025] = "*You will connect to a player!\n";
    int i;
    for(i = 0 ; i < maxNumberOfClients ; i++)
    {
        if(clientSockets[i] == 0)
            continue;
        else if( connectReqeusts[i] == 0 && i != k)
        {
            send(clientSockets[k], clientDatas[i], strlen(clientDatas[i]), 0);
            send(clientSockets[i], willConnectMessage, strlen(willConnectMessage), 0);
            closeSocket(i);
            closeSocket(k);
            return ;
        }
    }
    send(clientSockets[k], waitMessage, strlen(waitMessage), 0);
}
void matchSomeone(int k)
{
    if(clientDatas[k][0] == '#')
        matchSpecefic(k);
    else
        matchAnyone(k);
    
}
void handleIncomingInformations()
{
    //else its some IO operation on some other socket 
    int i ;
    struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
    int addrlen = sizeof(address);   
    for (i = 0; i < maxNumberOfClients; i++)   
    {      
        if (FD_ISSET( clientSockets[i] , &readFDs))   
        {   
            //Check if it was for closing , and also read the  
            //incoming message  
            int valread;
            char buffer[1025];
            if ((valread = read( clientSockets[i] , buffer, 1024)) <= 0)   
            {  
                closeSocket(i); 
            }     
            else 
            {   
                buffer[valread] = '\0';   
                if(buffer[0] == '~')
                {
                    strcpy(clientDatas[i], "statusReporter$");
                    addNewStatus(buffer);
                }
                else
                {
                    strcpy(clientDatas[i], buffer);
                    matchSomeone(i);
                }  
            }   
        }   
    }   
}

void addNewStatus(char* status)
{
    int statusFD = open("status.txt",  O_APPEND| O_RDWR | O_CREAT);
    write(statusFD, status, strlen(status));
    write(statusFD, "\n", 1);
    close(statusFD);
}
void printStauts()
{
    int numBytes;
    char buf[1024];
    int statusFD = open("status.txt",  O_RDONLY);
    do
    {
        numBytes = read(statusFD, buf, 1023);
        buf[numBytes] = '\0';
        write(1, buf, strlen(buf));
    }while(numBytes >= 1023);
    close(statusFD);
}
void commandHandler()
{
    while(true)
    {
        char buf[1024];
        int numBytes;
        numBytes = read(0, buf, 1024);
        buf[numBytes - 1] = '\0';
        if(strcmp(buf, "status") == 0)
            printStauts();
        else
            write(1, "Wrong command!\n", 16);
    }
}