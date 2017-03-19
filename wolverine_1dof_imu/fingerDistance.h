#ifndef _FINGER_DISTANCE_H_
#define _FINGER_DISTANCE_H_

#include <Arduino.h>


namespace fingerDistance {

extern String serialInput;
extern String oldSerialInput;
extern int interfingerDistance; // measured in millimeters, like the ToL sensor.
extern unsigned long lastTimeActivated; // last time the motor was activated, in ms.
extern bool canBeReactivated;
const int sampleCount = 15;
extern float sampleTimes[sampleCount];
extern float sampleLengths[sampleCount];

extern float getFingerDistance(uint8_t* rangeValues, uint8_t * addresses, int motor1);
}

#endif // _FINGER_DISTANCE_H_
