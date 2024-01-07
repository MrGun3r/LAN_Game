#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#define MAX_CLIENTS 3
#define MAX_ROCKETS 10
typedef struct{
    TCPsocket socket;   
} ClientData;




void ServerData();


float gameData[2*MAX_CLIENTS+MAX_ROCKETS*4+1];
int activeClientList[MAX_CLIENTS] = {0};
int activeClients = 0;
int main(int argc, char *argv[]){
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

  ClientData clients[MAX_CLIENTS] = {0};
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
      float data[6];
    if(SDLNet_TCP_Recv(clients[i].socket,data,sizeof(data)) <= 0){
       clients[i].socket = 0;
       activeClients--;
       activeClientList[i] = 0;
       printf("User left with id %d\n",i);
       gameData[2*i] = -99;
       gameData[2*i+1] = -99;
       continue;
    }
    ServerData(&data);
    // player position
    gameData[2*i] = data[0];
    gameData[2*i+1] = data[1];
    // rocket data
    gameData[2*MAX_CLIENTS+1+4*i]   = data[2];
    gameData[2*MAX_CLIENTS+1+4*i+1] = data[3];
  
    // player index
    gameData[2*MAX_CLIENTS] = i;
    void* gameDataPtr = gameData;
    SDLNet_TCP_Send(clients[i].socket,gameDataPtr,sizeof(gameData));
     }
    }
    SDL_Delay(10);     
  }
  SDLNet_TCP_Close(server);
  SDLNet_Quit();
}
void ServerData(float *data){
  data[2] += data[4];
  data[3] += data[5];
}