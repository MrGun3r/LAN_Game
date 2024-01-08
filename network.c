#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_CLIENTS 3
#define MAX_ROCKETS 4
#define MAX_EXPLOSIONS 10
#define ROCKET_WIDTH 10
#define ROCKET_HEIGHT 10

int windowWidth = 900;
int windowHeight = 600;
typedef struct{
    TCPsocket socket;   
} ClientData;


int collisionCheck(float ax,float ay,float awidth,float aheight,float bx,float by,float bwidth,float bheight){
  return ax+awidth > bx && bx+bwidth > ax && ay+aheight > by && by+bheight > by;
}
// MAP DATA
struct PlatformData{
    bool reserved;
    float x;
    float y;
    float width;
    float height;
};


struct PlatformData platforms[10];  
void mapData();
void ServerData();


float gameData[2*MAX_CLIENTS+MAX_ROCKETS*2+1+3*MAX_EXPLOSIONS];
int activeClientList[MAX_CLIENTS] = {0};
int activeClients = 0;
int main(int argc, char *argv[]){
  mapData();
  // FILL UP WITH -99
  for(int i = 0;i<sizeof(gameData)/sizeof(gameData[0]);i++){
    gameData[i] = -99;
  }
  SDL_Init(SDL_INIT_EVERYTHING);
  SDLNet_Init();
  IPaddress ip;
  IPaddress ip2;
  SDLNet_ResolveHost(&ip,NULL,1234);
  TCPsocket server=SDLNet_TCP_Open(&ip);
  printf("Network Port is:%d\n",SDLNet_Read16(&ip.port));
  Uint8* IpIter =(Uint8*) &ip.host;
  printf("Network IP address is: ");
  printf("%d.",*(IpIter));
  printf("%d.",*(IpIter+1));
  printf("%d.",*(IpIter+2));
  printf("%d ",*(IpIter+3));
  printf("\n");
  
  ClientData clients[MAX_CLIENTS];
  SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);

  SDLNet_TCP_AddSocket(socketSet, server);

  while(1){
    for(int i = 0;i<MAX_CLIENTS;i++){
        if(!clients[i].socket){
            clients[i].socket = SDLNet_TCP_Accept(server);
        }
        else{
          if(!activeClientList[i]){
              activeClientList[i] = 1;
              printf("User Joined with id of %d\n",i);
              activeClients++;

            }
        }
    }
    for(int i = 0;i<MAX_CLIENTS;i++){
    if(clients[i].socket){
      float data[9];
      if(SDLNet_TCP_Recv(clients[i].socket,data,sizeof(data)) <= 0 && SDLNet_CheckSockets(socketSet,0) != -1){
        clients[i].socket = 0;
        activeClients--;
        activeClientList[i] = 0;
        printf("User left with id %d\n",i);
        gameData[2*i] = -99;
        gameData[2*i+1] = -99;
        continue;
       }

    // UPDATE SERVER 
    ServerData(&data);

    // player position
    gameData[2*i] = data[0];
    gameData[2*i+1] = data[1];
    // rocket data
    gameData[2*MAX_CLIENTS+1+2*i]   = data[2];
    gameData[2*MAX_CLIENTS+1+2*i+1] = data[3];

    gameData[2*MAX_CLIENTS+1+2*MAX_ROCKETS+3*i] = data[6];
    gameData[2*MAX_CLIENTS+1+2*MAX_ROCKETS+3*i+1] = data[7];
    gameData[2*MAX_CLIENTS+1+2*MAX_ROCKETS+3*i+2] = data[8];

    gameData[2*MAX_CLIENTS] = i;
    void* gameDataPtr = gameData;
    SDLNet_TCP_Send(clients[i].socket,gameDataPtr,sizeof(gameData));
    }
     
    }   
  }
  SDLNet_TCP_Close(server);
  SDLNet_Quit();
}
void ServerData(float *data){
  data[2] += data[4];
  data[3] += data[5];
  if(data[6]>0 || data[7]>0){
    data[8] -= 10;
  if(data[8] < 0){
   data[8] = 0;
   data[6] = -99;
   data[7] = -99;
  }
  }
  
for(int i = 0;i<sizeof(platforms)/sizeof(platforms[0]);i++){
  if(collisionCheck(data[2],data[3],ROCKET_WIDTH,ROCKET_HEIGHT,platforms[i].x,platforms[i].y,platforms[i].width,platforms[i].height) || data[2] > windowWidth || data[3] > windowHeight){
    data[6] = data[2];
    data[7] = data[3];
    data[8] = 255;
    data[2] = -99;
    data[3] = -99;
    break;
  }
}
  
}

void mapData(){
  platforms[0].x = 200;
  platforms[0].y = 500;
  platforms[0].width = 500;
  platforms[0].height = 50;

  platforms[1].x = 200;
  platforms[1].y = 400;
  platforms[1].width = 50;
  platforms[1].height = 50;
}