//FirebaseESP8266.h must be included before ESP8266WiFi.h
#include "FirebaseESP8266.h"  // Install Firebase ESP8266 library
#include <ESP8266WiFi.h>
#include <SPI.h>

#define FIREBASE_HOST "freq-synth-cicy-default-rtdb.firebaseio.com/" //Without http:// or https:// schemes
#define FIREBASE_AUTH "tlAYylZSMVf6yWQ0gLx8Qr8aljW3ZTHf5v48nmsT"
#define WIFI_SSID "IZZI-F66A"
#define WIFI_PASSWORD "FCAE34BEF66A"


#define CS 15
//#define Fout 9

#define M1_S0 16 
#define M1_S1 5 
//
#define M2_S0 4 
#define M2_S1 0 
#define M2_S2 2 
//
#define M3_S0 3

String fbDB = " ";
float fb = 0;
int fbDBsize = 0;
float fbP = 0; //N recibida anteriormente

int c1 = 0; //division control
int xa = 0;

float A[10] = {0,0,0,0,0,0,0,0,0,0};

String dDB = " ";
int dDBsize = 0;
int d = 0; //armonico elegido

float N = 0;
int N20 = 0;

int st = 0; //Stages flag
String Napply = " ";

byte ChipAdd = 0x9E;

byte DevAdd = 0x01;
byte DevData = 0x00; 

byte DevCAdd = 0x02;
byte DevCData = 0X00; 

byte DevConfAdd = 0x03;
byte DevConfData = 0x05; //65: x8 //0x05; //5: x1 //0x25 //25: x2;

byte GConfAdd = 0x05;
byte GConfData = 0x09; 
byte GConfFreeze = 0x01 ;

byte NINCRAdd = 0x86;
byte N6Add = 0x06;
byte N6Data = 0x00; //0x14;//0x00; 
byte N7Add = 0x07;
byte N7Data = 0x00; //0x50;//0x33; 
byte N8Add = 0x08;
byte N8Data = 0x00; //0x00;//0x33; 
byte N9Add = 0x09;
byte N9Data = 0x00; //0x00;//0x33; 

byte FunCINCRAdd = 96;
byte FunC1Add = 0x16;
byte FunC1Data = 0x90; //90: skipping mode enabled //0x10; 
byte FunC2Add = 0x17;
byte FunC2Data = 0x08; 
byte FunC3Add = 0x1E;
byte FunC3Data = 0x30; 

FirebaseData firebaseData;
FirebaseData ledData;
 
FirebaseJson json;

void setup() {
  Serial.begin(9600);
  pinMode(CS, OUTPUT);
  pinMode(M1_S0, OUTPUT);
  pinMode(M1_S1, OUTPUT);
  pinMode(M2_S0, OUTPUT);
  pinMode(M2_S1, OUTPUT);
  pinMode(M2_S2, OUTPUT);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  SPI.begin();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Serial.end();
  pinMode(M3_S0, OUTPUT);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  Firebase.setInt(firebaseData, "/c",1);
  delay(10);
  digitalWrite(CS, HIGH);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  //SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  delay(10);
  FunC3_init();
  DevC_init();
  FunC2_init();
  DevConf_init();
  GConf_init();
  FunC1_init();
  delay(50);
  N6_init();
  N7_init();
  N8_init();
  N9_init();
  delay(50);
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(GConfAdd);
  SPI.transfer(GConfFreeze);
  digitalWrite(CS, HIGH);
  //SPI.endTransaction();
  SPI.end();  
}

