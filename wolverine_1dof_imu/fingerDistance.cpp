#include "fingerDistance.h"
#include "vl6180x.h"
#include <BasicLinearAlgebra.h>


namespace fingerDistance {
String serialInput = "";
String oldSerialInput = "";
int interfingerDistance = 50; // measured in millimeters, like the ToL sensor.
unsigned long lastTimeActivated = 0; // last time the motor was activated, in ms.
bool canBeReactivated = true;
float sampleTimes[sampleCount];
float sampleLengths[sampleCount];
}

float fingerDistance::getFingerDistance(uint8_t * rangeValues, uint8_t * addresses, int motor1) {
  // shift samples by one [if this actually causes lag, do some extra work and clean the code, really order in the matrix doesn't matter]
  for (int i = 0; i < sampleCount - 1; i++) {
    sampleTimes[i] = sampleTimes[i + 1];
    sampleLengths[i] = sampleLengths[i + 1];
  }

  for (int j = 0; j < 3; j++)
  {
    rangeValues[j] = readSensor(addresses[j]);
  }
  unsigned long now_ = millis();
  sampleTimes[sampleCount - 1] = now_;
  sampleLengths[sampleCount - 1] = 142 - rangeValues[0];

  const int sizeResult = 3;
  Matrix<sampleCount, sizeResult> X;
  Matrix<sampleCount> Y;
  for (int i = 0; i < sampleCount; i++) {
    float thisSample = sampleTimes[i] - now_;
    X(i, 0) = 1;
    X(i, 1) = thisSample;
    X(i, 2) = thisSample * thisSample;
    Y(i) = sampleLengths[i];
  }

  // Run Least Squares
  Matrix<sizeResult, sampleCount> Xt;
  Transpose(X, Xt);
  Matrix<sizeResult, sizeResult> XtX;
  Multiply(Xt, X, XtX);
  Invert(XtX);
  Matrix<sizeResult> abc;
  Multiply(XtX, Xt * Y, abc);
  //Serial << "ABC:" << abc << '\n';
  float predictedPosition = abc(2) * 100 + abc(1) * 10 + abc(0); // predict 10ms into the future

  // update interfinger target distances
  while (Serial.available()) {
    int inChar = Serial.read();
    if (isDigit(inChar)) {
      // convert the incoming byte to a char
      // and add it to the string:
      serialInput += (char)inChar;
    }
    // if you get a newline, print the string,
    // then the string's value:
    if (inChar == '\n') {
      interfingerDistance = serialInput.toInt();
      // clear the string for new input:
      oldSerialInput = serialInput;
      serialInput = "";
    }
  }

  if (predictedPosition < interfingerDistance && (millis() - lastTimeActivated) > 800 && canBeReactivated)
  {
    digitalWrite(motor1, 20);
    lastTimeActivated = millis();
    canBeReactivated = false;
  }
  if (millis() - lastTimeActivated > 100) {
    digitalWrite(motor1, LOW);
  }
  if (predictedPosition > interfingerDistance + 9) { // 7 is the hysteresis length, it's gotta go past 7 to be able to reactivate.
    canBeReactivated = true;
  }
  return predictedPosition;

}
