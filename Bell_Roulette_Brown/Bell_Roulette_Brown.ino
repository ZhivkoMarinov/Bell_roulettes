#include "Bell_roulette_brown.h"

wheel wheelControl = {
    .isWheelRotating = false,
    .isBallThere = false,
    .isWheelUp = false,
    .isRetrieved = false,
    .error = false,
};

int roundAttemptsToRetrieve = 0;
unsigned long roundCounter = 1;
void(* resetFunc) (void) = 0;

void setup() {
  singleDelay(powerOnDelay);
  pinMode(winSensorPin, INPUT_PULLUP);
  pinMode(wheelSensorPin, INPUT_PULLUP);
  pinMode(controlWheelSensorPin, INPUT_PULLUP);
  pinMode(ballSensor, INPUT_PULLUP);
  pinMode(retrieveSensor, INPUT_PULLUP);

  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  pinMode(motorRotate, OUTPUT);
  pinMode(motorFast, OUTPUT);
  pinMode(wheelLifter, OUTPUT);
  pinMode(ballFan, OUTPUT);
  Serial.begin(9600);
  setWheel(&wheelControl);
}

void loop() {
  resetRoundID();
  playRound();
}

void playRound(){
  gameRound newGameRound = {
    .wheelSectorCounter = 0,
    .ballDetected = false,
    .sectorLengthCounter = 0,
    .isSectorCounted = false,
    .statusWinSensor = false,
    .statusWheelSensor = false,
    .statusControlSensor = false,
    .isNumberRead = false,
    .winningSector = -1,
    .winningSectorCorrectCount = 0,
    .winningNumber = -1,
    .lastReadNumber = -1,
    .ballCounter = 0
  };
  sendEvent(SNRS, -1);
  spinSlow(&wheelControl);
  randomDelay(minPreBetDelay, maxPreBetDelay);
  sendEvent(SBTS, -1);
  bettingTimeFunc(&wheelControl, &newGameRound);
  sendEvent(SBTE, -1);
  fireBall(&wheelControl, &newGameRound);
  shuffleWheel(&wheelControl);
  result(&newGameRound);
  if (!isBall()) {
    sendEvent(SRS, newGameRound.winningNumber);
  }
  else {
    sendError(PSF);
  }
  roundCounter++; 
}

void sendEvent(String event, int result) {
  Serial.print(roundCounter);
  Serial.print(";");
  Serial.print(event);
  Serial.print(";");
  if (event == "RESULT" && result != -1) {
    Serial.print(result);    
  }
  Serial.println();
}

volatile void sendError(String error) {
  while (true){
    if (!wheelControl.error){
      Serial.println(error);
      wheelControl.error = true;
      stopWheel(&wheelControl);
    }
    if (error != PSF) {
      break;
    }
    singleDelay(1000);
  }
  wheelControl.error = false;
  resetFunc();
}

void resetRoundID(){
  if (roundCounter == resetRoundThreshold + 1){
    resetFunc();  
  }
}

int randomTime(int min, int max){
  return random(min, max + 1);  
}

void randomDelay(int min, int max){
  int randTime = random(min, max + 1) * multiplier;
  unsigned long currentTime = millis();
  while(currentTime + randTime > millis());  
}

void singleDelay(long milliseconds){
  unsigned long currentTime = millis();
  while(currentTime + milliseconds > millis());
}

void spinSlow(wheel *wheelControl) {
  if (wheelControl->isWheelRotating == false) {
    digitalWrite(motorRotate, HIGH);
    wheelControl->isWheelRotating = true;  
  }
  else{
    digitalWrite(motorFast, LOW);
    digitalWrite(motorRotate, HIGH);
  }
}

void spinFast(wheel *wheelControl) {
  if (wheelControl->isWheelRotating == true) {
    digitalWrite(motorFast, HIGH);  
  }
}

void stopWheel(wheel *wheelControl) {
  if(wheelControl->isWheelRotating == true){
    digitalWrite(motorRotate, LOW);
    singleDelay(stopWheelDuration);    
    wheelControl->isWheelRotating = false;  
  }
}

void setWheel (wheel *wheelControl) {
  while(digitalRead(retrieveSensor)) {
    digitalWrite(wheelLifter, HIGH);  
  }
  digitalWrite(wheelLifter, LOW);
  singleDelay(2000);
  digitalWrite(wheelLifter, HIGH);
  singleDelay(600);
  digitalWrite(wheelLifter, LOW);
}

void wheelUp(wheel *wheelControl){
  
  if(wheelControl->isWheelUp == false) {
    while(digitalRead(retrieveSensor)) {
      digitalWrite(wheelLifter, HIGH);  
    }
    singleDelay(delayWheelUp);
    digitalWrite(wheelLifter, LOW);    
    wheelControl->isWheelUp = true;
  }
}

void wheelDown(wheel *wheelControl){
  if(wheelControl->isWheelUp) {
    digitalWrite(wheelLifter, HIGH);
    singleDelay(delayWheelDown);
    digitalWrite(wheelLifter, LOW);
    wheelControl->isWheelUp = false;
  }
}