void fbUpdate()
{
  if (Firebase.getString(firebaseData, "/fb", fbDB))
  {
    fbDBsize=fbDB.length();
    fbDB.remove((fbDBsize-2), 2);
    fbDB.remove(0, 2);
    fb=fbDB.toFloat(); 
    if (fb != fbP)
    {
//      Serial.print("fb ");
//      Serial.println(fb);
      fbP=fb;
      if (fb>=5)
      {
        if(fb<38)
        {
          c1=2;
//          Serial.print("c1 ");
//          Serial.println(c1);
        }
        else if(fb<=1000)
        {
          c1=1;
//          Serial.print("c1 ");
//          Serial.println(c1);
        }
        else
        {
          c1=0;
//          Serial.print("c1 ");
//          Serial.println(c1);
        }
      }
      else
      {
        c1=0;
//        Serial.print("c1 ");
//        Serial.println(c1);
      }
      st=1;
//      Serial.println(st);
      if(st == 1){
      if (0<c1 && c1<3)
      {
        for (int i=1;i<11;i++)
        {
          A[i-1]=fb*i;
        }
        if (c1 == 2)
        {
          xa=1;
          digitalWrite(M3_S0, HIGH);
          while (A[xa-1]<37)
          {
            xa++;
          }
          fb=A[xa-1];
          switch (xa)
          {
            case 2:
              digitalWrite(M2_S2,LOW);
              digitalWrite(M2_S1,LOW);
              digitalWrite(M2_S0,LOW);
              break;
            case 3:
              digitalWrite(M2_S2,LOW);
              digitalWrite(M2_S1,LOW);
              digitalWrite(M2_S0,HIGH);
              break;
            case 4:
              digitalWrite(M2_S2,LOW);
              digitalWrite(M2_S1,HIGH);
              digitalWrite(M2_S0,LOW);
              break;
            case 5:
              digitalWrite(M2_S2,LOW);
              digitalWrite(M2_S1,HIGH);
              digitalWrite(M2_S0,HIGH);
              break;
            case 6:
              digitalWrite(M2_S2,HIGH);
              digitalWrite(M2_S1,LOW);
              digitalWrite(M2_S0,LOW);
              break;
            case 7:
              digitalWrite(M2_S2,HIGH);
              digitalWrite(M2_S1,LOW);
              digitalWrite(M2_S0,HIGH);
              break;
            case 8:
              digitalWrite(M2_S2,HIGH);
              digitalWrite(M2_S1,HIGH);
              digitalWrite(M2_S0,LOW);
              break;
            default:
              digitalWrite(M2_S2,HIGH);
              digitalWrite(M2_S1,HIGH);
              digitalWrite(M2_S0,HIGH);
              break;
          }
            Firebase.setInt(firebaseData,"/w",0);
            Firebase.setFloat(firebaseData,"/A1", A[0]);
            Firebase.setFloat(firebaseData,"/A2", A[1]);
            Firebase.setFloat(firebaseData,"/A3", A[2]);
            Firebase.setFloat(firebaseData,"/A4", A[3]);
            Firebase.setFloat(firebaseData,"/A5", A[4]);
            Firebase.setFloat(firebaseData,"/A6", A[5]);
            Firebase.setFloat(firebaseData,"/A7", A[6]);
            Firebase.setFloat(firebaseData,"/A8", A[7]);
            Firebase.setFloat(firebaseData,"/A9", A[8]);
            Firebase.setFloat(firebaseData,"/A10", A[9]);

//          Serial.println("Armonicos ");
//          for (int i=1;i<11;i++)
//          {
//            Serial.println(A[i-1]);
//          }
          
           for (int i=1;i<11;i++)
          {
            A[i-1]=fb*i;
          }
          c1=3;
        }
    
        if (c1==1)
        {
          digitalWrite(M3_S0, LOW);
          if (fb>100)
          {
            xa=1;
            while (A[xa-1]<=1000)
            {
              xa++;
            }
            Firebase.setInt(firebaseData,"/w1", 0);
            Firebase.setInt(firebaseData,"/w2",(xa-1));
            Firebase.setInt(firebaseData,"/w",1);
    
            for (int j=xa;j<11;j++)
            {
              A[j-1]=0;
            }
            
            Firebase.setFloat(firebaseData,"/A1", A[0]);
            Firebase.setFloat(firebaseData,"/A2", A[1]);
            Firebase.setFloat(firebaseData,"/A3", A[2]);
            Firebase.setFloat(firebaseData,"/A4", A[3]);
            Firebase.setFloat(firebaseData,"/A5", A[4]);
            Firebase.setFloat(firebaseData,"/A6", A[5]);
            Firebase.setFloat(firebaseData,"/A7", A[6]);
            Firebase.setFloat(firebaseData,"/A8", A[7]);
            Firebase.setFloat(firebaseData,"/A9", A[8]);
            Firebase.setFloat(firebaseData,"/A10", A[9]);
            
//            Serial.println("Armonicos ");
//            for (int i=1;i<xa;i++)
//            {              
//              Serial.println(A[i-1]);
//            }
          }
          else
          {
            Firebase.setInt(firebaseData,"/w",0);             
            Firebase.setFloat(firebaseData,"/A1", A[0]);
            Firebase.setFloat(firebaseData,"/A2", A[1]);
            Firebase.setFloat(firebaseData,"/A3", A[2]);
            Firebase.setFloat(firebaseData,"/A4", A[3]);
            Firebase.setFloat(firebaseData,"/A5", A[4]);
            Firebase.setFloat(firebaseData,"/A6", A[5]);
            Firebase.setFloat(firebaseData,"/A7", A[6]);
            Firebase.setFloat(firebaseData,"/A8", A[7]);
            Firebase.setFloat(firebaseData,"/A9", A[8]);
            Firebase.setFloat(firebaseData,"/A10", A[9]);

//            Serial.println("Armonicos ");
//            for (int i=1;i<11;i++)
//            {              
//              Serial.println(A[i-1]);
//            }
          }
          c1=3;
        }
        st=2;
//        Serial.println(st);
        if(st==2)
        {
          NUpdate();
        }
      }
      else 
      {
        for (int j=1;j<11;j++)
        {
          A[j-1]=0;
        }
        Firebase.setFloat(firebaseData,"/N", 0);
        Firebase.setFloat(firebaseData,"/A1", 0);
        Firebase.setFloat(firebaseData,"/A2", 0);
        Firebase.setFloat(firebaseData,"/A3", 0);
        Firebase.setFloat(firebaseData,"/A4", 0);
        Firebase.setFloat(firebaseData,"/A5", 0);
        Firebase.setFloat(firebaseData,"/A6", 0);
        Firebase.setFloat(firebaseData,"/A7", 0);
        Firebase.setFloat(firebaseData,"/A8", 0);
        Firebase.setFloat(firebaseData,"/A9", 0);
        Firebase.setFloat(firebaseData,"/A10", 0);
        Firebase.setInt(firebaseData,"/w1",1);
        Firebase.setInt(firebaseData,"/w",1);
        fbUpdate();
        c1=3; //
      }
    }
  }
}
}

