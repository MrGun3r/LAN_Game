#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#define GRAVITY 1000
#define PI 3.1415
#define MAX_CLIENTS 4
#define MAX_ROCKETS 4
#define MAX_EXPLOSIONS 4
#define KILL_FEED 5
#define ROCKET_WIDTH 20
#define ROCKET_HEIGHT 7
float FRICTION = 0.05;
int windowWidth = 900;
int windowHeight = 600;
int PLAYER_INDEX;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool running = true;
float deltaTime = 0;
float last_frame = 0;
float last_tick = 0;
float PLAYERSDATA[MAX_CLIENTS*4+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*KILL_FEED];
// data to send to server
float data[10];
SDL_Surface * TEXTURES_Surface = NULL;
SDL_Texture * TEXTURES_Texture = NULL;
SDL_Surface * BACKGROUND_Surface = NULL;
SDL_Texture * BACKGROUND_Texture = NULL;
SDL_Surface * TEXT_Surface = NULL;
SDL_Texture * TEXT_Texture = NULL;
int tickInit = 0;
// FILE HEADERS
#include "structs.h"
#include "collision.h"
#include "render_text.h"



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

   renderer = SDL_CreateRenderer(window,-1,0);
   if(!renderer){
    printf("Renderer Closed");
    return 0;
   }
   // init image
   IMG_Init(IMG_INIT_PNG);

   TEXTURES_Surface = IMG_Load("assets/atlas_textures.png");
   BACKGROUND_Surface = IMG_Load("assets/background.png");
   TEXTURES_Texture = SDL_CreateTextureFromSurface(renderer,TEXTURES_Surface);
   BACKGROUND_Texture = SDL_CreateTextureFromSurface(renderer,BACKGROUND_Surface);
   TEXT_Surface = IMG_Load("assets/font_atlas.png");  
   TEXT_Texture = SDL_CreateTextureFromSurface(renderer,TEXT_Surface);

   SDL_FreeSurface(TEXTURES_Surface);
   SDL_FreeSurface(BACKGROUND_Surface);
   SDL_FreeSurface(TEXT_Surface);
}
void INIT_GAMEDATA(){
  for(int i = 0;i<sizeof(PLAYERSDATA)/sizeof(PLAYERSDATA[0]);i++){
    PLAYERSDATA[i] = -9999;
  }
  for(int i = 0;i<sizeof(data)/sizeof(data[0]);i++){
    data[i] = -9999;
  }
  // PLAYERS
  player.x = 200;
  player.y = 200;
  player.veloX = 1;
  player.veloY = 1;
  player.accelerationX = 0;
  player.accelerationY = GRAVITY;
  player.speedLimit = 1000;
  player.width = 25;
  player.height = 35;
  player.onPlatform = false;
  player.knockBackX = 0;
  player.knockBackY = 0;
  player.fireRateMax = 500;
  player.fireRateTimer = 0;
  player.percentageTaken = 20000;
  player.rocketAngle = 0;
  player.angle = 0;
  player.animationID = 0;
  player.score = 0;
  player.tagID = -1;
  player.taggingDuration = 0;
  player.weaponPushBack = 0;
  player.killedEnemyOfId = -1;
  player.killedEnemyTextDuration = 0;
  // OBJECTS
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
void ControlPlayer(){
  if(key.a){
    player.accelerationX = -400;
    if(player.animationID > 0){
      player.animationID *= -1;
    }
    player.animationID -= 14*deltaTime;
  }
  else if(key.d){
    player.accelerationX = 400;
     if(player.animationID < 0){
      player.animationID *= -1;
    }
     player.animationID += 14*deltaTime;
  }
  else {
    if(SDL_abs((int)player.animationID) > 0.1){
      player.animationID *= 0.1;
    }
    player.accelerationX = 0;
  }
  if(SDL_abs((int)player.animationID) > 7.9){
    player.animationID = 0;
  }
  if(key.w && player.onPlatform){
    player.accelerationY = -2250;
  }
    if(key.mouseLeft && player.fireRateTimer > player.fireRateMax && (PLAYERSDATA[4*MAX_CLIENTS+1+3*PLAYER_INDEX] < 0 || PLAYERSDATA[4*MAX_CLIENTS+1+3*PLAYER_INDEX+1] < 0 )){
    player.fireRateTimer = 0;
    data[4] = player.x+player.width/2;
    data[5] = player.y+player.height/2;
    player.rocketAngle = SDL_atan((player.y+player.height/2 - mouse.y)/(player.x+player.width/2 - mouse.x));
    if((float)mouse.x >= player.x+player.width/2){
      player.rocketAngle += PI;
    }
    data[6] = -7*SDL_cos(player.rocketAngle);
    data[7] = -7*SDL_sin(player.rocketAngle);
    player.weaponPushBack = 10;
  }
  else{
    data[4] = PLAYERSDATA[4*MAX_CLIENTS+1+3*PLAYER_INDEX];
    data[5] = PLAYERSDATA[4*MAX_CLIENTS+1+3*PLAYER_INDEX+1];
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
      player.angle = SDL_atan((player.y+player.height/2 - mouse.y)/(player.x+player.width/2 - mouse.x));
      if((float)mouse.x >= player.x+player.width/2){
      player.angle += PI;
    }
    }
}
void DrawVideo(){
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderClear(renderer);   
    SDL_RenderCopy(renderer,BACKGROUND_Texture,NULL,NULL);

    SDL_RendererFlip flipPlayer = SDL_FLIP_NONE;
    if(player.animationID < 0){
      flipPlayer = SDL_FLIP_HORIZONTAL;
    }
    SDL_RenderCopyEx(renderer,TEXTURES_Texture,&(SDL_Rect){
      0+SDL_abs((int)player.animationID)*18,69,17,20
    },&(SDL_Rect){
      player.x,player.y,player.width,player.height
    },0,NULL,flipPlayer);

    SDL_RendererFlip flipRocket = SDL_FLIP_NONE;

    if(player.angle < PI/2){
      flipRocket = SDL_FLIP_VERTICAL;
    }
    SDL_RenderCopyEx(renderer,TEXTURES_Texture,&(SDL_Rect){
      0,0,35,25
    },&(SDL_Rect){
    player.x+(int)(player.width/4+player.weaponPushBack*cos(player.angle))-10*(flipRocket == SDL_FLIP_VERTICAL),player.y+(int)(player.height/4+player.weaponPushBack*sin(player.angle)),25,21
   },(player.angle-PI)*360/(2*PI),NULL,flipRocket);

    for(int i = 0;i<4*MAX_CLIENTS;i+=4){
      if((PLAYERSDATA[i] > -9998 || PLAYERSDATA[i+1] > -9998) && i/4 != PLAYER_INDEX){
        SDL_SetTextureColorMod(TEXTURES_Texture,255,0,0);
        flipPlayer = SDL_FLIP_NONE;
        if(PLAYERSDATA[i+3] < 0){
          flipPlayer = SDL_FLIP_HORIZONTAL;
        }
        SDL_RenderCopyEx(renderer,TEXTURES_Texture,&(SDL_Rect){
          0+SDL_abs((int)PLAYERSDATA[i+3])*18,69,17,20
        },&(SDL_Rect){
          PLAYERSDATA[i],PLAYERSDATA[i+1],player.width,player.height
        },0,NULL,flipPlayer);

     SDL_SetTextureColorMod(TEXTURES_Texture,255,255,255);
     // draw guns
    SDL_RendererFlip flipRocket = SDL_FLIP_NONE;
    if(PLAYERSDATA[i+2] < PI/2){
      flipRocket = SDL_FLIP_VERTICAL;
    }
    SDL_RenderCopyEx(renderer,TEXTURES_Texture,&(SDL_Rect){
      0,0,35,25
    },&(SDL_Rect){
      PLAYERSDATA[i]+(int)player.width/4,PLAYERSDATA[i+1]+(int)player.height/4,25,21
    },(PLAYERSDATA[i+2]-PI)*360/(2*PI),NULL,flipRocket);
      }
    }
    // DRAW ROCKETS
    for(int i = 4*MAX_CLIENTS+1;i<MAX_CLIENTS*3+1+3*MAX_ROCKETS;i+=3){
      if(PLAYERSDATA[i] > 0 && PLAYERSDATA[i+1] > 0){
        SDL_RenderCopyEx(renderer,TEXTURES_Texture,&(SDL_Rect){
        0,31,17,5},&(SDL_Rect){
        PLAYERSDATA[i],PLAYERSDATA[i+1],ROCKET_WIDTH,ROCKET_HEIGHT
      },(PLAYERSDATA[i+2])*360/(2*PI),NULL,SDL_FLIP_NONE);
      }
    }
    // DRAW EXPLOSIONS 
    for(int i = MAX_CLIENTS*4+1+3*MAX_ROCKETS;i<MAX_CLIENTS*3+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS;i+=3){
  
      if(PLAYERSDATA[i] > 0 && PLAYERSDATA[i+1]> 0){
        SDL_RenderCopy(renderer,TEXTURES_Texture,&(SDL_Rect){
          0+33*(int)PLAYERSDATA[i+2],37,30,30
        },&(SDL_Rect){
          PLAYERSDATA[i]-25,PLAYERSDATA[i+1]-25,50,50
        });
      }
      }
    // DRAW PLATFROMS
    for(int i = 0;i<sizeof(platforms)/sizeof(platforms[0]);i++){
      if(platforms[i].reserved){
        for(int j = 0;j<platforms[i].width;j+=30){
          SDL_RenderCopy(renderer,TEXTURES_Texture,&(SDL_Rect){0,94,16*SDL_min(30,platforms[i].width-j)/30,16},&(SDL_Rect){platforms[i].x+j,platforms[i].y,SDL_min(30,platforms[i].width-j),30});
          for(int k = 30;k<platforms[i].height;k+=30){
            SDL_RenderCopy(renderer,TEXTURES_Texture,&(SDL_Rect){16,94,16*SDL_min(30,platforms[i].width-j)/30,16*SDL_min(30,platforms[i].height-k)/30},&(SDL_Rect){platforms[i].x+j,platforms[i].y+k,SDL_min(30,platforms[i].width-j),SDL_min(30,platforms[i].height-k)});
          }
        }
      }
    }

    // DRAW SCOREBOARD
    for(int i = 0;i<MAX_CLIENTS;i++){
      if(PLAYERSDATA[4*i]>-9999 && PLAYERSDATA[4*i+1] > -9999){
        int deaths = PLAYERSDATA[MAX_CLIENTS*4+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+i];
        char displayText[15];
        sprintf(displayText,"Player %d  %d",i,deaths);
        int color[3] = {200,0,0};
        if(PLAYER_INDEX == i){
           SDL_memcpy(&color,(int[3]){255,255,255},3*sizeof(int));
        }
        renderText(displayText,sizeof("Player 0  ")+(int)SDL_log10(deaths+1),10,10+i*25,(sizeof("Player 0  ")+(int)SDL_log10(deaths+1))*10,20,255,color);
        
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderFillRect(renderer,&(SDL_Rect){
          10*8 +13,8+i*25,5,22
        });
        
        SDL_SetRenderDrawColor(renderer,200,200,200,255);
        SDL_RenderFillRect(renderer,&(SDL_Rect){
          10*8 +15,10+i*25,5,20
        });

      }
    }
    // DRAW KILL ALERT
    if(player.killedEnemyOfId > -0.5){
      char killedText[10];
      sprintf(killedText,"Player %d",(int)player.killedEnemyOfId);
      renderText("You killed",sizeof("You killed"),windowWidth/2-5*sizeof("You killed Player 0"),200,10*sizeof("You Killed"),20,255,(int[3]){255,255,255});
      renderText(killedText,sizeof("Player 0"),windowWidth/2-5*sizeof("You killed Player 0")+10*sizeof("You Killed"),200,10*sizeof("Player 0"),20,255,(int[3]){200,0,0});
    }
    // DRAW PERCENTAGE TEXT
    char percentageText[4];
    sprintf(percentageText,"%d",(int)(player.percentageTaken/20));
    renderText(percentageText,(int)SDL_log10((player.percentageTaken/20 ) + 1),10,windowHeight-30,(int)SDL_log10((player.percentageTaken/20)+ 1)*10,20,255,(int[3]){255,255,255});


    // DRAW KILL FEED
    for(int i = MAX_CLIENTS*4+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1;i<MAX_CLIENTS*4+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1+2*KILL_FEED;i+=2){
      int j = (i-(MAX_CLIENTS*4+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS+1))/2;
  
      if(PLAYERSDATA[i] > -0.5 && PLAYERSDATA[i+1] > -0.5){
        char textKillFeed[26];
        sprintf(textKillFeed,"Player %d Killed Player %d",(int)PLAYERSDATA[i],(int)PLAYERSDATA[i+1]);
        renderText(textKillFeed,sizeof(textKillFeed),windowWidth-8*sizeof(textKillFeed),10+17*j,8*sizeof(textKillFeed),15,255,(int[3]){200,200,200});
      }
    }



    SDL_RenderPresent(renderer);
}
void UpdateData(){
  player.score = PLAYERSDATA[MAX_CLIENTS*4+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+PLAYER_INDEX];
  if(player.tagID > -0.5){
    player.taggingDuration += 1000*deltaTime;
    if(player.taggingDuration>5000){
      player.tagID = -1;
      player.taggingDuration = 0;
   
    }
  }
  if(player.killedEnemyOfId > -0.5){
    player.killedEnemyTextDuration += 1000*deltaTime;
    if(player.killedEnemyTextDuration > 2500){
      player.killedEnemyOfId = -1;
      player.killedEnemyTextDuration = 0;
    }
  }
  if(PLAYERSDATA[4*MAX_CLIENTS+1+3*PLAYER_INDEX] < 0 || PLAYERSDATA[4*MAX_CLIENTS+1+3*PLAYER_INDEX+1] < 0){
        data[6] = 0;
        data[7] = 0;
      }
      if(fabs(player.weaponPushBack)>0.1){
        player.weaponPushBack*=pow(0.2,deltaTime);
      }
      else{
        player.weaponPushBack = 0;
      }
      if(PLAYERSDATA[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS] > -0.5){
        player.killedEnemyOfId = PLAYERSDATA[4*MAX_CLIENTS+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS+MAX_CLIENTS]; 
        player.killedEnemyTextDuration = 0;     
      }
    ControlPlayer();
    player.fireRateTimer += 1000*deltaTime;
      player.x += (player.veloX+player.knockBackX)*deltaTime;
      player.y += (player.veloY+player.knockBackY)*deltaTime;
      
      if(SDL_abs(player.veloX) < player.speedLimit){
        player.veloX += 2*(player.accelerationX)*deltaTime;
      }    
      if(SDL_abs(player.veloY) < player.speedLimit){
        player.veloY += (player.accelerationY)*deltaTime;
      }
      data[9] = -1;
      if(player.y > windowHeight || player.y < -200 || fabs(player.x+windowWidth/2) > 2000+windowWidth/2){
        if(player.tagID>-0.5){
          data[9] = player.tagID;
        }
        else{
         
          data[9] = PLAYER_INDEX;
        }
        player.tagID = -1;
        player.taggingDuration = 0;
        player.y = 100;
        player.x = 250;
        player.veloY = 0;
        player.knockBackY = 0;
        player.knockBackX = 0;
        player.percentageTaken = 10000;
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
    int j;
    for(int i = MAX_CLIENTS*4+1+3*MAX_ROCKETS;i<MAX_CLIENTS*3+1+3*MAX_ROCKETS+3*MAX_EXPLOSIONS;i+=3){
       float distance = pow(pow(PLAYERSDATA[i]-25-player.x,2)+pow(PLAYERSDATA[i+1]-25-player.y,2),0.5);
       j = (int)(i-(MAX_CLIENTS*4+1+3*MAX_ROCKETS))/3;
       if(distance > 100 || PLAYERSDATA[i+2]>1){
        continue;
       }
       if(distance < 5){
        distance = 5;
       }

       float angle = atan((PLAYERSDATA[i+1]-25-player.y)/(PLAYERSDATA[i]-25-player.x));
       float hitCoeff = player.percentageTaken;

       player.knockBackX = (hitCoeff/2)*(cos(angle))/distance;
       if(PLAYERSDATA[i]-25-player.x > 0){
         player.knockBackX = -(hitCoeff/2)*(cos(angle))/distance;
       }
       player.knockBackY = (hitCoeff)*((sin(angle)))/distance;
       if((PLAYERSDATA[i+1]-25-player.y) > 0 && (PLAYERSDATA[i]-25-player.x) > 0){
        player.knockBackY = -(hitCoeff)*((sin(angle)))/distance;
       }
      if(fabs(player.knockBackX) > 20000){
        player.knockBackX = 20000*(-1*player.knockBackX < 0);
      }
      if(fabs(player.knockBackY) > 20000){
        player.knockBackY = 20000*(-1*player.knockBackY < 0);
      }
      player.percentageTaken += (SDL_abs((int)player.knockBackX) + SDL_abs((int)player.knockBackY));

      if(j != PLAYER_INDEX){
        player.tagID = j;
        player.taggingDuration = 0;
      }
      player.accelerationY = 0;
      player.accelerationX = 0;
    }
     if(player.onPlatform){
      player.veloY = 0;
      player.accelerationY = 0;
     }
     else if(player.accelerationY<GRAVITY){
      player.accelerationY += (float)(12000*(2-key.w))*deltaTime;
     }
      player.veloX *= pow(FRICTION,deltaTime);
      player.knockBackX *= pow(0.025,deltaTime);
      if(player.knockBackX < 1 && player.knockBackX > -1 ){
        player.knockBackX = 0;
      }
      player.knockBackY *= pow(0.025,deltaTime);
      if(player.knockBackY < 1 && player.knockBackY > -1 ){
        player.knockBackY = 0;
      }
      
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
void Program_Kill(){
  printf("Disconnected\n");
  SDL_DestroyTexture(TEXTURES_Texture);
  SDL_DestroyTexture(BACKGROUND_Texture);
  SDL_DestroyTexture(TEXT_Texture);
  SDLNet_Quit();
  IMG_Quit();
  SDL_Quit();
}
int main(int argc, char *argv[]){
  SDLNet_Init();
  IPaddress ip;

  
  char ipConnect[16];
  int portConnect;
  bool connected = false;
  while(!connected){
  printf("Enter IP address to join : ");
  scanf("%s",&ipConnect);
  printf("Enter port to join : ");
  scanf("%d",&portConnect);
  printf("Connecting to %s:%d...\n",ipConnect,portConnect);
  if(SDLNet_ResolveHost(&ip,ipConnect,portConnect) != -1){
    connected = true;
  }
  else{
    printf("Failed to connected.\n");
  }
  }
  printf("Connected to %s:%d\n",ipConnect,portConnect);
  TCPsocket client=SDLNet_TCP_Open(&ip);
  INIT_VIDEO();
  INIT_GAMEDATA();
  void* dataPtr = data;
  SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(2);
  SDLNet_TCP_AddSocket(socketSet, client);
  while(running){
    if(SDL_GetTicks()-last_tick>0){
      data[0] = player.x;
      data[1] = player.y;
      data[2] = player.angle;
      data[8] = player.rocketAngle;
      data[3] = player.animationID;
      dataPtr = (void*)data; 
      int timeStart = SDL_GetTicks();
      
      if(SDLNet_TCP_Send(client,dataPtr,10*sizeof(dataPtr)) < 0){
        break;
        }
      //if(SDLNet_CheckSockets(socketSet,10) > 0){ 
        SDLNet_TCP_Recv(client,PLAYERSDATA,sizeof(PLAYERSDATA));
        PLAYER_INDEX = PLAYERSDATA[4*MAX_CLIENTS];
       // }
        last_tick = SDL_GetTicks();
    }
    MainLoop();
  }
  SDLNet_FreeSocketSet(socketSet);
  SDLNet_TCP_Close(client);
  Program_Kill();
  return 0;
}