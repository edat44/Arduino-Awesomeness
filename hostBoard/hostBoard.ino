
#include <Wire.h>

#define DATA_SIZE 1 // how many bytes to expect from each I2C salve node
#define CONTESTANTS 2 // maximum number of slave nodes (I2C addresses) to probe
#define START_ADDRESS 1 // The starting I2C address of slave nodes
#define READ_DELAY 10 // Some delay between I2C node reads

#define BUZZ_OFFSET 0

enum GAME_STATE {
  question,
  answer,
  paused
};

volatile GAME_STATE gameState = question;



void setup()
{
  Serial.begin(9600);  
  Serial.println("MASTER HOST DEVICE");
  Serial.print("Number of Contestants: ");
  Serial.println(CONTESTANTS);
  
  Wire.begin();        // Activate I2C link
}

void loop()
{
  for (int address = START_ADDRESS; address <= CONTESTANTS; address++) { // we are starting from Node address 2
    Wire.requestFrom(address, DATA_SIZE);    // request data from node#
    if(Wire.available() == DATA_SIZE) {  // if data size is avaliable from nodes
      processData(address, Wire.read());    
    }
  }
  delay(READ_DELAY);
}

void processData(int address, byte data) {
  bool buzzIn;
  switch(gameState) {
    case question:
      buzzIn = bitRead(data, BUZZ_OFFSET);
      if (buzzIn) {
        Serial.print("Contestant ");
        Serial.print(address);
        Serial.println(" has buzzed in!");
        gameState = answer;
      }
      break;
    case answer:
      break;
    case paused:
      break;
    default:
      Serial.println("STATE NOT FOUND");
      break;
  }
}

