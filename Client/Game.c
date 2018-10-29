#include "Client.h"
void loadMap()
{
    char filePath[256];
    write(1, "Please enter your mapFile path. \n", 34);
    int numBytes;
    numBytes = read(1, filePath, 256); filePath[numBytes-1] = '\0';
    int fd;
    char content[200];
    fd = open(filePath, O_RDONLY);
    if(fd < 0 )
    {
        write(1, "err", 3);
        return ;
    }   
    read(fd, content, 200);
    int i;
    for(i = 0 ; i < 200 ; i++)
    {
        if(i%2 == 0)
        {
            switch(content[i])
            {
                case '1':
                    map[ i / 20 ][(i % 20)/2] = 1;
                    break;
                case '0':
                    map[ i / 20 ][(i % 20)/2] = 0;
                    break;
            }
        }
    }
    write(1, "\nThis is your map!\n", 20);
    write(1, content, 200);
    write(1, "\n", 1);
}
void game(bool turn, bool start)
{
    if(!turn)
        defense();
    if(turn)
        fire();
}
bool amILost()
{
    int j, i;
    for(i = 0 ; i < 10 ; i++)
    {
         for(j = 0 ; j < 10 ; j++)
         {
             if(map[i][j] == 1)
               return false;
         }
    }
    return true;
}
void getXY(int *x, int *y, char destination[128])
{
    char *X; char*Y;
    X = strtok(destination, " ");
    Y = strtok(NULL, " ");
    *x = atoi(X); *y = atoi(Y);
}
void fire() // It's a kind of server in each turn
{
    char result[128];
    write(1, "Enter destination point in form \"X Y\". ( X < 10 and Y < 10) \n", 62);
    int numBytes; char destination[128];
    numBytes = read(1, destination, 128); destination[numBytes - 1] = '\0';
    send(gameSocketFD, destination, 128, 0);
    recv(gameSocketFD, result, 128, 0);
    if(strcmp(result, "duplicate") == 0)
    {
        write(1, "You choosed a duplicate block! Please try agian.\n", 50);
        game(true, false);
    }
    else if(strcmp(result, "success") == 0)
    {
        write(1, "You successfully hit the ship!\n", 32);
        game(true, false);
    }
    else if(strcmp(result, "fail") == 0)
    {
        write(1, "Unsuccessfull shoot!\n", 22);
        game(false, false);
    }
    else
    {
        write(1, "Congratulation you win this game!\n", 35);
        handleNewStatus();
        return;
    }
}

void defense() // It's a kind of client in each turn
{
    write(1, "Wait for ", 10);
    write(1, opponentName, strlen(opponentName));
    write(1, " to shoot ... \n", 16);

    char destination[128];
    recv(gameSocketFD, destination, 128, 0);
    write(1, "shoot on ", 10);
    write(1, destination, strlen(destination));
    write(1, ".\n", 2);
    int x, y;
    getXY(&x, &y, destination);
    if( map[x][y] == 1)
    {
        map[x][y] = -1;
        if(amILost())
        {
            write(1, "You lost this game.\n", 21);
            send(gameSocketFD, "I Lost", 7, 0);
            return ;
        } 
        else
        {
            write(1, opponentName, strlen(opponentName));
            write(1, " successfully hit the ship!\n", 29);
            send(gameSocketFD, "success", 8, 0);
            game(false, false);
        }
    }
    else if( map[x][y] == -1)
    {
        write(1, opponentName, strlen(opponentName));
        write(1, " choose a duplicat block! Let him try agian.\n", 46);
        send(gameSocketFD, "duplicate", 10, 0);
        game(false, false);
    }
    else
    {
        map[x][y] = -1;
        write(1, opponentName, strlen(opponentName));
        write(1, " had an unsuccessfull shoot!\n", 30);
        send(gameSocketFD, "fail", 5, 0);
        game(true, false);
    }
}
