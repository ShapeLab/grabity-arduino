#include <Arduino.h>
#include "ui.h"

void printSignalStatus(float plateauProportion, float vcAmplitude, float vcFrequency, bool symmetricVibration, bool inverseVibration, bool torqueL, bool torqueR) {
  Serial.println();
  //Serial.println("PP\tAMP\tFREQ");

  if (symmetricVibration) {
    Serial.print("SYM");
  }
  else {
    Serial.print("-");
  }
  Serial.print("\t");
  if (inverseVibration) {
    Serial.print("INV");
  }
  else {
    Serial.print("-");
  }
  Serial.print("\t");
  if (torqueL) {
    Serial.print("TQL");
  }
  else if (torqueR) {
    Serial.print("TQR");
  }
  else {
    Serial.print("-");
  }
  Serial.println();
  Serial.println("PP\tAMP\tFREQ");
  Serial.print(plateauProportion);
  Serial.print("\t");
  Serial.print(vcAmplitude);
  Serial.print("\t");
  Serial.print(vcFrequency);
  Serial.println();
  //Serial.println("+4 -1\t+5 -2\t+6 -3");
}
