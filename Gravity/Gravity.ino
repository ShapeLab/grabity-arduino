// Simple DAC wave test on Teensy 3.1
#include "waves.h"
#include "output.h"
#include "ui.h"

int phaseIndex = 0;
elapsedMicros usec = 0;

// Default values
float vcAmplitude = 0.1;  // 1.0
float vcFrequency = 40.0; // 40.0
float plateauProportion = 0.30;

bool symmetricVibration = false;
bool inverseVibration = false;
bool torqueL = false;
bool torqueR = false;

int offsetPin1 = 37;
int offsetPin2 = 38;

unsigned long nextBrakeTriggerTime = 0;
bool motorLowIsScheduled = false;
unsigned long motorLowTriggerTime = 0;

int motorPin = 10;

#define encoder0PinA 22
#define encoder0PinB 23
volatile long encoder0Pos = 0;
int count = 0;
long printPos = 0;
bool toLock = false;

String serialReceptionString = "";

// http://stackoverflow.com/questions/9072320/split-string-into-string-array
String splitString(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

float encoderToGraspDistance(long encoder) {
  return 130.0 + 2.34*encoder;
}

void setup() {
  Serial.begin(19200);
  serialReceptionString.reserve(200);
  
  delay(2000);

  analogWriteResolution(12);
  analogWriteFrequency(offsetPin1, 14648);
  analogWriteFrequency(offsetPin2, 14648);

  pinMode(offsetPin1, OUTPUT);
  pinMode(offsetPin2, OUTPUT);
  pinMode(motorPin, OUTPUT);

  analogWrite(offsetPin1, 0xbff);
  analogWrite(offsetPin2, 0xbff);
  digitalWrite(motorPin, LOW);

  pinMode(encoder0PinA, INPUT);
  pinMode(encoder0PinB, INPUT);

  // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(22, doEncoderA, CHANGE);

  // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(23, doEncoderB, CHANGE);
}

void loop() {
  // Update the VC signal value.
  analogWrite(A21, getOutputSignalValueL(phaseIndex, plateauProportion, vcAmplitude, symmetricVibration, inverseVibration, torqueL, torqueR));
  analogWrite(A22, getOutputSignalValueR(phaseIndex, plateauProportion, vcAmplitude, symmetricVibration, inverseVibration, torqueL, torqueR));
  phaseIndex = phaseIndex + 1;

  // handle all the characters sent over serial
  if (phaseIndex >= WAVEFORM_RESOLUTION) {
    phaseIndex = 0;
  }
  
  /****Encoder****/
  if ((count & 0b11111111) == 0)
  {
    printPos = encoder0Pos;
    Serial.println (encoderToGraspDistance(printPos), DEC);
  }

  /****Brake****/
  if (toLock && (millis() > nextBrakeTriggerTime))
  {
    digitalWrite(motorPin, HIGH);
    motorLowTriggerTime = 150 + millis();
    motorLowIsScheduled = true;
    nextBrakeTriggerTime = 1000 + millis();
  }

  // millis trigger for the motor that drives the brake. Designed to avoid issues with delay()ing too long.
  if ((count & 0b1111111) == 0) {
    if (motorLowIsScheduled && (millis() > motorLowTriggerTime)) {
      digitalWrite(motorPin, LOW);
      motorLowIsScheduled = false;
    }
  }
  
  while (Serial.available()) {
    char readChar = Serial.read();
    if (readChar == '\n') {
      // process the full string: it is of the form forceL<tab>forceR<tab>distance

      float forceL = splitString(serialReceptionString, '\t', 0).toFloat();
      float forceR = splitString(serialReceptionString, '\t', 1).toFloat();
      float gripperForce = splitString(serialReceptionString, '\t', 2).toFloat();

      toLock = gripperForce > 0.0;

      float amp = -forceL / 5.0; // negative because

      vcAmplitude = amp > 1.0 ? 1.0 : (amp < -1.0 ? -1.0 : amp); // it's semi-difficult to go past this.

      serialReceptionString = "";
    }
    else {
      // add the character, wait for the newline to change everything.
      serialReceptionString += readChar;
    }
  }

  unsigned int waveformSegmentLength = (1000000 / (vcFrequency * WAVEFORM_RESOLUTION));
  while (usec < waveformSegmentLength) {
    ; // nothing is on this line except a semicolon and comment. it's a spin lock
  }
  usec = usec - waveformSegmentLength;
  
  count++;
}



void doEncoderA()
{
  // look for a low-to-high on channel A
  if (digitalRead(encoder0PinA) == HIGH)
  {
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == LOW)
    {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    else
    {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  else   // must be a high-to-low edge on channel A
  {
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinB) == HIGH)
    {
      encoder0Pos = encoder0Pos + 1;          // CW
    }
    else
    {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }

  // use for debugging - remember to comment out
}

void doEncoderB()
{
  // look for a low-to-high on channel B
  if (digitalRead(encoder0PinB) == HIGH)
  {
    // check channel A to see which way encoder is turning
    if (digitalRead(encoder0PinA) == HIGH)
    {
      encoder0Pos = encoder0Pos + 1;         // CW
    }
    else
    {
      encoder0Pos = encoder0Pos - 1;         // CCW
    }
  }

  // Look for a high-to-low on channel B

  else
  {
    // check channel B to see which way encoder is turning
    if (digitalRead(encoder0PinA) == LOW)
    {
      encoder0Pos = encoder0Pos + 1;          // CW
    }
    else
    {
      encoder0Pos = encoder0Pos - 1;          // CCW
    }
  }
}
