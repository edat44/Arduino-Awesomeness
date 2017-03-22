#include <LiquidCrystal.h>

//Overarching game structure
enum GAME_STATE {
  question, //Waiting for contestant answer input
  answer,
  pause,
  gameOver
};

const int DELAY = 5; //Delay in milliseconds between iteration of the main loop
const int PLAYERS = 2;

const int P1_BUTTON = 2;
const int P2_BUTTON = 3;

const int P1_BUZZ_LED = 4;
const int P2_BUZZ_LED = 7;

const int P1_SCORE_LEDS[] = {5, 6};
const int P2_SCORE_LEDS[] = {9, 10};

const int TIMER_RED_LED = 3;
//const int TIMER_GREEN_LED = 11;

const int CORRECT_ANSWER_BUTTON = 8;
const int WRONG_ANSWER_BUTTON = 11;
const int POINT_VALUE_POT = A0;

const int BUZZER = 12;
const int START_BUTTON = 13;

const int BUZZ_TIME = 750;
const int QUESTION_TIME = 5000;

//Variables to be changed in interrupts
volatile GAME_STATE gameState = pause;
int buzzedInPlayer;
int points[2];

volatile unsigned long buzzTimer = 0;
unsigned long questionTimer = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Welcome to the show!");

  // set up the LCD's number of columns and rows:

  //Set up correct and wrong answer buttons
  pinMode(CORRECT_ANSWER_BUTTON, INPUT);
  pinMode(WRONG_ANSWER_BUTTON, INPUT);

  pinMode(P1_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(P1_BUTTON), playerOneAnswer, RISING);
  pinMode(P2_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(P2_BUTTON), playerTwoAnswer, RISING);
  
  pinMode(P1_BUZZ_LED,OUTPUT);
  pinMode(P2_BUZZ_LED,OUTPUT);
  
  pinMode(P1_SCORE_LEDS[0],OUTPUT);
  pinMode(P1_SCORE_LEDS[1],OUTPUT);

  pinMode(P2_SCORE_LEDS[0],OUTPUT);
  pinMode(P2_SCORE_LEDS[1],OUTPUT);

  pinMode(BUZZER, OUTPUT);
  pinMode(START_BUTTON,OUTPUT);
  
  //Initialize points
  for (int i = 0; i < PLAYERS; i++) {
    points[i] = 0;
  }
  delay(200);
}

void loop()
{
  /*
  digitalWrite(P1_BUZZ_LED,HIGH);
  digitalWrite(P2_BUZZ_LED,HIGH);

  digitalWrite(P1_SCORE_LEDS[0],HIGH);
  digitalWrite(P1_SCORE_LEDS[1],HIGH);

  digitalWrite(P2_SCORE_LEDS[0],HIGH);
  digitalWrite(P2_SCORE_LEDS[1],HIGH);

  tone(BUZZER,1000);
  */
  bool correct = false, wrong = false, start = false, p1 = false, p2 = false;
  unsigned long newTime = 0, timeLeft = 0;
  switch(gameState) {
    case question:
      newTime = millis();
      if (newTime >= questionTimer + QUESTION_TIME) {
        tone(BUZZER, 1000);
        gameState = pause;
        buzzTimer = millis();
      }
      break;
    case answer:
      questionTimer = 0;
      correct = digitalRead(CORRECT_ANSWER_BUTTON);
      wrong = digitalRead(WRONG_ANSWER_BUTTON);
      if (correct || wrong) {
        int pointValue = map(analogRead(POINT_VALUE_POT), 0, 1023, 1, 3);
        if (correct) {
          points[buzzedInPlayer - 1] += pointValue;
        }
        else {
          points[buzzedInPlayer - 1] = max(points[buzzedInPlayer - 1], 0);
        }
        digitalWrite(P1_BUZZ_LED, LOW);
        digitalWrite(P2_BUZZ_LED, LOW);
        gameState = pause;
      }
      break;
    case pause:
      start = digitalRead(START_BUTTON);
      if (start) {
        gameState = question;
        questionTimer = millis();
      }
      break;
    case gameOver:
      break;
    default:
      Serial.println("STATE NOT FOUND");
      break;
  }
  if (buzzTimer > 0) {
    unsigned long newTime = millis();
    if ((newTime - buzzTimer) >= BUZZ_TIME) {
      noTone(BUZZER);
      buzzTimer = 0;
    }
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
    tone(BUZZER,800);
    buzzTimer=millis();
    Serial.print("Player ");
    Serial.print(player);
    Serial.println(" buzzed in!");
    gameState = answer;
    buzzedInPlayer = player;
    int led = 0;
    if (player == 1) {
      led = P1_BUZZ_LED;
    } else {
      led = P2_BUZZ_LED;
    }
    digitalWrite(led, HIGH);
  }
}

