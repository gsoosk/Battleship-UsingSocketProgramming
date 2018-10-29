#include "Client.h"


void handleNewStatus()
{
    char status[256];
    fillStatus(status);
    if(serverAvailability == true)
        sendStatusToServer(status);
    else
        saveStatusLocal(status);
}
void fillStatus(char status[256])
{
    char _time[128];
    fillTime(_time);
    strcpy(status, "~");
    strcat(status, name);
    strcat(status, " won ");
    strcat(status, opponentName);
    strcat(status, " at ");
    strcat(status, _time);
}
void saveStatusLocal(char status[256])
{
    int fd = open("status.txt", O_WRONLY | O_APPEND | O_CREAT);
    write(fd, status, strlen(status));
    write(fd, "\n", 1);
    close(fd);
}
void sendStatusToServer(char status[256])
{
    if( (infoTransferFD = socket( AF_INET , SOCK_STREAM, 0)) < 0 )
    {
        write(1, "fail to open socket", 20);
        return ;
    }
    if (connect(infoTransferFD, (struct sockaddr*)&serverAddress,
	            sizeof(serverAddress)) < 0) {
		write(1, "could not connect to server\n", 29);
        return ;
	}
    send(infoTransferFD, status,  strlen(status), 0);
    close(infoTransferFD);

}
void sendLocalStatusesToServer()
{

    int fd = open("status.txt", O_RDONLY);
    char buf[4096];
    read(fd, buf, 4096);
    
    char* token;
    token = strtok(buf, "\n");
    while(token != NULL)
    {
        char status[256];
        strcpy(status, token);
        sendStatusToServer(status);
        token = strtok(NULL, "\n");
    }
    clearStatusLocalFile();
    close(fd);
    
}
void clearStatusLocalFile()
{
    remove("status.txt");
    // int fd = open("status.txt", O_WRONLY | O_CREAT);
    // close(fd);
}