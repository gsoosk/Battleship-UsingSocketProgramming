#include "Client.h"



int main(int argc, char *argv[])
{
    if(doesItHaveInputError(argc, argv))
        return 0;
        
    
    write(1, "Please enter your username to login\n", 37); 
    input(name);
    write(1, "Do you want to play with other user?\n", 38);
    char opponent[128]; bool withOpponent = false;
    withOpponent = getOpponent(opponent);
    loadMap(map);

    initializeBroadcastSocket(argv[1], HEARTBEAT); 
    if(serverAvailability = doesServerAlive())
    {
        write(1, "Server is alive!\n", 18);
        initializeClientSocketToListen();
        sendInformationToServer(name, opponent, withOpponent);
    }
    else
    {
        write(1, "Server is not alive.\n", 22);
        initializeBroadcastSocket(argv[2], BROADCAST);
        if(!doesAnyoneWantToPlay(withOpponent, opponent))
        {
            initializeClientSocketToListen();
            broadcastPID = fork();
            if(broadcastPID == 0)
            {
                initializingBroadcasting(argv[2], name, opponent, withOpponent);
                broadcastingInformaitions();
                while(true){};
            }
            else
                acceptToPlay(BROADCAST_CONNECT);
        }
    }

    close(gameSocketFD);
    close(clientFD);
    return 1;
}