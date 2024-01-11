#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_CLIENTS 3
#define MAX_ROCKETS 4
#define MAX_EXPLOSIONS 10
#define ROCKET_WIDTH 20
#define ROCKET_HEIGHT 7

int windowWidth = 900;
int windowHeight = 600;
typedef struct{
    TCPsocket socket;   
} ClientData;


int collisionCheck(float ax,float ay,float awidth,float aheight,float bx,float by,float bwidth,float bheight){
  return (ax+awidth > bx && bx+bwidth > ax && ay+aheight > by && by+bheight > ay);

  
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


float gameData[4*MAX_CLIENTS+MAX_ROCKETS*3+1+3*MAX_EXPLOSIONS+MAX_CLIENTS];
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

      float data[13];
      if(SDLNet_TCP_Recv(clients[i].socket,data,sizeof(data)) <= 0 && SDLNet_CheckSockets(socketSet,0) != -1){
        clients[i].socket = 0;
        activeClients--;
        activeClientList[i] = 0;
        printf("User left with id %d\n",i);
        gameData[4*i] = -99;
        gameData[4*i+1] = -99;
        continue;
       }
    // UPDATE SERVER 
    ServerData(&data);
    // player position {x,y,angle,animationID}
    gameData[4*i] = data[0];
    gameData[4*i+1] = data[1];
    gameData[4*i+2] = data[2];
    gameData[4*i+3] = data[3];
    // rocket data {x,y,angle}
    gameData[4*MAX_CLIENTS+1+3*i]   = data[4];
    gameData[4*MAX_CLIENTS+1+3*i+1] = data[5];
    gameData[4*MAX_CLIENTS+1+3*i+2] = data[8];
    // explosion data {x,y,animationID}
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i] = data[10];
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i+1] = data[11];
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i+2] = data[12];
    // deaths data
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+i] = data[9];
    gameData[4*MAX_CLIENTS] = i;
    void* gameDataPtr = gameData;
    SDLNet_TCP_Send(clients[i].socket,gameDataPtr,sizeof(gameData));
    }
     
    }
    SDL_Delay(10);   
  }
  SDLNet_TCP_Close(server);
  SDLNet_Quit();
  return 0;
}
void ServerData(float *data){
  data[4] += data[6];
  data[5] += data[7];
  if(data[10]>0 || data[11]>0){
    data[12] += 0.15;
  if(data[12] > 5){
   data[12] = 6;
   data[10] = -99;
   data[11] = -99;
  }
  }
  
for(int i = 0;i<sizeof(platforms)/sizeof(platforms[0]);i++){
  if(platforms[i].reserved){
    if(collisionCheck(data[4],data[5],ROCKET_WIDTH,ROCKET_HEIGHT,platforms[i].x,platforms[i].y,platforms[i].width,platforms[i].height)){
    printf("bru\n");
    data[10] = data[4];
    data[11] = data[5];
    data[12] = 0;
    data[4] = -99;
    data[5] = -99;
    break;
  }
  }
  
}
}

void mapData(){
platforms[0].reserved = true;
  platforms[0].x = 200;
  platforms[0].y = 500;
  platforms[0].width = 500;
  platforms[0].height = 50;

  platforms[1].reserved = true;
  platforms[1].x = 100;
  platforms[1].y = 400;
  platforms[1].width = 50;
  platforms[1].height = 30;

  platforms[2].reserved = true;
  platforms[2].x = 750;
  platforms[2].y = 400;
  platforms[2].width = 50;
  platforms[2].height = 30;

  platforms[3].reserved = true;
  platforms[3].x = 400;
  platforms[3].y = 350;
  platforms[3].width = 100;
  platforms[3].height = 40;
}