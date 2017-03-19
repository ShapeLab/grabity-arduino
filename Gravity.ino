// Simple DAC wave test on Teensy 3.1
#include "waves.h"
#include "output.h"
#include "ui.h"

int phaseIndex = 0;
elapsedMicros usec = 0;

// Default values
float vcAmplitude = 0.5;  // 1.0
float vcFrequency = 40.0; // 40.0
float plateauProportion = 0.30;

bool symmetricVibration = false;
bool inverseVibration = false;
bool torqueL = false;
bool torqueR = false;

int offsetPin1 = 37;
int offsetPin2 = 38;

long nextBrakeTriggerTime = 0;
bool motorLowIsScheduled = false;
long motorLowTriggerTime = 0;

int motorPin = 10;

#define encoder0PinA 22
#define encoder0PinB 23
volatile long encoder0Pos = 0;
int count = 0;
long printPos = 0;

void setup() {
  Serial.begin(19200);
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
    while (Serial.available()) {
      char readChar = Serial.read();
      switch (readChar) {
        // PLATEAU PROPORTION
        case '4':
          plateauProportion += PLATEAU_RESOLUTION;
          if (plateauProportion > PLATEAU_MAX) plateauProportion = PLATEAU_MAX;
          break;
        case '1':
          plateauProportion -= PLATEAU_RESOLUTION;
          if (plateauProportion < PLATEAU_MIN) plateauProportion = PLATEAU_MIN;
          break;
        // AMPLITUDE
        case '5':
          vcAmplitude += AMPLITUDE_RESOLUTION;
          if (vcAmplitude > AMPLITUDE_MAX) vcAmplitude = AMPLITUDE_MAX;
          break;
        case '2':
          vcAmplitude -= AMPLITUDE_RESOLUTION;
          if (vcAmplitude < AMPLITUDE_MIN) vcAmplitude = AMPLITUDE_MIN;
          break;
        // FREQUENCY
        case '6':
          vcFrequency += FREQUENCY_RESOLUTION;
          if (vcFrequency > FREQUENCY_MAX) vcFrequency = FREQUENCY_MAX;
          break;
        case '3':
          vcFrequency -= FREQUENCY_RESOLUTION;
          if (vcFrequency < FREQUENCY_MIN) vcFrequency = FREQUENCY_MIN;
          break;
        case '0':
          symmetricVibration = !symmetricVibration;
          break;
        case '7':
          inverseVibration = !inverseVibration;
          break;
        case '8':
          torqueL = !torqueL;
          break;
        case '9':
          torqueR = !torqueR;
          break;
        default:
          continue;
      };
      printSignalStatus(plateauProportion, vcAmplitude, vcFrequency, symmetricVibration, inverseVibration, torqueL, torqueR);
    }
  }
  int waveformSegmentLength = (1000000 / (vcFrequency * WAVEFORM_RESOLUTION));
  while (usec < waveformSegmentLength) {
    ; // nothing is on this line except a semicolon and comment. it's a spin lock
  }
  usec = usec - waveformSegmentLength;

  /****Brake****/
  if (printPos < -10 && (millis() > nextBrakeTriggerTime))
  {
    digitalWrite(motorPin, HIGH);
    motorLowTriggerTime = 150 + millis();
    motorLowIsScheduled = true;
    nextBrakeTriggerTime = 1000 + millis();
  }

  /****Encoder****/
  if ((count % 100) == 0)
  { printPos = encoder0Pos;
    Serial.println (printPos, DEC);
  }

  // millis trigger for the motor that drives the brake. Designed to avoid issues with delay()ing too long.
  if ((count % 100) == 0) {
    Serial.println("Checking motor trigger....");
    Serial.println(motorLowIsScheduled);
    Serial.println(millis());
    Serial.println(motorLowTriggerTime);
    if (motorLowIsScheduled && (millis() > motorLowTriggerTime)) {
      digitalWrite(motorPin, LOW);
      motorLowIsScheduled = false;
      Serial.println("---Writing MotorPin low---");
    }
  }
  
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