void retrieveBall(wheel *wheelControl){
  stopWheel(wheelControl);
  wheelUp(wheelControl);
  singleDelay(keepWheelUpDuration);
  wheelDown(wheelControl);
  spinSlow(wheelControl);
}

void shuffleWheel(wheel *wheelControl){
  long shuffleDuration = randomTime(minShuffleTime, maxShuffleTime) * shuffleTimeMultiplier;
  //Serial.print("Shuffle randTime: ");
  //Serial.println(shuffleDuration);
  if (wheelControl->isWheelRotating){
    digitalWrite(motorFast, HIGH);
    singleDelay(shuffleDuration);
    digitalWrite(motorFast, LOW);
  }
}

void bettingTimeFunc(wheel *wheelControl, gameRound *gameRound){
  unsigned long currentTime = millis();
  retrieveBall(wheelControl);
  singleDelay(lookingForBallDuration);

  while(bettingTime + currentTime > millis()) {
    if (isBall() == 0) {
      roundAttemptsToRetrieve++;    
      if (roundAttemptsToRetrieve <= 3){
        bettingTimeFunc(wheelControl, gameRound);
      }
      else{
        sendError(EBNF);
      }
    }
    else {
      roundAttemptsToRetrieve = 0;
    }  
  }
}

int isBall(){
  if (digitalRead(ballSensor)) {
    return 1;  
  }
  return 0;
}

void fireBall(wheel *wheelControl, gameRound *gameRound) {
  int fireBallDuration = (randomTime(fireBallMinTime, fireBallMaxTime) * 10) + fireBallCompressorTime;
  //Serial.print("Fire randTime: ");
  //Serial.println(fireBallDuration);
  const unsigned long current_time = millis();
  while (current_time + fireBallDuration > millis()) {
    digitalWrite(ballFan, HIGH);
  }
  digitalWrite(ballFan, LOW);
}

void readBall(gameRound *gameRound){
  if(digitalRead(winSensorPin) == 0){
    gameRound->ballCounter++;
  }  
  else{
    if(gameRound->ballCounter > ballDetectionTimeThreshold){
      gameRound->ballDetected = true;  
    }
    gameRound->ballCounter = 0;
  }  
  delay(3);
}

void readSector(gameRound *gameRound, unsigned long currentTime){
  int counter = 0;
  gameRound->ballDetected = false;
  gameRound->isSectorCounted = false;
  bool isWinningBall = false;
  while(digitalRead(wheelSensorPin) == 0){
    counter++;
    delay(3);
  }
  
  if(counter > falseSignal){
    gameRound->wheelSectorCounter++;  
  }
  if(counter > sectorLengthThreshold){
    gameRound->wheelSectorCounter = 0;
  }
  readBall(gameRound);
}

void readNumber(gameRound *gameRound, unsigned long currentTime){
  bool successfulRead = false;
  while (!successfulRead){
    readSector(gameRound, currentTime);
    if (gameRound->wheelSectorCounter > 36 && gameRound->wheelSectorCounter % 36 == 0){
      gameRound->wheelSectorCounter = 36;
    }
    if (gameRound->ballDetected){      
      //Serial.print("Sector: ");
      //Serial.println(gameRound->wheelSectorCounter);      
      gameRound->winningNumber = numbersArray[gameRound->wheelSectorCounter];
      successfulRead = true;
      if(gameRound->lastReadNumber == -1){
        gameRound->lastReadNumber = gameRound->winningNumber;
      }
    }
  }
}

void result(gameRound *gameRound){
  unsigned long currentTime = millis();
  while(gameRound->winningSectorCorrectCount < correctCountThreshold){
    readNumber(gameRound, currentTime);
    if (gameRound->winningNumber != -1 && gameRound->winningNumber == gameRound->lastReadNumber){
      gameRound->winningSectorCorrectCount++;
    }
    else {
      gameRound->winningSectorCorrectCount = 0;
      gameRound->lastReadNumber = -1;
    }
  }  
}

volatile void sensorTest(){
  bool counted = false;
  int lastPrinted = -1;
  int prevSector = -1;
  int sectorCount = -1;
  bool winner = false;

  int counter = 0;
  while(true){
    while(digitalRead(winSensorPin) == 0){
    counter++;
    singleDelay(10);

    
    /*Serial.println(sectorCount);
    sectorCount = 0;

    int darkLength = 0;
    while (digitalRead(winSensorPin) == 0) {
      darkLength++;
      singleDelay(1);
    }
    Serial.println(darkLength);
    //Serial.println(numbersArray[sectorCount]);
    if (darkLength > 150) {
      //Serial.println(numbersArray[sectorCount]);
      Serial.println(darkLength);
    }
    Serial.println("------------------");*/
    }
    if(counter > 0){
      Serial.println(counter);
    }
    counter = 0;
  }
}
