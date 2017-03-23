#include <LiquidCrystal.h>

//Overarching game structure
enum GAME_STATE {
  question, //Waiting for contestant to buzz in (using interrupts)
  answer,   //Waiting for 'correct answer' or 'wrong answer' button to be pressed
  pause,    //Waiting for 'start next question button' to be pressed, nothing else happens in this state
  gameOver
};
volatile GAME_STATE gameState = pause; //Start off in the pause state

enum PLAYER {
  none = -1,
  playerOne = 0,
  playerTwo = 1
};

const int DELAY = 5; //Delay in milliseconds between iteration of the main loop
const int PLAYERS = 2; //Number of contestants playing

/*PIN CONSTANTS*/
const int P1_BUTTON = 2;
const int P2_BUTTON = 3;

const int P1_BUZZ_LED = 4;
const int P2_BUZZ_LED = 7;
const int BUZZ_LEDS[] = {P1_BUZZ_LED, P2_BUZZ_LED};

const int P1_SCORE_LEDS[] = {5, 6};
const int P2_SCORE_LEDS[] = {9, 10};

//Combines All Score LEDs into a single multidimensional array for easier processing
const int SCORE_LEDS[2][2] = {
  {P1_SCORE_LEDS[0], P1_SCORE_LEDS[1]},
  {P2_SCORE_LEDS[0], P2_SCORE_LEDS[1]}
};

const int CORRECT_ANSWER_BUTTON = 8;
const int WRONG_ANSWER_BUTTON = 11;
const int POINT_VALUE_POT = A0;

const int BUZZER = 12;
const int START_BUTTON = 13;
/*END PIN CONSTANTS*/

const int QUESTION_TIME = 5000; //Length of time (in milliseconds) that contestants get to answer each question
const int POINT_TARGET = 6; //Number of points to reach to win
const int POINTS_PER_LED = POINT_TARGET / 2; //number of points show by a full brightness score LED
const int MAX_POINT_VALUE = 3;

volatile PLAYER buzzedInPlayer = none; //Keep track of player that buzzed in for the last question
PLAYER winner = none; 
int points[2]; //Keep track of points for both players


/*BUZZER VARIABLES*/
volatile unsigned long buzzLength = 0;  //Length of time (in milliseconds) for buzzer to buzz
volatile unsigned long buzzRepeat = 0;  //Number of times to repeat buzzer sound (0 for no repeat)
volatile unsigned long buzzDelay = 0;   //Length of time (int millisecond) to delay between consecutive buzzes
volatile unsigned long buzzTone = 0;    //Frequency for buzzer sound (in Hz)
volatile unsigned long buzzTimer = 0;   //Current arduino time when buzzer is started
/*END BUZZER VARIABLES*/

unsigned long questionTimer = 0; //Current arduino time when question starts
unsigned long winnerTimer = 0; //Timer for flashing LED of winner

/*--------------------------------------------------------------------------------------------------------------*/

void setup() {
  Serial.begin(9600);
  Serial.println("Welcome to the show!");

  //SET UP HOST BUTTONS
  pinMode(CORRECT_ANSWER_BUTTON, INPUT);
  pinMode(WRONG_ANSWER_BUTTON, INPUT);
  pinMode(START_BUTTON,OUTPUT);

  //PLAYER BUZZ-IN BUTTONS
  pinMode(P1_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(P1_BUTTON), playerOneAnswer, RISING);
  pinMode(P2_BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(P2_BUTTON), playerTwoAnswer, RISING);

  //PLAYER BUZZ INDICATORS
  pinMode(P1_BUZZ_LED,OUTPUT);
  pinMode(P2_BUZZ_LED,OUTPUT);

  //PLAYER SCORE LEDS
  pinMode(P1_SCORE_LEDS[0],OUTPUT);
  pinMode(P1_SCORE_LEDS[1],OUTPUT);
  pinMode(P2_SCORE_LEDS[0],OUTPUT);
  pinMode(P2_SCORE_LEDS[1],OUTPUT);

  //BUZZER
  pinMode(BUZZER, OUTPUT);

  
  //INITIALIZE POINTS
  for (int i = 0; i < PLAYERS; i++) {
    points[i] = 0;
  }
  
  //Initial pause before starting the loop
  delay(200);
} //END setup FUNCTION

/*--------------------------------------------------------------------------------------------------------------*/

/*
 * loop()
 * Runs continuously while the arduino is powered on until a new program is loaded or the reset button is hit.
 * The gameState variable controls the overarching flow of the program.
 * Different code is run depending on the current state of the game. 
 */
void loop() {
  switch(gameState) {
    case question:
      questionState();
      break;
    case answer:
      answerState();
      break;
    case pause:
      pauseState();
      break;
    case gameOver:
      gameOverState();
      break;
    default:
      Serial.println("STATE NOT FOUND");
      break;
  }
  buzzer();
  delay(DELAY);
} //END loop FUNCTION

/*--------------------------------------------------------------------------------------------------------------*/
/*INTERRUPT FUNCTIONS*/
void playerOneAnswer() {
  buzzIn(PLAYER::playerOne);
}