void ApplyR()
{
  Firebase.setString(firebaseData, "/Napply","0");
  N20 = N * (1048576); //N*2^(20)
  //N value is assigned as bytes in hexadecimal
  N9Data = (byte) (N20 & 0xFF); 
  N8Data = (byte) ((N20 >> 8) & 0xFF);
  N7Data = (byte) ((N20 >> 16) & 0xFF);
  N6Data = (byte) ((N20 >> 24) & 0xFF);
//  Serial.println("HEX ");
//  Serial.println(N6Data, DEC);
//  Serial.println(N6Data, HEX);
//  Serial.println(N7Data, HEX);
//  Serial.println(N8Data, HEX);
//  Serial.println(N9Data, HEX);
  st=4;
  while (st == 4){ //stage 4, N value is sent to registers through SPI
    SPI.begin();
    delay(10);
    digitalWrite(CS, HIGH);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    delay(10);
    N6_init();
    N7_init();
    N8_init();
    N9_init();
    delay(50);
    digitalWrite(CS,LOW);
    SPI.transfer(ChipAdd);
    SPI.transfer(GConfAdd);
    SPI.transfer(GConfFreeze);
    digitalWrite(CS, HIGH);
    SPI.end();
    st=5;
  }   
  Firebase.setInt(firebaseData,"/t",1);
  NUpdate();   
}

