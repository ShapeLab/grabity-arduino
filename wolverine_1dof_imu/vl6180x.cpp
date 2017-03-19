#include <i2c_t3.h>
#include "temp.h"
#include "vl6180x.h"

// I2C read/write functions for the VL6180X sensor
// VL6180X has 16-bit register addresses, so the MSB of the register is sent first, then the LSB

void writeByteVL(uint8_t address, uint16_t subAddress, uint8_t data)
{
  Wire.beginTransmission(address);      // Initialize the Tx buffer
  Wire.write((subAddress >> 8) & 0xFF); // Put MSB of 16-bit slave register address in Tx buffer
  Wire.write(subAddress & 0xFF);        // Put LSB of 16-bit slave register address in Tx buffer
  Wire.write(data);                     // Put data in Tx buffer
  Wire.endTransmission();               // Send the Tx buffer
}

uint8_t readByteVL(uint8_t address, uint16_t subAddress)
{
  uint8_t data; // `data` will store the register data
  Wire.beginTransmission(address);         // Initialize the Tx buffer
  Wire.write((subAddress >> 8) & 0xFF);    // Put MSB of 16-bit slave register address in Tx buffer
  Wire.write(subAddress & 0xFF);           // Put LSB of 16-bit slave register address in Tx buffer
  Wire.endTransmission(I2C_NOSTOP);        // Send the Tx buffer, but send a restart to keep connection alive
  //  Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
  //  Wire.requestFrom(address, 1);  // Read one byte from slave register address
  Wire.requestFrom(address, (size_t) 1);   // Read one byte from slave register address
  data = Wire.read();                      // Fill Rx buffer with result
  return data;                             // Return data read from slave register
}

void readByteVLs(uint8_t address, uint16_t subAddress, uint8_t count, uint8_t * dest)
{
  Wire.beginTransmission(address);      // Initialize the Tx buffer
  Wire.write((subAddress >> 8) & 0xFF); // Put MSB of 16-bit slave register address in Tx buffer
  Wire.write(subAddress & 0xFF);        // Put LSB of 16-bit slave register address in Tx buffer
  Wire.endTransmission(I2C_NOSTOP);     // Send the Tx buffer, but send a restart to keep connection alive
  //  Wire.endTransmission(false);          // Send the Tx buffer, but send a restart to keep connection alive
  uint8_t i = 0;
  //        Wire.requestFrom(address, count);   // Read bytes from slave register address
  Wire.requestFrom(address, (size_t) count);  // Read bytes from slave register address
  while (Wire.available()) {
    dest[i++] = Wire.read();
  }            // Put read results in the Rx buffer
}


