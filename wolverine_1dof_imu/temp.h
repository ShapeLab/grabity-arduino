#ifndef _TEMP_H_
#define _TEMP_H_

#include <Arduino.h>

// Pin definitions
extern int myLed;

extern int motor1;
extern int motor2;
extern int motor3;

extern float als;

enum ALSGain {  // define lower nibble of ALS gain register
  alsGain20 = 0,  // ALS gain of 20
  alsGain10,      // ALS gain of 10.32
  alsGain5,       // ALS gain of 5.21
  alsGain2_5,     // ALS gain of 2.60
  alsGain1_67,    // ALS gain of 1.72
  alsGain1_25,    // ALS gain of 1.28
  alsGain1_0,     // ALS gain of 1.01
  alsGain40       // ALS gain of 40
};

extern int VL6180X_ADDRESS;  //  Default device address of VL6180X

enum VL6180XMode { contRangeMode = 0, contALSMode, interLeaveMode};

//uint8_t VL6180XMode = contALSMode;
//uint8_t VL6180XMode = contRangeMode;
extern uint8_t VL6180XMode;

//
extern uint8_t ALSGain;
extern float realalsGain;
#define verboseMode  false
extern int resetPins[];
extern uint8_t rangeValues[];
extern float alsValues[];
extern uint8_t addresses[];

#define SerialDebug true  // set to true to get Serial output for debugging

// Pin definitions
extern int intPin;  // These can be changed, 2 and 3 are the Arduinos ext int pins

extern uint16_t Pcal[8];         // calibration constants from MS5637 PROM registers
extern unsigned char nCRC;       // calculated check sum to ensure PROM integrity
extern uint32_t D1, D2;  // raw MS5637 pressure and temperature data
extern double dT, OFFSET, SENS, T2, OFFSET2, SENS2;  // First order and second order corrections for raw S5637 temperature and pressure data
extern int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
extern int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
extern int16_t magCount[3];    // Stores the 16-bit signed magnetometer sensor output
extern float magCalibration[];  // Factory mag calibration and mag bias
extern float gyroBias[], accelBias[], magBias[], magScale[];      // Bias corrections for gyro and accelerometer
extern int16_t tempCount;            // temperature raw count output
extern float   temperature;          // Stores the MPU9250 gyro internal chip temperature in degrees Celsius
extern double Temperature, Pressure; // stores MS5637 pressures sensor pressure and temperature
extern float SelfTest[6];            // holds results of gyro and accelerometer self test

// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
extern float GyroMeasError;   // gyroscope measurement error in rads/s (start at 40 deg/s)
extern float GyroMeasDrift;   // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
extern float beta;   // compute beta
extern float zeta;   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value
#define Kp 2.0f * 5.0f // these are the free parameters in the Mahony filter and fusion scheme, Kp for proportional feedback, Ki for integral
#define Ki 0.0f

extern uint32_t delt_t, count, sumCount;  // used to control display output rate
extern float pitch, yaw, roll;
extern float deltat, sum;          // integration interval for both filter schemes
extern uint32_t lastUpdate, firstUpdate; // used to calculate integration interval
extern uint32_t Now;                         // used to calculate integration interval

extern float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values
extern float *q;    // vector to hold quaternion
extern float eInt[];       // vector to hold integral error for Mahony method
/******************************** MPU9250 Define End *********************************/

#endif // _TEMP_H_
