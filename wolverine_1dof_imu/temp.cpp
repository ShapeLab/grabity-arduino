#include "temp.h"

// Pin definitions
int myLed = 13;

int motor1 = 6;
int motor2 = 22;
int motor3 = 21;

int VL6180X_ADDRESS = 0x29;  //  Default device address of VL6180X

//uint8_t VL6180XMode = contALSMode;
//uint8_t VL6180XMode = contRangeMode;
uint8_t VL6180XMode = interLeaveMode;

uint8_t ALSGain = alsGain20;
int resetPins[3] = {3, 4, 5};
uint8_t rangeValues[3] = {0};
float alsValues[3] = {0};
uint8_t addresses[3] = {0x30, 0x31, 0x32};

#define SerialDebug true  // set to true to get Serial output for debugging

// Pin definitions
int intPin = 8;  // These can be changed, 2 and 3 are the Arduinos ext int pins

uint32_t D1 = 0, D2 = 0;  // raw MS5637 pressure and temperature data
float magCalibration[3] = {0, 0, 0};  // Factory mag calibration and mag bias
float gyroBias[3] = {0, 0, 0}, accelBias[3] = {0, 0, 0}, magBias[3] = {0, 0, 0}, magScale[3]  = {0, 0, 0};      // Bias corrections for gyro and acceleromete

// global constants for 9 DoF fusion and AHRS (Attitude and Heading Reference System)
float GyroMeasError = PI * (40.0f / 180.0f);   // gyroscope measurement error in rads/s (start at 40 deg/s)
float GyroMeasDrift = PI * (0.0f  / 180.0f);   // gyroscope measurement drift in rad/s/s (start at 0.0 deg/s/s)
float beta = sqrt(3.0f / 4.0f) * GyroMeasError;   // compute beta
float zeta = sqrt(3.0f / 4.0f) * GyroMeasDrift;   // compute zeta, the other free parameter in the Madgwick scheme usually set to a small or zero value

uint32_t delt_t = 0, count = 0, sumCount = 0;  // used to control display output rate
float deltat = 0.0f, sum = 0.0f;          // integration interval for both filter schemes
uint32_t lastUpdate = 0, firstUpdate = 0; // used to calculate integration interval
uint32_t Now = 0;                         // used to calculate integration interval

float qat[4] = {1.0f, 0.0f, 0.0f, 0.0f};    // vector to hold quaternion
float * q = qat;
float eInt[3] = {0.0f, 0.0f, 0.0f};       // vector to hold integral error for Mahony method

float pitch, yaw, roll;
float ax, ay, az, gx, gy, gz, mx, my, mz; // variables to hold latest sensor data values

int16_t tempCount;            // temperature raw count output
float   temperature;          // Stores the MPU9250 gyro internal chip temperature in degrees Celsius
double Temperature, Pressure; // stores MS5637 pressures sensor pressure and temperature
float SelfTest[6];            // holds results of gyro and accelerometer self test

int16_t accelCount[3];  // Stores the 16-bit signed accelerometer sensor output
int16_t gyroCount[3];   // Stores the 16-bit signed gyro sensor output
int16_t magCount[3];    // Stores the 16-bit signed magnetometer sensor output
double dT, OFFSET, SENS, T2, OFFSET2, SENS2;  // First order and second order corrections for raw S5637 temperature and pressure data
uint16_t Pcal[8];         // calibration constants from MS5637 PROM registers
unsigned char nCRC;       // calculated check sum to ensure PROM integrity

float realalsGain;
float als;
