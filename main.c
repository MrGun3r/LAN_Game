#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define GRAVITY 1000
#define PI 3.1415
#define MAX_CLIENTS 3
#define MAX_ROCKETS 4
float FRICTION = 0.1;
int windowWidth = 900;
int windowHeight = 600;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool running = true;
float deltaTime = 0;
float last_frame = 0;
float last_tick = 0;
float PLAYERSDATA[MAX_CLIENTS*2+1+2*MAX_ROCKETS];
float data[6];
int tickInit = 0;
// FILE HEADERS
#include "structs.h"
#include "collision.h"

struct PlatformData platforms[10];

int INIT_VIDEO(){
  if(SDL_Init(SDL_INIT_VIDEO) != 0){
    printf("SDL unsupported !");
    return 0;
   }
   window = SDL_CreateWindow("Game",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,
   windowWidth,windowHeight,0
 );
   if(!window){
    printf("Window Closed");
    return 0;
   }
   renderer = SDL_CreateRenderer(window,2,-1);
   SDL_RendererInfo rendererInfo = {};
   if(!renderer){
    printf("Renderer Closed");
    return 0;
   }
   SDL_SetRenderDrawBlendMode(renderer,SDL_BLENDMODE_BLEND);
}
void INIT_GAMEDATA(){
  // PLAYERS
  player.x = 200;
  player.y = 200;
  player.veloX = 1;
  player.veloY = 1;
  player.accelerationX = 0;
  player.accelerationY = GRAVITY;
  player.speedLimit = 1000;
  player.width = 20;
  player.height = 20;
  player.onPlatform = false;
  // OBJECTS
  platforms[0].reserved = true;
  platforms[0].x = 200;
  platforms[0].y = 500;
  platforms[0].width = 500;
  platforms[0].height = 50;

  platforms[1].reserved = true;
  platforms[1].x = 200;
  platforms[1].y = 400;
  platforms[1].width = 50;
  platforms[1].height = 50;
}
void ControlPlayer(){
  if(key.a){
    player.accelerationX = -400;
  }
  else if(key.d){
    player.accelerationX = 400;
  }
  else {
    player.accelerationX = 0;
  }

  if(key.w && player.onPlatform ){
    player.accelerationY = -2250;
  }

  if(key.mouseLeft){
    int index = PLAYERSDATA[2*MAX_CLIENTS];
    data[2] = player.x+player.width/4;
    data[3] = player.y+player.height/4;
    float angle = SDL_atan((player.y+player.height/2 - mouse.y)/(player.x+player.width/2 - mouse.x));
    if((float)mouse.x >= player.x+player.width/2){
          angle += PI;
          }
    data[4] = -10*SDL_cos(angle);
    data[5] = -10*SDL_sin(angle);
  }
  else{
    int index = PLAYERSDATA[2*MAX_CLIENTS];
    data[2] = PLAYERSDATA[2*MAX_CLIENTS+1+2*index];
    data[3] = PLAYERSDATA[2*MAX_CLIENTS+1+2*index+1];
  }
 
  
}
void CheckInput(){
    SDL_Event event;
    SDL_PollEvent(&event);
    
    if(event.type == SDL_QUIT){
      running = false;
    }
    if(event.type == SDL_KEYDOWN){
      switch(event.key.keysym.sym){
        case SDLK_w:{
          key.w = true;
          break;
        }    
        case SDLK_s:{
          key.s = true;
          break;  
      }
      case SDLK_d:{
          key.d = true;
          break;  
      }
      case SDLK_a:{
          key.a = true;
          break;  
      }
     }
    }
    if(event.type == SDL_KEYUP){
      switch(event.key.keysym.sym){
        case SDLK_w:{
          key.w = false;
          break;
        }    
        case SDLK_s:{
          key.s = false;
          break;  
      }
      case SDLK_d:{
          key.d = false;
          break;  
      }
      case SDLK_a:{
          key.a = false;
          break;  
      }
     }
    }
    if(event.type == SDL_MOUSEBUTTONDOWN){
      key.mouseLeft = true;
    }
    if(event.type == SDL_MOUSEBUTTONUP){
      key.mouseLeft = false;
    }
    if(event.type == SDL_MOUSEMOTION){
      SDL_GetMouseState(&mouse.x,&mouse.y);
    }
}
void DrawVideo(){
    SDL_SetRenderDrawColor(renderer,100,100,100,255);
    SDL_RenderClear(renderer);   
    SDL_SetRenderDrawColor(renderer,255,255,255,255);

    SDL_RenderFillRect(renderer,&(SDL_Rect){
         player.x,player.y,player.width,player.height
        });
    for(int i = 0;i<2*MAX_CLIENTS;i+=2){
      if((PLAYERSDATA[i] > -98 || PLAYERSDATA[i+1] > -98) && i/2 != PLAYERSDATA[2*MAX_CLIENTS]){
        SDL_RenderFillRect(renderer,&(SDL_Rect){
         PLAYERSDATA[i],PLAYERSDATA[i+1],player.width,player.height
        });
      }
    }
    // DRAW ROCKETS
    SDL_SetRenderDrawColor(renderer,100,0,0,255);
    for(int i = 2*MAX_CLIENTS+1;i<MAX_CLIENTS*2+1+2*MAX_ROCKETS;i+=2){
      SDL_RenderFillRect(renderer,&(SDL_Rect){
        PLAYERSDATA[i],PLAYERSDATA[i+1],10,10
      });
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    for(int i = 0;i<sizeof(platforms)/sizeof(platforms[0]);i++){
      if(platforms[i].reserved){
        SDL_RenderFillRect(renderer,&(SDL_Rect){
          platforms[i].x,platforms[i].y-1,platforms[i].width,platforms[i].height+1
        });
      }
    }
    SDL_RenderPresent(renderer);
}
void UpdateData(){
    ControlPlayer();
      player.x += player.veloX*deltaTime;
      player.y += player.veloY*deltaTime;
      if(player.veloX < player.speedLimit){
        player.veloX += 2*player.accelerationX*deltaTime;
      }    
      if(player.veloY < player.speedLimit){
        player.veloY += player.accelerationY*deltaTime;
      }
      if(player.y > windowHeight){
        player.y = 100;
        player.veloY = 0;
      }
     bool BoolSwitch = true;
     for(int j = 0;j<sizeof(platforms)/sizeof(platforms[0]);j++){
      if(platforms[j].reserved){
        if(BoolSwitch){
          player.onPlatform = false;
        }
        if(collisionCheck(player,platforms[j])){
          collisionCorrection(&player,&platforms[j],&BoolSwitch);
        }
      }
    }
     if(player.onPlatform){
      player.veloY = 0;
      player.accelerationY = 0;
     }
     else if(player.accelerationY<GRAVITY){
      player.accelerationY += (float)(12000*(2-key.w))*deltaTime;
     }
      player.veloX *= pow(FRICTION,deltaTime);
}
void GAME_DeltaTime(){ 
  deltaTime = ((float)SDL_GetTicks() - last_frame)/1000.0f;
  last_frame = SDL_GetTicks();
}
void MainLoop(){
    CheckInput();
    DrawVideo();
    UpdateData();

    GAME_DeltaTime();
}
int main(int argc, char *argv[]){
  INIT_VIDEO();
  INIT_GAMEDATA();
  SDLNet_Init();
  IPaddress ip;
  SDLNet_ResolveHost(&ip,"192.168.8.116",1234);
  TCPsocket client=SDLNet_TCP_Open(&ip);
  void* dataPtr = data;
  SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
  SDLNet_TCP_AddSocket(socketSet, client);
  while(running){
    if(SDL_GetTicks()-last_tick>0){
      data[0] = player.x;
      data[1] = player.y;
      dataPtr = (void*)data; 
      if(SDLNet_TCP_Send(client,dataPtr,6*sizeof(dataPtr)) > 0){  
        SDLNet_TCP_Recv(client,PLAYERSDATA,sizeof(PLAYERSDATA));
        for(int i = 0;i<sizeof(PLAYERSDATA)/sizeof(PLAYERSDATA[0]);i++){
          printf("%d :%f\n",i,PLAYERSDATA[i]);
        }
        int index = PLAYERSDATA[2*MAX_CLIENTS];
        last_tick = SDL_GetTicks();
       }
    else{break;}
    }
    MainLoop();
  }
  printf("Disconnected\n");
  SDLNet_TCP_Close(client);
  SDLNet_Quit();
  SDL_Quit();
  return 0;
}