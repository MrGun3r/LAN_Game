

int collisionCheck(struct playersData player,struct PlatformData platform){
  return player.x+player.width > platform.x && platform.x+platform.width > player.x && player.y+player.height > platform.y && platform.y+platform.height > player.y;
}
void collisionCorrection(struct playersData *player,struct PlatformData *platform,bool *BoolSwitch){  
  if (SDL_min(SDL_abs(player->x+player->width-platform->x),SDL_abs(player->x-platform->x-platform->width))>SDL_min(SDL_abs(player->y+player->height-platform->y),SDL_abs(player->y-platform->y-platform->height)))   
    {
      if (SDL_abs(player->y + player->height+1 - platform->y)>SDL_abs(player->y - platform->y - platform->height)){
        player->y = platform->y + platform->height;
        player->veloY = 1;
        player->knockBackY = 0;
       }
       else {player->y = platform->y - player->height-1;
             player->onPlatform = true;
             *BoolSwitch = false;
             player->knockBackY = 0;
        }
    }
    else {
      if (SDL_abs(player->x + player->width -platform->x)>SDL_abs(player->x - platform->x - platform->width)){
      player->x = platform->x + platform->width;   
      player->veloX = 0;
      player->knockBackX = 0;
     }
     else {player->x = platform->x - player->width;
     player->veloX = 0;
     player->knockBackX = 0;
      }
    }
}