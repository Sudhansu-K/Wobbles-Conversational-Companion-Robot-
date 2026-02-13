#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Servo.h>
#include <Stepper.h>
#include <U8g2lib.h>
#include "AudioTools.h"
#include "AudioTools/AudioCodecs/CodecMP3Helix.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
char expression[]="-_-\0";
int override_exp=0;
int progress_load=1;
int y_position=55;

#define Wifi_network ""
#define Wifi_password ""
#define Timeout 7000
#define urladdress "http://10.53.7.6:8000/live.mp3"

int comm[2]={0,0};
int ic=0,timep=5000,curr_pos=90,curr_pos1=90;
static const int servopin[2]={18,19};
const int motorpin1=32;
const int motorpin2=33;
const int motorpin3=13;
const int motorpin4=12;
const int motor_en=14;
const int speed=70;

URLStream url(Wifi_network,Wifi_password);
AnalogAudioStream out; // final output of decoded stream
EncodedAudioStream dec(&out, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier(dec, url); // copy url to decoder

void connectToWifi()
{
  Serial.printf("Connecting to WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(Wifi_network, Wifi_password);
  unsigned long startAttemptTime = millis();

  while(WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < Timeout){
    Serial.printf(".");
    delay(100);
  }

  if(WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed!");
  }
  else
  {
    Serial.printf("Connected!");
    Serial.println(WiFi.localIP());
  }
}

WebSocketsServer webSocket = WebSocketsServer(80);
Servo myservo[2];


void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  switch(type){
    case WStype_DISCONNECTED:
    Serial.printf("{%u} Disconnected\n",num);
    break;

    case WStype_CONNECTED:
    {
      IPAddress ip=webSocket.remoteIP(num);
      Serial.printf("{%u} connected\n",num);
      Serial.printf("[%u] Connected from %d.%d.%d.%d url\n", num, ip[0], ip[1], ip[2], ip[3]);
    }
    break;

    case WStype_TEXT:
    Serial.printf("{%u} Text: %s\n",num,payload);
    for(size_t i=0;i<length;i++)
      {
        Serial.printf("{%u}",payload[i]);
        comm[ic]=payload[i];
        if((ic+1)<2)
        ic++;
        else
        {
          if(comm[ic]!=33)
          comm[ic]=0;
        }

      }
    break;
    case WStype_BIN :
    case WStype_FRAGMENT_TEXT_START :
    case WStype_FRAGMENT_BIN_START :
    case WStype_FRAGMENT :
    case WStype_FRAGMENT_FIN :
    default:
      break;
  }
}

void setup() 
{
  // put your setup code here, to run once:
  u8g2.begin();
  Serial.begin(115200);
  connectToWifi();
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  pinMode(2,OUTPUT);
  pinMode(motorpin1, OUTPUT);
  pinMode(motorpin2, OUTPUT);
  pinMode(motorpin3, OUTPUT);
  pinMode(motorpin4, OUTPUT);
  pinMode(motor_en, OUTPUT);
  myservo[0].attach(servopin[0]);
  myservo[1].attach(servopin[1]);
  myservo[0].write(curr_pos);
  myservo[1].write(curr_pos1);

  // setup out
  auto config = out.defaultConfig(TX_MODE);
  out.begin(config);

  // setup I2S based on sampling rate provided by decoder
  dec.begin();
  AudioToolsLogger.begin(Serial, AudioToolsLogLevel::Info);

}

void loop() 
{
  // put your main code here, to run repeatedly:
  if(override_exp==0){
    u8g2.clearBuffer();					// clear the internal memory
    u8g2.setFont(u8g2_font_inr38_mf);	// choose a suitable font
    u8g2.drawStr(17,y_position,expression);	// write something to the internal memory ^-^60 T-T T^T O-O ._.35 -_- X-X >w< >-< >3<
    u8g2.sendBuffer();
  }
  Serial.printf("%d",comm[0]);
  Serial.printf("%d",comm[1]);
  if(url)
  {
    Serial.printf(".\n");
  }
  else
  {
    Serial.printf("no\n");
    url.begin(urladdress,"audio/mp3");
  }
  if(WiFi.status()==WL_CONNECTED)
  {
    webSocket.loop();
    digitalWrite(2,HIGH);
    
    if(comm[0]==113 && curr_pos>=0 && curr_pos<180)
    {
      ++curr_pos;
      Serial.printf("%d\n",curr_pos);
      myservo[0].write(curr_pos);
      delay(20);
    }
    if(comm[0]==101 && curr_pos>0 && curr_pos<=180)
    {
      --curr_pos;
      Serial.printf("%d\n",curr_pos);
      myservo[0].write(curr_pos);
      delay(20);
    }
    if(comm[0]==114 && curr_pos1>0 && curr_pos1<=180)
    {
      --curr_pos1;
      Serial.printf("%d\n",curr_pos1);
      myservo[1].write(curr_pos1);
      delay(20);
    }
    if(comm[0]==102 && curr_pos1>=0 && curr_pos1<180)
    {
      ++curr_pos1;
      Serial.printf("%d\n",curr_pos1);
      myservo[1].write(curr_pos1);
      delay(20);
    }
    if(comm[0]==119)
    {
      //forward 'w'
      analogWrite(motor_en,speed);
      digitalWrite(motorpin1,HIGH);
      digitalWrite(motorpin2,LOW);
      digitalWrite(motorpin3,HIGH);
      digitalWrite(motorpin4,LOW);
      delay(10);
    }
    if(comm[0]==115)
    {
      //backward 's'
      analogWrite(motor_en,speed);
      digitalWrite(motorpin1,LOW);
      digitalWrite(motorpin2,HIGH);
      digitalWrite(motorpin3,LOW);
      digitalWrite(motorpin4,HIGH);
      delay(10);
    }
    if(comm[0]==100)
    {
      //right 'd'
      analogWrite(motor_en,speed);
      digitalWrite(motorpin1,HIGH);
      digitalWrite(motorpin2,LOW);
      digitalWrite(motorpin3,LOW);
      digitalWrite(motorpin4,HIGH);
      delay(10);
    }
    if(comm[0]==97)
    {
      //left 'a'
      analogWrite(motor_en,speed);
      digitalWrite(motorpin1,LOW);
      digitalWrite(motorpin2,HIGH);
      digitalWrite(motorpin3,HIGH);
      digitalWrite(motorpin4,LOW);
      delay(10);
    }
    if(comm[0]==49)
    {
      override_exp=0;
      expression[0]='^';
      expression[1]='-';
      expression[2]='^';
      y_position=60;
  
    }
    if(comm[0]==50)
    {
      override_exp=0;
      expression[0]='T';
      expression[1]='-';
      expression[2]='T';
      y_position=55;
    }
    if(comm[0]==51)
    {
      override_exp=0;
      expression[0]='T';
      expression[1]='^';
      expression[2]='T';
      y_position=55;
    }
    if(comm[0]==52)
    {
      override_exp=0;
      expression[0]='O';
      expression[1]='-';
      expression[2]='O';
      y_position=55;
    }
    if(comm[0]==53)
    {
      override_exp=0;
      expression[0]='.';
      expression[1]='_';
      expression[2]='.';
      y_position=35;
    
    }
    if(comm[0]==54)
    {
      override_exp=0;
      expression[0]='-';
      expression[1]='_';
      expression[2]='-';
      y_position=55;
      
    }
    if(comm[0]==55)
    {
      override_exp=0;
      expression[0]='X';
      expression[1]='-';
      expression[2]='X';
      y_position=55;

    }
    if(comm[0]==56)
    {
      override_exp=0;
      expression[0]='>';
      expression[1]='W';
      expression[2]='<';
      y_position=55;

    }
    if(comm[0]==57)
    {
      override_exp=0;
      expression[0]='>';
      expression[1]='-';
      expression[2]='<';
      y_position=55;
 
      
    }
    if(comm[0]==48)
    {
      override_exp=0;
      expression[0]='>';
      expression[1]='3';
      expression[2]='<';
      y_position=55;
      
    }
    if(comm[0]==116)
    {
      override_exp=1;
      Serial.printf("playing\n");
      u8g2.clearBuffer();
      u8g2.drawStr(17,55," - ");	
      u8g2.sendBuffer();					
      u8g2.clearBuffer();
      u8g2.drawStr(17,55," - ");
      u8g2.sendBuffer();
      u8g2.clearBuffer();
      u8g2.drawStr(17,55," o ");
      u8g2.sendBuffer();
      u8g2.clearBuffer();
      u8g2.drawStr(17,55," O ");
    }
    if(comm[0]==84)
    {
      copier.copy();
      if(url)
      {
        Serial.printf("audio: ");
      }
      else
      {
        Serial.printf("no\n");
        url.begin(urladdress,"audio/mp3");
        delay(1000);
      }
    }
    if(comm[1]==33 || comm[0]==33)
    {
      analogWrite(motor_en,0);
      override_exp=0;
      progress_load=1;
      ic=0;
      comm[0]=0;
      comm[1]=0;
      analogWrite(motor_en,0);
    }
  }
  else
  {
    override_exp=0;
    expression[0]='X';
    expression[1]='-';
    expression[2]='X';
    y_position=55;
    digitalWrite(2,HIGH);
    delay(100);
    digitalWrite(2,LOW);
    delay(100);
    connectToWifi();
  }
}