void initVL6180X(uint8_t address) {
  uint8_t reset = readByteVL(address, VL6180X_SYSTEM_FRESH_OUT_OF_RESET);  // read fresh_out_of_reset bit
  if (reset == 1) { // if if fresh_out_of_reset bit set, then device has been freshly initialized

    writeByteVL(address, 0x0207, 0x01);
    writeByteVL(address, 0x0208, 0x01);
    writeByteVL(address, 0x0096, 0x00);
    writeByteVL(address, 0x0097, 0xFD);
    writeByteVL(address, 0x00e3, 0x00);
    writeByteVL(address, 0x00e4, 0x04);
    writeByteVL(address, 0x00e5, 0x02);
    writeByteVL(address, 0x00e6, 0x01);
    writeByteVL(address, 0x00e7, 0x03);
    writeByteVL(address, 0x00f5, 0x02);
    writeByteVL(address, 0x00d9, 0x05);
    writeByteVL(address, 0x00db, 0xce);
    writeByteVL(address, 0x00dc, 0x03);
    writeByteVL(address, 0x00dd, 0xf8);
    writeByteVL(address, 0x009f, 0x00);
    writeByteVL(address, 0x00a3, 0x3c);
    writeByteVL(address, 0x00b7, 0x00);
    writeByteVL(address, 0x00bb, 0x3c);
    writeByteVL(address, 0x00b2, 0x09);
    writeByteVL(address, 0x00ca, 0x09);
    writeByteVL(address, 0x0198, 0x01);
    writeByteVL(address, 0x01b0, 0x17);
    writeByteVL(address, 0x01ad, 0x00);
    writeByteVL(address, 0x00ff, 0x05);
    writeByteVL(address, 0x0100, 0x05);
    writeByteVL(address, 0x0199, 0x05);
    writeByteVL(address, 0x01a6, 0x1b);
    writeByteVL(address, 0x01ac, 0x3e);
    writeByteVL(address, 0x01a7, 0x1f);
    writeByteVL(address, 0x0030, 0x00);

    // Configure range measurement for low power
    // Specify range measurement interval in units of 10 ms from 0 (= 10 ms) - 254 (= 2.55 s)
    writeByteVL(address, VL6180X_SYSRANGE_INTERMEASUREMENT_PERIOD, 0x02); // 100 ms interval in steps of 10 ms
    writeByteVL(address, VL6180X_SYSRANGE_VHV_REPEAT_RATE, 0xFF); // sets number of range measurements after which autocalibrate is performed
    writeByteVL(address, VL6180X_SYSRANGE_VHV_RECALIBRATE, 0x01); // perform temperature calibration of the ranging sensor

    // Set Early Convergence Estimate for lower power consumption
    writeByteVL(address, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x25); // set max convergence time to 50 ms (steps of 1 ms)
    writeByteVL(address, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
    // This ECE is calculated as follows:
    // [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
    // This is ~123 ms for 50 ms max convergence time and 80% below threshold
    // This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
    writeByteVL(address, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
    writeByteVL(address, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x7B); // set early convergence estimate to 5%

    // Configure ALS
    writeByteVL(address, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x01);   // set to 100 ms
    // Following is a 16-bit register with the first MSByte reserved
    writeByteVL(address, VL6180X_SYSALS_INTEGRATION_PERIOD + 1, 0x20);        // set ALS integration time to 100 ms in steps of 1 ms -----25

    // The internal readout averaging sample period can be adjusted from 0 to 255. Increasing the sampling
    // period decreases noise but also reduces the effective max convergence time and increases power consumption:
    // Effective max convergence time = max convergence time - readout averaging period (see
    // Section 2.5: Range timing). Each unit sample period corresponds to around 64.5 μs additional
    // processing time. The recommended setting is 48 which equates to around 4.3 ms
    writeByteVL(address, VL6180X_READOUT_AVERAGING_SAMPLE_PERIOD, 0x02);  // compromise between low noise and increased execution time -----15
    // Gain can be 0 = 20, 1 = 10, 2 = 5, 3 = 2.5, 4 = 1.67, 5 = 1.25, 6 = 1.0 and 7 = 40
    // These are value possible for the lower nibble. The upper nibble must be 4
    writeByteVL(address, VL6180X_SYSALS_ANALOGUE_GAIN, 0x40 | ALSGain);   // Sets light and dark gain (don't change upper nibble)
    // Scalar (1 - 32, bits 4:0) to multiply raw ALS count for additonal gain, if necessary
    writeByteVL(address, VL6180X_FIRMWARE_RESULT_SCALER, 0x01);

    // Configure the interrupts
    writeByteVL(address, VL6180X_SYSTEM_MODE_GPIO0, 0x00);                // set up GPIO 0 (set to high impedence for now)
    writeByteVL(address, VL6180X_SYSTEM_MODE_GPIO1, 0x00);                // set up GPIO 1 (set to high impedence for now)
    writeByteVL(address, VL6180X_SYSTEM_INTERRUPT_CONFIG_GPIO, 0x24);     // enable sample ready interrupt


    // enable continuous range mode
    if (VL6180XMode == contRangeMode) {
      writeByteVL(address, VL6180X_SYSRANGE_START, 0x03);  // start auto range mode

    }
    // enable continuous ALS mode
    if (VL6180XMode == contALSMode) {
      // Configure ALS
      writeByteVL(address, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x32);   // set to 100 ms
      // Following is a 16-bit register with the first MSByte reserved
      writeByteVL(address, VL6180X_SYSALS_INTEGRATION_PERIOD + 1, 0x32);      // set ALS integration time to 50 ms in steps of 1 ms
      writeByteVL(address, VL6180X_SYSALS_START, 0x03);                     // start auto range mode
    }

    if (VL6180XMode == interLeaveMode) {
      // Configure ALS for interleaved mode at 10 Hz
      writeByteVL(address, VL6180X_SYSALS_INTERMEASUREMENT_PERIOD, 0x00);       // set to 100 ms  -----01
      // Following is a 16-bit register with the first MSByte reserved
      writeByteVL(address, VL6180X_SYSALS_INTEGRATION_PERIOD + 1, 0x02);        // set ALS integration time to 50 ms in steps of 1 ms  -----03
      writeByteVL(address, VL6180X_SYSRANGE_MAX_CONVERGENCE_TIME, 0x02);        // set max convergence time to 30 ms -----04
      writeByteVL(address, VL6180X_SYSRANGE_RANGE_CHECK_ENABLES, 0x10 | 0x01);  // enable (0x01) early convergence estimate
      // This ECE is calculated as follows:
      // [(1 - % below threshold) x 0.5 x 15630]/ range max convergence time
      // This is ~72 ms for 30 ms max convergence time and 80% below threshold
      // This is a sixteen bit (2 byte) register with the first byte MSByte and the second LSByte
      writeByteVL(address, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE, 0x00); // set early convergence estimate to 5%
      writeByteVL(address, VL6180X_SYSRANGE_EARLY_CONVERGENCE_ESTIMATE + 1, 0x1E); // set early convergence estimate to 5%
      writeByteVL(address, VL6180X_INTERLEAVED_MODE_ENABLE, 0x01);  // eanble interleave mode
      writeByteVL(address, VL6180X_SYSALS_START, 0x03);  // start continuous als measurement mode
      // range read automatically performed immediately after each ALS measurement

      // Clear reset bit
      writeByteVL(address, VL6180X_SYSTEM_FRESH_OUT_OF_RESET, 0x00);        // reset fresh_out_of_reset bit to zero

    }

  }

}

bool initializeSensor(uint8_t address)
{
  // Read the WHO_AM_I register, this is a good test of communication
  byte c = readByteVL(address, VL6180X_WHO_AM_I);  // Read WHO_AM_I register for VL6180X
  delay(50);
  if (c == 0xB4) // VL6180X WHO_AM_I should always be 0xB4
  {
    initVL6180X(address);
    return true;
  }
  else
    return false;
}

uint8_t readSensor(uint8_t address)
{
  uint8_t status = readByteVL(address, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);  // read new sample data ready status register

  if ( !(status & 0x40) && !(status & 0x80) )
  {
    if (VL6180XMode == contRangeMode)
    {
      // See if range new data is ready
      uint8_t range_status = status & 0x07;  // extract range status component

      while (range_status != 0x04) { // wait for new range measurement ready status
        status = readByteVL(address, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);
        range_status = status & 0x07;
        delay(1);
      }

      // OK, range data is ready, get range
      uint8_t range = readByteVL(address, VL6180X_RESULT_RANGE_VAL);
      //        Serial.print("Current range is "); Serial.print(range); Serial.println( " mm");  // print out range
      writeByteVL(address, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
      return range;
    }

    else if (VL6180XMode == contALSMode)
    {
      uint8_t als_status = (status & 0x38) >> 3;    // extract als status component

      while (als_status != 0x04) { // wait for new als measurement ready status
        status = readByteVL(address, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);
        als_status = (status & 0x38) >> 3;
        delay(1);
      }

      uint8_t rawData[2] = {0, 0};
      readByteVLs(address, VL6180X_RESULT_ALS_VAL, 2, &rawData[0]); // two-byte als data
      uint16_t alsraw = (uint16_t) (((uint16_t) rawData[0] << 8) | rawData[1]); //get 16-bit als raw value
      als = 0.32f * ((float) alsraw / realalsGain) * (100.0f / 100.0f); // convert to absolute lux

      // print out ambient light intensity in lux
      //            Serial.print("Current ambient light intensity is "); Serial.print(als, 1); Serial.println( " lux");
      //      alsValues[id] = als;
      writeByteVL(address, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
    }

    else if (VL6180XMode == interLeaveMode)
    {
      uint8_t als_status = (status & 0x38) >> 3;    // extract als status component

      while (als_status != 0x04)
      { // wait for new als measurement ready status
        status = readByteVL(address, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);
        als_status = (status & 0x38) >> 3;
        delay(1);
      }

      uint8_t rawData[2] = {0, 0};
      readByteVLs(address, VL6180X_RESULT_ALS_VAL, 2, &rawData[0]); // two-byte als data
      uint16_t alsraw = (uint16_t) (((uint16_t) rawData[0] << 8) | rawData[1]); //get 16-bit als raw value
      als = 0.32f * ((float) alsraw / realalsGain) * (100.0f / 100.0f); // convert to absolute lux

      //          Serial.print("Current ambient light intensity is "); Serial.print(als, 1); Serial.println( " lux");  // print out ambient light intensity in lux
      //      alsValues[id] = als;

      // See if range new data is ready
      uint8_t range_status = status & 0x07;  // extract range status component

      while (range_status != 0x04)
      {
        // wait for new range measurement ready status
        status = readByteVL(address, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);
        range_status = status & 0x07;
        delay(1);
      }

      // OK, range data is ready, get range
      uint8_t range = readByteVL(address, VL6180X_RESULT_RANGE_VAL);
      //          Serial.print("Current range is "); Serial.print(range); Serial.println( " mm");  // print out range
      writeByteVL(address, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);  // clear all data ready status interrupts
      return range;
    }
  }
  return 0xff;
}

void getalsGain() {
  switch (ALSGain)
  {
    // Possible ALS gains
    case alsGain20:
      realalsGain = 20.; // get actual ALS gain from nominal index
      break;
    case alsGain10:
      realalsGain = 10.32;
      break;
    case alsGain5:
      realalsGain = 5.21;
      break;
    case alsGain2_5:
      realalsGain = 2.60;
      break;
    case alsGain1_67:
      realalsGain = 1.72;
      break;
    case alsGain1_25:
      realalsGain = 1.28;
      break;
    case alsGain1_0:
      realalsGain = 1.01;
      break;
    case alsGain40:
      realalsGain = 40.;
      break;
  }
}
