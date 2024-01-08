struct keys{
  bool a;
  bool d;
  bool s;
  bool w;
  bool mouseLeft;
} key;
struct mouse{
  int x;
  int y;
} mouse;
struct playersData{
  bool onPlatform;
  float x;
  float y;
  float width;
  float height;
  float veloX;
  float veloY;
  float accelerationX;
  float accelerationY;
  float knockBackX;
  float knockBackY;
  float speedLimit;
} player;
struct rocketData{
 float x;
 float y;
} rocket;
struct PlatformData{
    bool reserved;
    float x;
    float y;
    float width;
    float height;
};

struct PlatformData platforms[10];
