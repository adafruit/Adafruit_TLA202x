/*!
 *  @file Adafruit_TLA202x.cpp
 *
 *  @mainpage Adafruit library for the TLA202x ADCs from TI
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the TLA202x 12-bit ADCs
 *
 * 	This is a library is written to work with the Adafruit TLA2024 breakout:
 * 	https://www.adafruit.com/product/47XX
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *  @section dependencies Dependencies
 *  This library depends on the Adafruit BusIO library
 *
 *  This library depends on the Adafruit Unified Sensor library
 *
 *  @section author Author
 *
 *  Bryan Siepert for Adafruit Industries
 *
 * 	@section license License
 *
 * 	BSD (see license.txt)
 *
 * 	@section  HISTORY
 *
 *     v1.0 - First release
 */

#include "Arduino.h"
#include <Wire.h>

#include "Adafruit_TLA202x.h"

/**
 * @brief Construct a new Adafruit_TLA202x::Adafruit_TLA202x object
 *
 */
Adafruit_TLA202x::Adafruit_TLA202x(void) {}


/**
 * @brief Destroy the Adafruit_TLA202x::Adafruit_TLA202x object
 *
 */
Adafruit_TLA202x::~Adafruit_TLA202x(void) {
  if (config_register)
    delete config_register;
  if (data_register)
    delete data_register;
}

/*!
 *    @brief  Sets up the hardware and initializes I2C
 *    @param  i2c_address
 *            The I2C address to be used.
 *    @param  wire
 *            The Wire object to be used for I2C connections.
 *    @param  sensor_id
 *            The unique ID to differentiate the sensors from others
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TLA202x::begin(uint8_t i2c_address, TwoWire *wire,
                               int32_t sensor_id) {
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }


  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);
  if (config_register)
    delete config_register;
  config_register = new Adafruit_BusIO_Register(i2c_dev, TLA202x_CONFIG_REG, 2, MSBFIRST);
  if (data_register)
    delete data_register;
  data_register = new Adafruit_BusIO_Register(i2c_dev, TLA202x_DATA_REG, 2, MSBFIRST);

  if (!i2c_dev->begin()) {
    return false;
  }
  return init();
}


/*!  @brief Initializer for post i2c/spi init
 *   @param sensor_id Optional unique ID for the sensor set
 *   @returns True if chip identified and initialized
 */
bool Adafruit_TLA202x::init(void) {

  setDataRate(TLA202x_RATE_3300_SPS);
  setMode(TLA202x_MODE_CONTINUOUS);
  setMux(TLA202x_MUX_AIN0_GND);
  setRange(TLA202x_RANGE_6_144_V);

  return true;
}

/**
 * @brief Gets the current rate at which pressure and temperature measurements
 * are taken
 *
 * @return tla202x_rate_t The current data rate
 */
tla202x_rate_t Adafruit_TLA202x::getDataRate(void) {
  Adafruit_BusIO_Register ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, TLA202x_CTRL_REG1, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl1, 3, 4);

  return (tla202x_rate_t)data_rate.read();
}
/**
 * @brief Sets the rate at which pressure and temperature measurements
 *
 * @param new_data_rate The data rate to set. Must be a `tla202x_rate_t`
 */
void Adafruit_TLA202x::setDataRate(tla202x_rate_t new_data_rate) {
  Adafruit_BusIO_Register ctrl1 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, TLA202x_CTRL_REG1, 1);
  Adafruit_BusIO_RegisterBits data_rate =
      Adafruit_BusIO_RegisterBits(&ctrl1, 3, 4);

  data_rate.write((uint8_t)new_data_rate);
}


float Adafruit_TLA202x::readVoltage(tla202x_channel_t channel){
  // _os = RWBit(_CONFIG_REG, 15, 2, lsb_first=False)
  setMux((tla202x_mux_t) channel+4); // single channel to gnd  are the last 4 mux enums
  return readVoltage();
}

float Adafruit_TLA202x::readVoltage(void){

}

tla202x_rate_t Adafruit_TLA202x::getDataRate(void){
  Adafruit_BusIO_RegisterBits rate_bits = Adafruit_BusIO_RegisterBits(config_register, 3, 5);
}
void Adafruit_TLA202x::setDataRate(tla202x_rate_t data_rate){
  Adafruit_BusIO_RegisterBits rate_bits = Adafruit_BusIO_RegisterBits(config_register,3, 5);
}

    // _mode = RWBit(_CONFIG_REG, 8, 2, lsb_first=False)
bool Adafruit_TLA202x::setMode(tla202x_mode_t mode){
  Adafruit_BusIO_RegisterBits mode_bit = Adafruit_BusIO_RegisterBits(config_register, 1, 8);
  return mode_bit.write(mode);
}
tla202x_mode_t Adafruit_TLA202x::getMode(void){
  Adafruit_BusIO_RegisterBits mode_bit = Adafruit_BusIO_RegisterBits(config_register, 1, 8);
  return (tla202x_mode_t)mode_bit.read();
}

// _mux = RWBits(3, _CONFIG_REG, 12, 2, lsb_first=False)
bool Adafruit_TLA202x::setMux(tla202x_mux_t mux){
  Adafruit_BusIO_RegisterBits mux_bits = Adafruit_BusIO_RegisterBits(config_register, 3, 12);
  return mux_bits.write(mux);
}
tla202x_mux_t Adafruit_TLA202x::getMux(void){
  Adafruit_BusIO_RegisterBits mux_bits = Adafruit_BusIO_RegisterBits(config_register, 3, 12);
  return (tla202x_mux_t)mux_bits.read();
}

    // _pga = RWBits(3, _CONFIG_REG, 9, 2, lsb_first=False)
bool Adafruit_TLA202x::setRange(tla202x_range_t range){
  Adafruit_BusIO_RegisterBits rate_bits = Adafruit_BusIO_RegisterBits(config_register, 3, 9);
  return rate_bits.write(range);
}
tla202x_range_t Adafruit_TLA202x::getRange(void){
  Adafruit_BusIO_RegisterBits rate_bits = Adafruit_BusIO_RegisterBits(config_register, 3, 9);
  return (tla202x_range_t)rate_bits.read();
}


/*!
 *     @brief  Updates the measurement data for all sensors simultaneously
 */
/**************************************************************************/
void Adafruit_TLA202x::_read(void) {
  // get raw readings


  // TODO: use this block to manually pack bytes and fix sign
  // Adafruit_BusIO_Register pressure_data = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, pressure_addr, 3);
  // uint8_t buffer[3];

  // temp_data.read(buffer, 2);
  // int16_t raw_temp;

  // raw_temp |= (int16_t)(buffer[1]);
  // raw_temp <<= 8;
  // raw_temp |= (int16_t)(buffer[0]);

  // pressure_data.read(buffer, 3);
  // int32_t raw_pressure;

  // raw_pressure = (int32_t)buffer[2];
  // raw_pressure <<= 8;
  // raw_pressure |= (int32_t)(buffer[1]);
  // raw_pressure <<= 8;
  // raw_pressure |= (int32_t)(buffer[0]);

  // // TODO: This can be done by casting to signed type
  // if (raw_temp & 0x8000) {
  //   raw_temp = raw_temp - 0xFFFF;
  // }
  // unscaled_temp = raw_temp;

  // if (raw_pressure & 0x800000) {
  //   raw_pressure = raw_pressure - 0xFFFFFF;
  // }


}

