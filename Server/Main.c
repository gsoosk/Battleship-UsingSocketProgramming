#include "Server.h"


int main(int argc, char *argv[])
{
    if(doesItHaveInputError(argc, argv))
        return 0;

    int newProccess = fork();
   
    if (newProccess > 0){
        newProccess = fork();
        if(newProccess > 0)
        {
            initializeHeartbeatSocket(argv[1]);
            sendingHeartbeat();
        }
        else{
            commandHandler();
            return 1;
        }
       
    }
    else {

    initializeServerSocket();

    while(true){
        FD_ZERO(&readFDs); // clear client file descriptors
        FD_SET(serverSocketFD, &readFDs); // add server descriptor to our set
        int i;

        int highestFileDescriptor = maxNumberOfClients;
        for ( i = 0 ; i < maxNumberOfClients ; i++)   
        {             
            //if valid socket descriptor then add to read list  
            if(clientSockets[i] > 0)
                FD_SET( clientSockets[i], &readFDs); 

            //highest file descriptor number, need it for the select function  
            if(clientSockets[i] > highestFileDescriptor)   
                highestFileDescriptor = clientSockets[i];   
                
        }   
        // we use NULL for write fds and exceptfd because we dont need it 
        // also we use NULL for time out because we want to wait for all!
        activity = select( highestFileDescriptor+1, &readFDs, NULL , NULL , NULL);
        if ((activity < 0)  &&  (errno!=EINTR))
        {   
            write(1, "select error", 13);   
        }   

        handleIncomingConnections();
        handleIncomingInformations();
    }
    }
    while(1){};
    return 1;
}
