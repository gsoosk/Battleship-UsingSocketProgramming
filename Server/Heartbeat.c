#include "Server.h"
bool heartbeating = false;
int maxNumberOfClients = 10;

bool doesItHaveInputError(int argc, char* argv[])
{
    if(argc != 3){
        write(1, "You should use server in \"server x y\" syntax\n", 46);
        return true;
    }
    return false;
}

void initializeHeartbeatSocket(char* heartbeatPort)
{
    //Creating a socket using SOCK_DGRAM for UDP 
    if( (heartbeatSocketFD = socket( AF_INET , SOCK_DGRAM, 0)) < 0 )
    {
        write(1, "fail to open socket", 20);
        return ;
    }


    //Defining socket address
    addressOfHearbeat.sin_family = AF_INET; //ipv4 protocol i think
    addressOfHearbeat.sin_port = htons(atoi(heartbeatPort));
    addressOfHearbeat.sin_addr.s_addr = inet_addr(ADDR);
}

void sendingHeartbeat()
{   
    const char* msg = HEARTBEAT_MSG;
    if( sendto(heartbeatSocketFD, msg , strlen(msg), 0, (struct sockaddr*)&addressOfHearbeat, sizeof(addressOfHearbeat)) > -1)
        if(!heartbeating)
        {
            write(1, "Server is now heartbeating.\n", 29);
            heartbeating = true;
        } 

    signal(SIGALRM, sendingHeartbeat);
    alarm(1);
}
static char *itoa_simple_helper(char *dest, int i) {
  if (i <= -10) {
    dest = itoa_simple_helper(dest, i/10);
  }
  *dest++ = '0' - i%10;
  return dest;
}

char *itoa_simple(char *dest, int i) {
  char *s = dest;
  if (i < 0) {
    *s++ = '-';
  } else {
    i = -i;
  }
  *itoa_simple_helper(s, i) = '\0';
  return dest;
}