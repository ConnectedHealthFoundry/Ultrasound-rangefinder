#include <ESP8266WiFi.h>
#define ESP8266_PLATFORM

long pulse, distraw, inches, cm;
int sum = 0;
int avgrange = 50;
const int pwPin = 5;//digital pin D1

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(pwPin,INPUT);
  }

void loop() {
  //Using digital pulsewidth
pulse = pulseIn(pwPin,HIGH);
inches = pulse/147;
cm = inches * 2.54;
Serial.println("Digital PW Distance:");
Serial.print("Inches:");
Serial.println(inches);
Serial.print("Centimeters:");
Serial.println(cm);
Serial.println("");

  //Using analog
  for (int i=0;i<avgrange;i++)
  {
    distraw = analogRead(A0)/2;
    sum+=distraw;
    delay(10);
  }
  inches = sum/avgrange;
  cm = inches *2.54;
  Serial.println("Analog Distance:");
  Serial.print("Inches:");
  Serial.println(inches);
  Serial.print("Centimeters:");
  Serial.println(cm);
  Serial.println("");
  
  sum = 0;
  delay(500);
}
