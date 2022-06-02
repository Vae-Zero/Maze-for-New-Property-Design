#include "ssd1306.h"
#include "nano_gfx.h"
#include "maze.h"

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define WIFI_SSID "Prototyping_1"
#define WIFI_PASS "hello world"
#define UDP_PORT 9527
WiFiUDP UDP;
char packet[255];

int gLevel = 4;     //0 = Easy, 1 = Norm, 2 = Hard
int gDir = 4;       //0 = Up, 1 = Right, 2 = Down, 3 = Left
int gX[4] = {-1, 0, 1, 0};
int gY[4] = {0, 1, 0, -1};

const int StartX = 1, StartY = 8;
const int EndX = 15, EndY = 8;
int X = 0, Y = 0;
int NewX = 0, NewY = 0;

#include <arduino-timer.h>
auto timer = timer_create_default();
int ReTime = 300;
//count down (300s in total)
bool cntdown(void *){
  ReTime--;
  return true;  // keep the timer active
}

// Draw the maze and moving point according to gLevel
void BitmapDraw(int gLevel){
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_setColor(RGB_COLOR16(255, 255, 255));
  ssd1306_clearScreen();    
  ssd1306_drawXBitmap(0, 0, 128, 128, pmaze[gLevel]);
  ssd1306_setColor(RGB_COLOR16(0, 0, 255));
  X = StartX; Y = StartY; 
  ssd1306_printFixed(Y * 8, X * 8, "o", STYLE_NORMAL); 
  ReTime = 300; timer.every(1000, cntdown);         // start to countdown
}

// Move the point accroding to gDir
void Move(int gDir){
  if (gLevel == 4) return;     // avoid selecting directiona before choosing a game
  NewX = X + gX[gDir];
  NewY = Y + gY[gDir];
  // is a movable blank 
  if (movable[gLevel][NewX][NewY]){
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_setColor(RGB_COLOR16(0, 0, 0));
    ssd1306_printFixed(Y * 8, X * 8, "o", STYLE_NORMAL);
    X = NewX; Y = NewY;
    ssd1306_setColor(RGB_COLOR16(0, 0, 255));
    ssd1306_printFixed(Y * 8, X * 8, "o", STYLE_NORMAL);
  }
  if (X == EndX && Y == EndY){
    ssd1306_clearScreen( );
    ssd1306_setFixedFont(ssd1306xled_font8x16);
    ssd1306_drawXBitmap(0, 0, 64, 56, rose);
    ssd1306_printFixed(32, 64, "YOU WIN!", STYLE_NORMAL);  
    gLevel = 4;  
  }
}

void setup()
{
  // Setup serial port
  Serial.begin(115200);
  Serial.println();  
  WiFi.begin(WIFI_SSID, WIFI_PASS); 
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }
  
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
 
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);

  // =================================================
  
  st7735_128x160_spi_init(3, 4, 5);
  ssd1306_clearScreen();
  ssd1306_setFixedFont(ssd1306xled_font8x16);
  //ssd1306_setMode( LCD_MODE_NORMAL );
  ssd1306_clearScreen();
  ssd1306_setColor(RGB_COLOR16(255, 255, 255));
  ssd1306_printFixed(28, 50, "Maze Game", STYLE_NORMAL);
  ssd1306_setFixedFont(ssd1306xled_font6x8);
  ssd1306_printFixed(40, 72, "to start", STYLE_NORMAL);
}

void loop(){
  timer.tick();  // tick the timer

  // If packet received...
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    Serial.println(packetSize); 
    int len = UDP.read(packet, 255);
    //if (len > 0) packet[len] = '\0';
    //Serial.print("Packet received: ");
    //Serial.println(packet);

    digitalWrite(D4, !digitalRead(D4)); // 指示灯
    
    switch(packet[0])
    {
      case 'U': gDir = 0; Move(gDir); break;
      case 'R': gDir = 1; Move(gDir); break;
      case 'D': gDir = 2; Move(gDir); break;
      case 'L': gDir = 3; Move(gDir); break;      
      case 'E': gLevel = 0; BitmapDraw(gLevel); break;
      case 'N': gLevel = 1; BitmapDraw(gLevel); break;
      case 'H': gLevel = 2; BitmapDraw(gLevel); break;
    } 
  }
  if (ReTime <= 0 && gLevel != 4) {
    ssd1306_clearScreen();
    ssd1306_setFixedFont(ssd1306xled_font8x16);
    ssd1306_setColor(RGB_COLOR16(255, 255, 255));
    ssd1306_printFixed(28, 50, "TIME OVER", STYLE_NORMAL);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_printFixed(44, 72, "restart", STYLE_NORMAL);
  }
}
