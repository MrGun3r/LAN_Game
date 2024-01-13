#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_CLIENTS 4
#define MAX_ROCKETS 4
#define MAX_EXPLOSIONS 4
#define ROCKET_WIDTH 20
#define ROCKET_HEIGHT 7
#define KILL_FEED 5
float data[13];
int windowWidth = 900;
int windowHeight = 600;
typedef struct{
    TCPsocket socket;   
} ClientData;


float kill_feed_timer[KILL_FEED];
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


float gameData[4*MAX_CLIENTS+MAX_ROCKETS*3+1+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*KILL_FEED];
float playersKilled[MAX_CLIENTS];
int activeClientList[MAX_CLIENTS] = {0};
int activeClients = 0;
int main(int argc, char *argv[]){
  mapData();
  // FILL UP WITH -99
  for(int i = 0;i<sizeof(gameData)/sizeof(gameData[0]);i++){
    gameData[i] = -9999;
  }
  for(int i = 0;i<MAX_CLIENTS;i++){
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+i] = 0;
  }
  for(int i = 0;i<MAX_CLIENTS;i++){
    playersKilled[i] = -1;
  }
  for(int i = 0;i<KILL_FEED;i++){
    kill_feed_timer[i] = 0;
  }
  SDL_Init(SDL_INIT_EVERYTHING);
  SDLNet_Init();
  IPaddress ip;
  IPaddress ip2;
  SDLNet_ResolveHost(&ip,NULL,1234);
  TCPsocket server=SDLNet_TCP_Open(&ip);
  printf("   Network Port. . . . . . . . . . . . . .: %d\n",SDLNet_Read16(&ip.port));
  system("C:\\Windows\\System32\\ipconfig | findstr /i \"ipv4\"");

  printf("\n   Max Server Capacity : %d\n",MAX_CLIENTS);
  ClientData clients[MAX_CLIENTS];
  SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(MAX_CLIENTS+1);

  SDLNet_TCP_AddSocket(socketSet, server);
  printf("    Server is Open\n");
  while(1){
    for(int i = 0;i<MAX_CLIENTS;i++){
        if(!clients[i].socket){
            clients[i].socket = SDLNet_TCP_Accept(server);
        }
        else{
          if(!activeClientList[i]){
              activeClientList[i] = 1;
              activeClients++;
            }
        }
    }
  
    for(int i = 0;i<MAX_CLIENTS;i++){
    if(clients[i].socket){
      for(int i = 0;i<sizeof(data)/sizeof(data[0]);i++){
          data[i] = -9999;
       }
      if(SDLNet_TCP_Recv(clients[i].socket,data,sizeof(data)) <= 0){
        clients[i].socket = 0;
        activeClients--;
        activeClientList[i] = 0;
        gameData[4*i] = -9999;
        gameData[4*i+1] = -9999;
        continue;
       }
    // UPDATE SERVER 

    data[10] = gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i];
    data[11] = gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i+1];
    data[12] = gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i+2];


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
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i]   = data[10];
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i+1] = data[11];
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*i+2] = data[12];
    // score data
    if(data[9]>-0.5){
      if((int)data[9] == i){
        if((int)gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+(int)data[9]]>0){
          gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+(int)data[9]]--;
        }
        
      }
      else{
      gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+(int)data[9]]++;
      playersKilled[(int)data[9]] = i; 
      for(int j = 1;j<KILL_FEED;j++){
        gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*(KILL_FEED-j)] = gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*(KILL_FEED-(j+1))];
        gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*(KILL_FEED-j) + 1] = gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*(KILL_FEED-(j+1)) + 1];
        kill_feed_timer[KILL_FEED-j] = kill_feed_timer[KILL_FEED-(j+1)];
      }
      kill_feed_timer[0] = 255;
      gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1] = data[9];
      gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+1] = i;
      }
      
    }
    
    gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS] = playersKilled[i];
    playersKilled[i] = -1;
    gameData[4*MAX_CLIENTS] = i;
    void* gameDataPtr = gameData;
    SDLNet_TCP_Send(clients[i].socket,gameDataPtr,sizeof(gameData));
    }
    }
    SDL_Delay(10);  
  }
  SDLNet_FreeSocketSet(socketSet);
  SDLNet_TCP_Close(server);
  SDLNet_Quit();
  return 0;
}
void ServerData(float *data){
  for(int i = 0;i<KILL_FEED;i++){
    if(kill_feed_timer[i]>=0){
      kill_feed_timer[i] -= 0.25;

      if(kill_feed_timer[i] <= 0){
        gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*i] = -9999;
        gameData[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*i + 1] = -9999;
      }
    }
  }
  data[4] += data[6];
  data[5] += data[7];
  if(data[10]>0 || data[11]>0){
    data[12] += 0.15;
  if(data[12] > 5){
   data[12] = 6;
   data[10] = -9999;
   data[11] = -9999;
  }
  }
  
for(int i = 0;i<sizeof(platforms)/sizeof(platforms[0]);i++){
  if(platforms[i].reserved){
    if(collisionCheck(data[4],data[5],ROCKET_WIDTH,ROCKET_HEIGHT,platforms[i].x,platforms[i].y,platforms[i].width,platforms[i].height) ||data[4]>windowWidth || data[5] >windowHeight){
    data[10] = data[4];
    data[11] = data[5];
    data[12] = 0;
    data[4] = -9999;
    data[5] = -9999;
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