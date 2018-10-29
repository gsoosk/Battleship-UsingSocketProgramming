#include "Client.h"

void input(char* inputStr)
{
    int numBytes;
    do 
    {
        numBytes = read(0, inputStr, 128);
        inputStr[numBytes - 1] = '\0';
    }while(numBytes < 2);
    
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
bool doesItHaveInputError(int argc, char* argv[])
{
    if(argc != 3){
        write(1, "You should use client in \"client x y\" syntax\n", 46);
        return true;
    }
    return false;
}
bool getOpponent(char* opponent)
{
    char answer[128];
    do 
    {
        if(strcmp(answer, "yes") == 0)
        {
            write(1, "what is username of opponent?\n", 31);
            input(opponent);
            return true;
        }
        else if(strcmp(answer, "no") == 0)
        {
            return false;
        }
        write(1, "please enter \"yes\" or \"no\".\n", 29);
        input(answer);
    } 
    while (true);
    return false;
}
void fillDataToSendStr(char* dataToSend, char* port, char* addr, char* opponent,  bool withOpponent)
{
    char _addr[10], _port[10];
    if(withOpponent)
    {
        strcpy(dataToSend, "#");
        strcat(dataToSend, opponent);
        strcat(dataToSend, "@");
    }
    else
    strcpy(dataToSend, "");
    strcpy(_addr, addr);
    strcpy(_port, port);
    strcat(dataToSend, name);
    strcat(dataToSend, "$");
    strcat(dataToSend, _addr);
    strcat(dataToSend, "$");
    strcat(dataToSend, _port);
}
void fillTime(char* _time)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char temp[10];
    strcpy(_time, "");
    strcat(_time, itoa_simple(temp, tm.tm_year + 1900));
    strcat(_time, "/");
    strcat(_time, itoa_simple(temp, tm.tm_mon));
    strcat(_time, "/");
    strcat(_time, itoa_simple(temp, tm.tm_mday));
    strcat(_time, " ");
    strcat(_time, itoa_simple(temp, tm.tm_hour));
    strcat(_time, ":");
    strcat(_time, itoa_simple(temp, tm.tm_min));        
}