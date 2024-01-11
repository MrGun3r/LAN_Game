
void renderText(char *Text,int stringCount,int x ,int y, int width,int height,int opacity,int color[3]){
  // DEALS WITH TEXT AS WELL AS NUMBERS
  
  char str[stringCount];
  for(int i = 0;i<stringCount;i++){
    str[i] = *(Text+i);
  }
  int posX;
  int cap;
  int num;
  SDL_SetTextureAlphaMod(TEXT_Texture,opacity); 
  // OUTLINE --> TEXT (order of rendering) 
   // {outline}
  for(int i = 0;i<stringCount;i++){
      cap = 0;
      num = 0;
      int charValue = str[i] - 'a';
      if (str[i]>=65 && str[i]<=90){
        cap = 1;
        num = 0;
        charValue += 32;
      }
      else if (str[i]>= 48 && str[i]<= 57){
        num = 1;
        cap = 0;
        charValue += 49;
      }
      posX = charValue*40;
      SDL_Rect srcRect = {
      posX,
      80*cap+150*num,
      40,
      70-(6*cap)
    };
    int deviation = SDL_floor((width/(stringCount*5))+1);
     SDL_Rect OUTLINEdstRect = {
      x+i*(width/stringCount)-deviation,
      y-deviation,
      width/stringCount+deviation,
      height+deviation
    };
    SDL_SetTextureColorMod(TEXT_Texture, 0,0,0);
    SDL_RenderCopy(renderer,TEXT_Texture,&srcRect,&OUTLINEdstRect);
    }
    // {text}
  for (int i = 0;i<stringCount;i++){
      cap = 0;
      num = 0;
      int charValue = str[i] - 'a';
      if (str[i]>=65 && str[i]<=90){
        cap = 1;
        num = 0;
        charValue += 32;
      }
      else if (str[i]>= 48 && str[i]<= 57){
        num = 1;
        cap = 0;
        charValue += 49;
      }
      posX = charValue*40;
    
    SDL_Rect srcRect = {
      posX,
      80*cap+150*num,
      40,
      70 -(6*cap)
    };
    SDL_Rect dstRect = {
      x+i*(width/stringCount)
      ,y
      ,width/stringCount
      ,height
    };
    
    
    SDL_SetTextureColorMod( TEXT_Texture, color[0],color[1],color[2]);
    SDL_RenderCopy(renderer,TEXT_Texture,&srcRect,&dstRect);
    }  
    SDL_SetTextureAlphaMod(TEXT_Texture,255);
}