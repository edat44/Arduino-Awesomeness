#include <LiquidCrystal.h>

//Overarching game structure
enum GAME_STATE {
  question, //Waiting for contestant answer input
  answer,
  awardPoints,
  gameOver
};
const int DELAY = 5; //Delay in milliseconds between iteration of the main loop
const int PLAYERS = 2;

const int P1_BUTTON = 2;
const int P2_BUTTON = 3;

const int P1_BUZZ_PIN = 4;
const int P2_BUZZ_PIN = 7;

const int P1_SCORE_PINS[] = {5, 6};
const int P2_SCORE_PINTS[] = {8, 9};

const int CORRECT_ANSWER_PIN = 4;
const int WRONG_ANSWER_PIN = 5;
const int POINT_VALUE_PIN = A0;

//Variables to be changed in interrupts
volatile GAME_STATE gameState = question;
int buzzedInPlayer;
int points[2];


void setup()
{
  Serial.begin(9600);
  Serial.println("Welcome to the show!");

  // set up the LCD's number of columns and rows:

  //Set up correct and wrong answer buttons
  pinMode(CORRECT_ANSWER_PIN, INPUT);
  pinMode(WRONG_ANSWER_PIN, INPUT);

  pinMode(P1_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(P1_BUTTON), playerOneAnswer, RISING);
  pinMode(P2_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(P2_BUTTON), playerTwoAnswer, RISING);

  //Initialize points
  for (int i = 0; i < PLAYERS; i++) {
    points[i] = 0;
  }
}

void loop()
{
  switch(gameState) {
    case question:
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
  buzzIn(1);
}

void playerTwoAnswer() {
  buzzIn(2);
}

void buzzIn(int player) {
  if (gameState == question) {
    Serial.print("Player ");
    Serial.print(player);
    Serial.println(" buzzed in!");
    gameState = answer;
    buzzedPlayer = player;
  }
}