void NUpdate()
{
  if (Firebase.getString(firebaseData, "/Napply",Napply))
  {
    while(Napply == "0")
    {
      if (Firebase.getString(firebaseData, "/d", dDB))
      {
        dDBsize=dDB.length();
        dDB.remove((fbDBsize-2), 2);
        dDB.remove(0, 2);
        d=dDB.toInt();
//        Serial.print("d ");
//        Serial.println(d);
        if (335<=A[d-1] && A[d-1]<=1000)
        {
          N=(A[d-1]*3000)/4485;
//          Serial.println("3000");
//          Serial.print("A[d-1] ");
//          Serial.println(A[d-1]);
//          Serial.print("N ");
//          Serial.println(N);
          digitalWrite(M1_S0,LOW);
          digitalWrite(M1_S1,LOW);
          Firebase.setFloat(firebaseData, "/N", N);
          Firebase.getString(firebaseData, "/Napply",Napply);
          fbUpdate(); 
        }
        else if (37<A[d-1] && A[d-1]<203)
        {
          N=(A[d-1]*27000)/4485;
//          Serial.println("27000");
//          Serial.print("A[d-1] ");
//          Serial.println(A[d-1]);
//          Serial.print("N ");
//          Serial.println(N);
          digitalWrite(M1_S0,HIGH);
          digitalWrite(M1_S1,LOW);
          Firebase.setFloat(firebaseData, "/N", N);
          Firebase.getString(firebaseData, "/Napply",Napply);
          fbUpdate(); 
        }
        else if (203<=A[d-1] && A[d-1]<335)
        {
          N=(A[d-1]*9000)/4485;
//          Serial.println("9000");
//          Serial.print("A[d-1] ");
//          Serial.println(A[d-1]);
//          Serial.print("N ");
//          Serial.println(N);      
          digitalWrite(M1_S0,LOW);
          digitalWrite(M1_S1,HIGH);
          Firebase.setFloat(firebaseData, "/N", N);
          Firebase.getString(firebaseData, "/Napply",Napply);
          fbUpdate(); 
        } 
        else {
          Firebase.setFloat(firebaseData, "/N", 0);
          Firebase.getString(firebaseData, "/Napply",Napply);
          fbUpdate(); 
        }
      }
    }
    st=3;
  }
//  Serial.println(st);
    if (st==3)
    {
      ApplyR();
    }
}

void loop() {
  fbUpdate(); 
}

int DevC_init(){
  digitalWrite(CS, LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(DevCAdd);
  SPI.transfer(DevCData);
  digitalWrite(CS, HIGH);
}

int DevConf_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(DevConfAdd);
  SPI.transfer(DevConfData);
  digitalWrite(CS, HIGH);
}

int GConf_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(GConfAdd);
  SPI.transfer(GConfData); 
  digitalWrite(CS, HIGH);
}

int N6_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(N6Add);
  SPI.transfer(N6Data);
  digitalWrite(CS, HIGH);
  Firebase.setString(firebaseData,"/N6",String(N6Data));
  Firebase.setString(firebaseData,"/N7",String(N7Data));
  Firebase.setString(firebaseData,"/N8",String(N8Data));
  Firebase.setString(firebaseData,"/N9",String(N9Data));
}

int N7_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(N7Add);
  SPI.transfer(N7Data);
  digitalWrite(CS, HIGH);
}

int N8_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(N8Add);
  SPI.transfer(N8Data);
  digitalWrite(CS, HIGH);
}

int N9_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(N9Add);
  SPI.transfer(N9Data);
  digitalWrite(CS, HIGH);
}

int FunC1_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(FunC1Add);
  SPI.transfer(FunC1Data);
  digitalWrite(CS, HIGH);
}

int FunC2_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(FunC2Add);
  SPI.transfer(FunC2Data);
  digitalWrite(CS, HIGH);
}

int FunC3_init(){
  digitalWrite(CS,LOW);
  SPI.transfer(ChipAdd);
  SPI.transfer(FunC3Add);
  SPI.transfer(FunC3Data);
  digitalWrite(CS, HIGH);
}
