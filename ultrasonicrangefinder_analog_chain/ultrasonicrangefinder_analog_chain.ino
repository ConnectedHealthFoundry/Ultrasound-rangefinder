#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <Q2HX711.h>
#include <WiFiClientSecure.h>
#include <M2XClient.h>
#include <SFClient.h>

#define ESP8266_PLATFORM

int distx1,distx2,basex1,basex2;
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
const char *m2x_stream_x1 = "ProximityX1";
const char *m2x_stream_x2 = "ProximityX2";

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
  //Obtain baseline x1
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;
  delay(100);
  }
  basex1 = sum/10;
  sum = 0;
  Serial.print("Baseline for x1:");
  Serial.println(basex1);
  //Obtain baseline x2
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;  
  delay(100);
  }
  basex2 = sum/10;
  sum = 0;
  Serial.print("Baseline for x2:");
  Serial.println(basex2);
  delay(250);
}

void setupbasex1(){
  sum=0;
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  //Obtain new baseline x1
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;
  delay(100);
  }
  basex1 = sum/10;
  sum = 0;
  Serial.print("New Baseline for x1:");
  Serial.println(basex1);
}

void setupbasex2(){
  sum=0;
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  //Obtain new baseline x2
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);
  for(int i=0;i<10;i++){
  sum+= (analogRead(A0)/2)*2.54;
  delay(100);
  }
  basex2 = sum/10;
  sum = 0;
  Serial.print("New Baseline for x2:");
  Serial.println(basex2);
}


void loop(){
  //Initiate Ranging
  digitalWrite(trpin, HIGH);
  delay(20);
  digitalWrite(trpin, LOW);
  
//Measure from sensor x1
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, LOW);
  distx1 = (analogRead(A0)/2)*2.54;
  if (distx1<0.9*basex1 || distx1>1.1*basex1){
    Serial.print("Someone is...");
      Serial.print(distx1);
      Serial.println("(cm) away in X1.");
      int response = m2xClient.updateStreamValue(m2x_deviceId, m2x_stream_x1, distx1);
      setupbasex1();
  }

//Measure from sensor x2
  digitalWrite(s2, LOW);
  digitalWrite(s1, LOW);
  digitalWrite(s0, HIGH);
   distx2 = (analogRead(A0)/2)*2.54;
    if (distx2<0.9*basex2 || distx2>1.1*basex2){
    Serial.print("Someone is...");
      Serial.print(distx2);
      Serial.println("(cm) away in X2.");
      int response = m2xClient.updateStreamValue(m2x_deviceId, m2x_stream_x2, distx2);
      setupbasex2();
  }

  Serial.print("Distance for x1:");
  Serial.print(distx1);
  Serial.print("....");
  Serial.print("Distance for x2:");
  Serial.println(distx2);


  
  delay(100);
}


