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
  float fireRateMax;
  float fireRateTimer;
  float percentageTaken;
  float rocketAngle;
  float angle;
  float animationID;
  float score;
  float tagID;
  float taggingDuration;
  float weaponPushBack;
  float killedEnemyOfId;
  float killedEnemyTextDuration;
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
