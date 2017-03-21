#include <LiquidCrystal.h>

//Overarching game structure
enum GAME_STATE {
  question, //Waiting for contestant answer input
  answer,
  awardPoints,
  gameOver
};
const int DELAY = 1000; //Delay in milliseconds between iteration of the main loop
const int PLAYERS = 2;

const int PLAYER_ONE_PIN = 2;
const int PLAYER_TWO_PIN = 3;
const int CORRECT_ANSWER_PIN = 4;
const int WRONG_ANSWER_PIN = 5;
const int POINT_VALUE_PIN = A0;

//Variables to be changed in interrupts
volatile GAME_STATE gameState = question;
int buzzedInPlayer;
int points[2];

LiquidCrystal lcd(11, 12, 4, 5, 6, 7);


void setup()
{
  Serial.begin(9600);
  Serial.println("Welcome to the show!");

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  //Set up correct and wrong answer buttons
  pinMode(CORRECT_ANSWER_PIN, INPUT);
  pinMode(WRONG_ANSWER_PIN, INPUT);

  pinMode(PLAYER_ONE_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PLAYER_ONE_PIN), playerOneAnswer, RISING);
  pinMode(PLAYER_TWO_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PLAYER_TWO_PIN), playerTwoAnswer, RISING);

  //Initialize points
  for (int i = 0; i < PLAYERS; i++) {
    points[i] = 0;
  }
  delay(1000);
}

void loop()
{
  lcd.clear();
  switch(gameState) {
    case question:
      displayPoints();
      gameState = answer;
      break;
    case answer:
      break;
    case awardPoints:
      break;
    case gameOver:
      break;
    default:
      Serial.println("STATE NOT FOUND");
      break;
  }
  delay(DELAY);
}

void playerOneAnswer() {
  
}

void playerTwoAnswer() {
  
}

void displayPoints() {
  lcd.home();
  lcd.clear();
  for (int i = 0; i < PLAYERS; i++) {
    lcd.setCursor(1, i);
    Serial.println(points[i]);
    lcd.print(5.0, 1);
  }
}