void playerTwoAnswer() {
  buzzIn(PLAYER::playerTwo);
}

void buzzIn(PLAYER player) {
  if (gameState == question) {
    buzzTone = 800;
    buzzLength = 750;
    buzzTimer=millis();
    Serial.print("Player ");
    Serial.print(player);
    Serial.println(" buzzed in!");
    gameState = answer;
    buzzedInPlayer = player;
    int led = 0;
    if (player == playerOne) {
      led = P1_BUZZ_LED;
    } else {
      led = P2_BUZZ_LED;
    }
    digitalWrite(led, HIGH);
  }
} //END buzzIn FUNCTION

/*--------------------------------------------------------------------------------------------------------------*/

void buzzer() {
  if (buzzTone > 0) {
    tone(BUZZER, buzzTone);
  }
  if (buzzTimer > 0) {
    unsigned long newTime = millis();
    if ((newTime - buzzTimer) >= buzzLength) {
      noTone(BUZZER);
      if (buzzRepeat > 0) {
        delay(buzzDelay);
        buzzRepeat--;
        buzzTimer = millis();
      }
      else
        buzzTone = 0;
    }
  }
}

/*--------------------------------------------------------------------------------------------------------------*/

String getPlayerName(int player) {
  switch(player) {
    case PLAYER::playerOne:
      return "Player 1";
    case PLAYER::playerTwo:
      return "Player 2";
    default:
      return "UNKNOWN PLAYER";
  }
}

/*--------------------------------------------------------------------------------------------------------------*/

void questionState() {
  unsigned long newTime = millis();
  if (newTime  - questionTimer >= QUESTION_TIME) {
    gameState = pause;
    buzzTone = 1000;
    buzzLength = 750;
    buzzTimer = millis();
    Serial.println("Out of time!");
  }
}

/*--------------------------------------------------------------------------------------------------------------*/

void answerState() {
  questionTimer = 0;
  bool correct = digitalRead(CORRECT_ANSWER_BUTTON);
  bool wrong = digitalRead(WRONG_ANSWER_BUTTON);
  if (correct || wrong) {
    int pointValue = map(analogRead(POINT_VALUE_POT), 0, 1023, 1, MAX_POINT_VALUE);
    String playerName = getPlayerName(buzzedInPlayer);
    if (correct) {
      points[buzzedInPlayer] += pointValue;
      buzzLength = 500;
      buzzTone = 1600;
      buzzTimer = millis();
      Serial.print("Player ");
      Serial.print(playerName);
      Serial.println(" is correct!");
      Serial.print(" Plus ");
      Serial.print(pointValue);
      Serial.println(" points!");
      if (points[buzzedInPlayer] >= POINT_TARGET) {
        Serial.print("Player ");
        Serial.print(playerName);
        Serial.print(" has won with ");
        Serial.print(points[buzzedInPlayer]);
        Serial.println(" points!");
        winner = buzzedInPlayer;
        winnerTimer = millis();
        gameState = gameOver;
      }
      else
        gameState = pause;
    }
    else {
      points[buzzedInPlayer] = max(points[buzzedInPlayer] - pointValue, 0);
      buzzLength = 120;
      buzzRepeat = 1;
      buzzDelay = 70;
      buzzTone = 400;
      buzzTimer = millis();
      Serial.print("Player ");
      Serial.print(playerName);
      Serial.print(" is wrong!");
      Serial.print(" Minus ");
      Serial.print(pointValue);
      Serial.println(" points!");
      gameState = question;
      questionTimer = millis();
    }
    //Lightup Player Point LEDS
    for (int i = 0; i < 2; i++) {
      Serial.print("Player ");
      String tempPlayerName = getPlayerName(i);
      Serial.print(tempPlayerName);
      Serial.print(" has ");
      Serial.print(points[i]);
      Serial.println(" points!");
      int values[] = {0, 0};
      if (points[i] <= POINTS_PER_LED) {
        values[0] = map(points[i], 0, POINTS_PER_LED, 0, 255);
      } else {
        values[0] = 255;
        values[1] = map(points[i], POINTS_PER_LED, POINTS_PER_LED*2, 0, 255);
      }
      for (int j = 0; j < 2; j++) {
        analogWrite(SCORE_LEDS[i][j], values[j]);
      }
    }
    digitalWrite(P1_BUZZ_LED, LOW);
    digitalWrite(P2_BUZZ_LED, LOW);
  }
}

/*--------------------------------------------------------------------------------------------------------------*/

void pauseState() {
  bool start = digitalRead(START_BUTTON);
  if (start) {
    gameState = question;
    questionTimer = millis();
    Serial.println("Next question is starting");
  }
}

/*--------------------------------------------------------------------------------------------------------------*/

void gameOverState() {
  if (winner != none && winnerTimer > 0 && millis() - 1000 >= winnerTimer) {
    digitalWrite(BUZZ_LEDS[winner], !digitalRead(BUZZ_LEDS[winner]));
    winnerTimer = millis();
  }
}

