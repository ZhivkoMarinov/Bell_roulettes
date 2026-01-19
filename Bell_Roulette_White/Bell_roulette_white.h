#ifndef BELL_ROULETTE_WHITE
#define BELL_ROULETTE_WHITE

#include <stdbool.h>
// Error(E) codes
#define EBNF "BALL_NOT_FOUND"
#define ECRN "CAN'T_READ_NUMBER"
#define EMNS "MOTOR_NOT_SPINNING"
#define EEM "EVENT MISSING"
#define PSF "POSSIBLE SENSOR FAULT"

// State(S) codes 
#define SNRS "NEW_ROUND_START"
#define SBTS "BETTING_TIME_START"
#define SBTE "BETTING_TIME_END"
#define SRS "RESULT"

const unsigned long maxRetrieveTime = 10000;
const unsigned long maxReadNumberTime = 60000;
const unsigned long lookingForBallDuration = 10000;
const unsigned long minShuffleTime = 50;
const unsigned long maxShuffleTime = 80;
const int shuffleTimeMultiplier = 100;
const unsigned int stopWheelDuration = 2900;
const int sectorLengthThreshold = 4;
const int readSectorDelay = 10;
const int fireBallCompressorTime = 1000;
const int fireBallMinTime = 0;
const int fireBallMaxTime = 30;
const unsigned long keepWheelUpDuration = 1800;
const unsigned long bettingTime = 3000 + lookingForBallDuration + keepWheelUpDuration + stopWheelDuration;
const int powerOnDelay = 2000;
const int delayBetweenACmotorRelays = 1000;
const int delayWheelDown = 500;
const int delayWheelUp = 100;
const int emergancyRetrieveDelay = 2000;
const int checkForBallDelay = 5000;
const int waitForWheelSpinning = 3000;
const int minPreBetDelay = 10;
const int maxPreBetDelay = 25;
const int multiplier = 100;
const int resetRoundThreshold = 10000;
const int nextRoundAwaitTime = 2000;

const int correctCountThreshold = 2;

const int winSensorPin = 2;
const int wheelSensorPin = 3;
const int controlWheelSensorPin = 4;
const int ballSensor = 5;
const int retrieveSensor = 6;

const int motorRotate = 9;
const int motorFast = 10;
const int ballFan = 11;
const int wheelLifter = 12;

typedef struct gameRound {
  int wheelSectorCounter;
  int ballLightCount;
  int sectorLengthCounter;
  bool isSectorCounted;
  bool statusWinSensor;
  bool statusWheelSensor;
  bool statusControlSensor;
  bool isNumberRead;
  int winningSector;
  int winningSectorCorrectCount;
  int winningNumber;
  int lastReadNumber;
} gameRound;

typedef struct wheelControl {
  bool isWheelRotating;
  bool isBallThere;
  bool isWheelUp;
  bool isRetrieved;
  bool error;
} wheel;

const int numbersArray[] = {
    23, 8, 30, 11, 36, 13, 27, 6, 34, 17, 25, 2, 21, 4, 19, 15, 32, 0, 26, 3, 35, 12, 28, 7, 29, 18, 22, 9, 31, 14, 20, 1, 33, 16, 24, 5, 10
};

#endif
