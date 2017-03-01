//Contest Board 1
//Main Author:

#include <Wire.h>

#define DATA_SIZE 1 //how many bytes to send with each I2C request
#define ADDRESS 1 //Address of slave I2C device

byte data[DATA_SIZE];

void setup() {
  Wire.begin(ADDRESS);  // Activate I2C network
  Wire.onRequest(requestEvent); // When data requested
  Wire.onReceive(receiveEvent); // When data received
}

void loop() {
}

void requestEvent() {
  Wire.write(data,DATA_SIZE);
}

void receiveEvent(int bytes) {
  
}

