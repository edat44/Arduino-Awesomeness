//Contest Board 2
//Main Author:
#include <Wire.h>

#define DATA_SIZE 1 //how many bytes to send with each I2C request
#define ADDRESS 2 //Address of slave I2C device

byte data[DATA_SIZE];

const int BUTTON=2; // const=can't be changed. 
                    // int=no decimal value 
const int BUZZER=9;

volatile bool buzz=0;

void setup() {
  
  Serial.begin(9600);
  pinMode(BUTTON,INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), buttonChange, CHANGE); 
  pinMode(BUZZER,OUTPUT);
  
}

void loop() {
  
Serial.println(buzz);
  
}

void buttonChange(){

  buzz=digitalRead(BUTTON);
  if (buzz){
    tone(BUZZER, 1000);
  }
  else {
    noTone(BUZZER);
  }

}


