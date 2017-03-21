#include <Wire.h>

//Contestant setup
const int ADDRESSES[] = {8, 9};
const int CONTESTANTS = sizeof(contestIds)/sizeof(int);
const int DATA_SIZE = 1; // how many bytes to expect from each I2C salve node
const int READ_DELAY = 10; // Some delay between I2C node reads

//How to interpret contestant data (bit by bit)
const int BUZZ_OFFSET 0;

//Overarching game structure
enum GAME_STATE {
  question, //Waiting for contestant answer input
  answer,
  paused
};

const int CORRECT_ANSWER_BUTTON = 2;
const int WRONG_ANSWER_BUTTON = 3;

const int POINT_VALUE = A0;

//Variables to be changed in interrupts
volatile GAME_STATE gameState = question;
volatile int contestantBuzzIn = 0;
volatile int points[CONTESTANTS];

void setup()
{
  Serial.begin(9600);  
  Serial.println("MASTER HOST DEVICE");
  Serial.print("Number of Contestants: ");
  Serial.println(CONTESTANTS);
  Wire.begin();        // Activate I2C link

  //Set up correct and wrong answer buttons
  pinMode(CORRECT_ANSWER_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(CORRECT_ANSWER_BUTTON), correctAnswer, RISING);
  pinMode(WRONG_ANSWER_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(WRONG_ANSWER_BUTTON), wrongAnswer, RISING);

  //Initialize values
  for (int i = 0; i < CONTESTANTS; i++) {
    points[i] = 0;
  }
}

void loop()
{
  switch(gameState) {
    case question:
      for (int i = 0; i <= CONTESTANTS; i++) { // Read address in the array
        int address = ADDRESSES[i];
        Wire.requestFrom(address, DATA_SIZE);    // request data from node#
        if(Wire.available() == DATA_SIZE) {  // if data size is avaliable from nodes
          processData(address, Wire.read());    
        }
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
  delay(READ_DELAY);
}

void processData(int address, byte data) {
  bool buzzIn;
  buzzIn = bitRead(data, BUZZ_OFFSET);
  if (buzzIn) {
    Serial.print("Contestant ");
    Serial.print(address);
    Serial.println(" has buzzed in!");
    contestant = getContestant(address);
    gameState = answer;
  }
}

int getContestant(int address) {
  for (int i = 0; i < CONTESTANTS; i++) {
    if (address = ADDRESSES[i])
      return (i + 1);
  }
  return 0;
}

void correctAnswer() {
  judgeAnswer(true);
}

void wrongAnswer() {
  judgeAnswer(false);
}

void judgeAnswer(bool corrent) {
  if (gameState == answer && contestant > 0) {
    short points = analogRead(POINT_VALUE) * (correct ? 1 : -1);
    Wire.beginTransmission(ADDRESSES[contestant]);
    Wire.write(points, 2);
    Wire.endTransmission();
  }
  else {
    Serial.println("Cannot judge answer at this time");
  }
}

