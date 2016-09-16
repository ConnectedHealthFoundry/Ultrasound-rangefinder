#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Q2HX711.h>
#include <WiFiClientSecure.h>
#include <M2XClient.h>
#include <SFClient.h>

#define ESP8266_PLATFORM

int distY1,distY2,baseY1,baseY2;
int sum = 0;
const int trpin = 5;//digital pin D1 for transmission start
const int s2 = 13;
const int s1 = 12;
const int s0 = 14;

/*
const char *sf_clientId = "3MVG9szVa2RxsqBYmyhpPA4kGtEL7I.wh71flKx52ksRF9P3XW3gf8._WQZ4P043LvETPpHE3UmggetzlS0Rf";
const char *sf_clientSecret = "2684527054782167756";
const char *sf_username = "james89@gmail.com";
const char *sf_password = "salesforce1";
const char *sf_securityToken = "PdYqfjS9H9PT0q2ZZSVN9XLl";
*/
const char *m2x_deviceId = "ccb067846803721b9ee611f650a9c9e2";
const char *m2x_key = "09ca7139261330fca25a584f24caa278";
const char *m2x_stream_Y1 = "ProximityY1";
const char *m2x_stream_Y2 = "ProximityY2";

char ssid[]     = "NetComm 0005";                     
char password[] = "Secazusuko";

WiFiClient client;
M2XClient m2xClient(&client, m2x_key);

//WiFiClientSecure tlsClient;
//SFClient sfClient(&tlsClient, sf_clientId, sf_clientSecret, sf_securityToken);

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Serial Open");

  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  // Log on to WiFi
  WiFi.begin(ssid, password);
  // Check WiFi status
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  // Print notification of WiFi connection success
  Serial.println("");
  Serial.println("Connected to wifi");
/*
  Serial.println("Connecting to salesforce");
  while(sfClient.connect() != 0)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to salesforce");

  int success = sfClient.authenticate(sf_username, sf_password);
  if(success == 0)
  {
    Serial.println("SF authentication success");
  }
  else
  {
    Serial.println("SF authentication failed");
  }
*/
int  success = m2xClient.connect();
  if(success == 0)
  {
    Serial.println("Connected to M2X");
  }
  else
  {
    Serial.println("Not connected to M2X");
  }

  // Assign analog multiplexer indexing pins
  pinMode(s2, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s0, OUTPUT);

  // Assign triger channel pin
  pinMode(trpin, OUTPUT);

  //Delay for sensor to calibrate
  delay(250);

  Serial.println("Obtaining baseline values....");
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  //Obtain baseline Y1
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;
  delay(100);
  }
  baseY1 = sum/10;
  sum = 0;
  Serial.print("Baseline for Y1:");
  Serial.println(baseY1);
  //Obtain baseline Y2
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;  
  delay(100);
  }
  baseY2 = sum/10;
  sum = 0;
  Serial.print("Baseline for Y2:");
  Serial.println(baseY2);
  delay(250);
}

void setupbaseY1(){
  sum=0;
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  //Obtain new baseline Y1
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;
  delay(100);
  }
  baseY1 = sum/10;
  sum = 0;
  Serial.print("New Baseline for Y1:");
  Serial.println(baseY1);
}

void setupbaseY2(){
  sum=0;
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  //Obtain new baseline Y2
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;
  delay(100);
  }
  baseY2 = sum/10;
  sum = 0;
  Serial.print("New Baseline for Y2:");
  Serial.println(baseY2);
}


void loop(){
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  
//Measure from sensor Y1
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);
  distY1 = (analogRead(A0)/2)*2.54;
  if (distY1<0.9*baseY1 || distY1>1.1*baseY1){
    Serial.print("Someone is...");
      Serial.print(distY1);
      Serial.println("(cm) away in Y1.");
      int response = m2xClient.updateStreamValue(m2x_deviceId, m2x_stream_Y1, distY1);
      setupbaseY1();
  }

//Measure from sensor Y2
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);
   distY2 = (analogRead(A0)/2)*2.54;
    if (distY2<0.9*baseY2 || distY2>1.1*baseY2){
    Serial.print("Someone is...");
      Serial.print(distY2);
      Serial.println("(cm) away in Y2.");
      int response = m2xClient.updateStreamValue(m2x_deviceId, m2x_stream_Y2, distY2);
      setupbaseY2();
  }

  Serial.print("Distance for Y1:");
  Serial.print(distY1);
  Serial.print("....");
  Serial.print("Distance for Y2:");
  Serial.println(distY2);


  
  delay(100);
}


