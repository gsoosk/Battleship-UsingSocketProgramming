#include "Client.h"
bool broadcasting = false;

void initializeBroadcastSocket(char* port, bool broadcastOrHeartbeat)
{
    struct sockaddr_in addressOfSocket;
    char recvString[MAXRECVSTRING+1];
    int recvStringLen;    
    //Creating a socket using SOCK_DGRAM for UDP 
    if( (broadcastFD = socket( AF_INET , SOCK_DGRAM, 0)) < 0 )
    {
        write(2, "fail to open socket\n", 21);
        return ;
    }
    int broadcast = 1;
    if (setsockopt(broadcastFD, SOL_SOCKET, SO_REUSEADDR, &broadcast,sizeof (broadcast)) == -1) {
        write(2, "fail to set broadcast\n", 23);
        return ;
    }

    //Defining socket address
    addressOfSocket.sin_family = AF_INET; //ipv4 protocol i think
    addressOfSocket.sin_port = htons(atoi(port));
    addressOfSocket.sin_addr.s_addr = htonl(INADDR_ANY);

    // binding server 
    if( bind(broadcastFD, (struct sockaddr *) &addressOfSocket , sizeof(addressOfSocket)) < 0) {
        write(2, "fail to bind", 13);
        return ;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(broadcastFD, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0){
        write(2, "setsockopt", 11);
        return ;
    }
    float timeout = broadcastOrHeartbeat == HEARTBEAT ? TIMEOUT : BROADCAST_TIMEOUT ;
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = timeout * 1000;
    if (setsockopt(broadcastFD, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
        write(1, "unable to set timeout", 22);
    }
}
bool doesAnyoneWantToPlayWithNoRule(char* recvString, bool withOpponent, char* opponent)
{
    if(recvString[0] != '#')
    {
        if(withOpponent == false)
        {
            close(broadcastFD);
            connectToPlay(recvString);
            return true;
        }else if (withOpponent == true)
        {
            char details[128];
            strcpy(details, recvString);
            char* broadcasterName;
            broadcasterName = strtok(details, "$");
            if(strcmp(broadcasterName, opponent) == 0)
            {
                close(broadcastFD);
                connectToPlay(recvString);
                return true;
            }
        }
    }
    return false;
}
bool doesAnyoneWantToPlayWithRule(char* recvString, bool withOpponent,char* opponent)
{
    if(recvString[0] == '#')
    {
        char details[128];
        opponentName = strtok(recvString, "#");
        opponentName = strtok(opponentName , "@");
        strcpy(details, strtok(NULL, "@"));
        if(strcmp(name, opponentName) == 0)
        {
            if(withOpponent)
            {
                char* broadcasterName;
                char detailsCopy[128] ; strcpy(detailsCopy, details);
                broadcasterName = strtok(detailsCopy, "$");
                if(strcmp(broadcasterName, opponent) == 0)
                {
                    close(broadcastFD);
                    connectToPlay(details);
                    return true;
                }
            }
            else
            {
                close(broadcastFD);
                connectToPlay(details);
                return true;
            }
        }     
    }
    return false;
}
bool doesAnyoneWantToPlay(bool withOpponent, char * opponent)
{
    int i ;
    write(1, "Search for others ...\n", 23);
    for(i = 1 ; i < 400 ; i++)
    {    
        char recvString[1025];
        int recvStringLen;  
        if( (recvStringLen = recvfrom(broadcastFD, recvString, MAXRECVSTRING, 0, NULL , 0)) > 0)
        {
            recvString[recvStringLen] = '\0';
            
            if(doesAnyoneWantToPlayWithNoRule(recvString, withOpponent, opponent))
                return true;
            if(doesAnyoneWantToPlayWithRule(recvString, withOpponent, opponent))
                return true;

        }
    }
    return false;
}
void broadcastingInformaitions()
{
    if( sendto(broadcastingFD, dataToBroadcast , strlen(dataToBroadcast), 0,
     (struct sockaddr*)&addressOfBroadcasting, sizeof(addressOfBroadcasting)) > -1)
        if(!broadcasting)
        {
            write(1, "Broadcasting.", 14);
            broadcasting = true;
        }   
        else{
            write(1, ".", 1);}

    signal(SIGALRM, broadcastingInformaitions);
    alarm(1);
}
void initializingBroadcasting(char* port, char* name, char* opponent, bool withOpponent)
{
     //Creating a socket using SOCK_DGRAM for UDP 
    if( (broadcastingFD = socket( AF_INET , SOCK_DGRAM, 0)) < 0 )
    {
        write(1, "fail to open socket", 20);
        return ;
    }
    //Defining socket address
    addressOfBroadcasting.sin_family = AF_INET; //ipv4 protocol i think
    addressOfBroadcasting.sin_port = htons(atoi(port));
    addressOfBroadcasting.sin_addr.s_addr = inet_addr(ADDR);

    char _port[10];
    fillDataToSendStr(dataToBroadcast, itoa_simple(_port, ntohs(clientAddress.sin_port)), CLIENT_ADDR
            , opponent, withOpponent);
   
}