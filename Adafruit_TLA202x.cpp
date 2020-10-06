/*!
 *  @file Adafruit_TLA202x.cpp
 *
 *  @mainpage Adafruit library for the TLA202x ADCs from TI
 *
 *  @section intro_sec Introduction
 *
 * 	I2C Driver for the Library for the TLA202x 12-bit ADCs
 *
 * 	This is a library for the Adafruit TLA202x breakout:
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
  if (temp_sensor)
    delete temp_sensor;
  if (pressure_sensor)
    delete pressure_sensor;
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
bool Adafruit_TLA202x::begin_I2C(uint8_t i2c_address, TwoWire *wire,
                               int32_t sensor_id) {
  spi_dev = NULL;
  if (i2c_dev) {
    delete i2c_dev; // remove old interface
  }

  i2c_dev = new Adafruit_I2CDevice(i2c_address, wire);

  if (!i2c_dev->begin()) {
    return false;
  }

  return _init(sensor_id);
}

/*!
 *    @brief  Sets up the hardware and initializes hardware SPI
 *    @param  cs_pin The arduino pin # connected to chip select
 *    @param  theSPI The SPI object to be used for SPI connections.
 *    @param  sensor_id
 *            The user-defined ID to differentiate different sensors
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TLA202x::begin_SPI(uint8_t cs_pin, SPIClass *theSPI,
                               int32_t sensor_id) {
  i2c_dev = NULL;

  if (spi_dev) {
    delete spi_dev; // remove old interface
  }
  spi_dev = new Adafruit_SPIDevice(cs_pin,
                                   1000000,               // frequency
                                   SPI_BITORDER_MSBFIRST, // bit order
                                   SPI_MODE0,             // data mode
                                   theSPI);
  if (!spi_dev->begin()) {
    return false;
  }

  return _init(sensor_id);
}

/*!
 *    @brief  Sets up the hardware and initializes software SPI
 *    @param  cs_pin The arduino pin # connected to chip select
 *    @param  sck_pin The arduino pin # connected to SPI clock
 *    @param  miso_pin The arduino pin # connected to SPI MISO
 *    @param  mosi_pin The arduino pin # connected to SPI MOSI
 *    @param  sensor_id
 *            The user-defined ID to differentiate different sensors
 *    @return True if initialization was successful, otherwise false.
 */
bool Adafruit_TLA202x::begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
                               int8_t mosi_pin, int32_t sensor_id) {
  i2c_dev = NULL;

  if (spi_dev) {
    delete spi_dev; // remove old interface
  }
  spi_dev = new Adafruit_SPIDevice(cs_pin, sck_pin, miso_pin, mosi_pin,
                                   1000000,               // frequency
                                   SPI_BITORDER_MSBFIRST, // bit order
                                   SPI_MODE0);            // data mode
  if (!spi_dev->begin()) {
    return false;
  }
  return _init(sensor_id);
}

/*!  @brief Initializer for post i2c/spi init
 *   @param sensor_id Optional unique ID for the sensor set
 *   @returns True if chip identified and initialized
 */
bool Adafruit_TLA202x::_init(int32_t sensor_id) {

  Adafruit_BusIO_Register chip_id = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, TLA202x_WHOAMI, 1);

  // make sure we're talking to the right chip
  if (chip_id.read() != TLA202x_CHIP_ID) {
    return false;
  }
  _sensorid_pressure = sensor_id;
  _sensorid_temp = sensor_id + 1;

  reset();
  // do any software reset or other initial setup

  // Set to highest rate
  setDataRate(TLA202x_RATE_25_HZ);
  

  // TODO: update for correct sensor types
  pressure_sensor = new Adafruit_TLA202x_Pressure(this);
  temp_sensor = new Adafruit_TLA202x_Temp(this);
  
  return true;
}

/**
 * @brief Performs a software reset initializing registers to their power on
 * state
 *
 */
void Adafruit_TLA202x::reset(void) {
  Adafruit_BusIO_Register ctrl_2 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, TLA202x_CTRL_REG2, 1);
  Adafruit_BusIO_RegisterBits sw_reset =
      Adafruit_BusIO_RegisterBits(&ctrl_2, 1, 2);

  sw_reset.write(1);
  while (sw_reset.read()) {
    delay(1);
  }
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

/******************* Adafruit_Sensor functions *****************/
/*!
 *     @brief  Updates the measurement data for all sensors simultaneously
 */
/**************************************************************************/
void Adafruit_TLA202x::_read(void) {
  // get raw readings

  // TODO: Update for sensor types and enable multi-byte read if needed
  // uint8_t pressure_addr = TLA202x_PRESS_OUT_XL;
  // uint8_t temp_addr = TLA202x_TEMP_OUT_L;
  // if (spi_dev) {
  //   // for TLA202x SPI, addr[7] is r/w, addr[6] is auto increment
  //   pressure_addr |= 0x40;
  //   temp_addr |= 0x40;
  // }



  // TODO: use this block to manually pack bytes and fix sign
  // Adafruit_BusIO_Register pressure_data = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, pressure_addr, 3);

  // Adafruit_BusIO_Register temp_data = Adafruit_BusIO_Register(
  //     i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, temp_addr, 2);

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



  Adafruit_BusIO_Register to_out =
      Adafruit_BusIO_Register(i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD,
                              (HTS221_T0_OUT | multi_byte_address_mask), 2);
  Adafruit_BusIO_Register t1_out =
      Adafruit_BusIO_Register(i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD,
                              (HTS221_T1_OUT | multi_byte_address_mask), 2);

  to_out.read(&T0_OUT);
  t1_out.read(&T1_OUT);



  unscaled_pressure = raw_pressure;
}


/*!
    @brief  Gets an Adafruit Unified Sensor object for the presure sensor
   component
    @return Adafruit_Sensor pointer to pressure sensor
 */
Adafruit_Sensor *Adafruit_TLA202x::getPressureSensor(void) {
  return pressure_sensor;
}

/*!
    @brief  Gets an Adafruit Unified Sensor object for the temp sensor component
    @return Adafruit_Sensor pointer to temperature sensor
 */
Adafruit_Sensor *Adafruit_TLA202x::getTemperatureSensor(void) {
  return temp_sensor;
}

/**************************************************************************/
/*!
    @brief  Gets the pressure sensor and temperature values as sensor events
    @param  pressure Sensor event object that will be populated with pressure
   data
    @param  temp Sensor event object that will be populated with temp data
    @returns True
*/
/**************************************************************************/
bool Adafruit_TLA202x::getEvent(sensors_event_t *pressure,
                              sensors_event_t *temp) {
  uint32_t t = millis();
  _read();

  // use helpers to fill in the events
  fillPressureEvent(pressure, t);
  fillTempEvent(temp, t);
  return true;
}


void Adafruit_TLA202x::fillPressureEvent(sensors_event_t *pressure,
                                       uint32_t timestamp) {
  memset(pressure, 0, sizeof(sensors_event_t));
  pressure->version = sizeof(sensors_event_t);
  pressure->sensor_id = _sensorid_pressure;
  pressure->type = SENSOR_TYPE_PRESSURE;
  pressure->timestamp = timestamp;
  pressure->pressure = (unscaled_pressure / 4096.0);
}

void Adafruit_TLA202x::fillTempEvent(sensors_event_t *temp, uint32_t timestamp) {
  memset(temp, 0, sizeof(sensors_event_t));
  temp->version = sizeof(sensors_event_t);
  temp->sensor_id = _sensorid_temp;
  temp->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  temp->timestamp = timestamp;
  temp->temperature = (unscaled_temp / 480) + 42.5;
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t data for the TLA202x's tenperature
*/
/**************************************************************************/
void Adafruit_TLA202x_Pressure::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "TLA202x_P", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_PRESSURE;
  sensor->min_delay = 0;
  sensor->min_value = 260;
  sensor->max_value = 1260;
  // 4096 LSB = 1 hPa >>  1 LSB = 1/4096 hPa >> 1 LSB =  2.441e-4 hPa
  sensor->resolution = 2.441e-4;
}

/**************************************************************************/
/*!
    @brief  Gets the pressure as a standard sensor event
    @param  event Sensor event object that will be populated
    @returns True
*/
/**************************************************************************/
bool Adafruit_TLA202x_Pressure::getEvent(sensors_event_t *event) {
  _theTLA202x->_read();
  _theTLA202x->fillPressureEvent(event, millis());

  return true;
}

/**************************************************************************/
/*!
    @brief  Gets the sensor_t data for the TLA202x's tenperature
*/
/**************************************************************************/
void Adafruit_TLA202x_Temp::getSensor(sensor_t *sensor) {
  /* Clear the sensor_t object */
  memset(sensor, 0, sizeof(sensor_t));

  /* Insert the sensor name in the fixed length char array */
  strncpy(sensor->name, "TLA202x_T", sizeof(sensor->name) - 1);
  sensor->name[sizeof(sensor->name) - 1] = 0;
  sensor->version = 1;
  sensor->sensor_id = _sensorID;
  sensor->type = SENSOR_TYPE_AMBIENT_TEMPERATURE;
  sensor->min_delay = 0;
  sensor->min_value = -30;
  sensor->max_value = 105;
  // 480 LSB = 1°C >> 1 LSB = 1/480°C >> 1 LSB =  0.00208 °C
  sensor->resolution = 0.00208;
}

/**************************************************************************/
/*!
    @brief  Gets the temperature as a standard sensor event
    @param  event Sensor event object that will be populated
    @returns True
*/
/**************************************************************************/
bool Adafruit_TLA202x_Temp::getEvent(sensors_event_t *event) {
  _theTLA202x->_read();
  _theTLA202x->fillTempEvent(event, millis());

  return true;
}

/**
 * @brief Sets the polarity of the INT pin.
 *
 * @param active_low Set to true to make the pin active low
 */
void Adafruit_TLA202x::interruptsActiveLow(bool active_low) {
  Adafruit_BusIO_Register ctrl3 = Adafruit_BusIO_Register(
      i2c_dev, spi_dev, ADDRBIT8_HIGH_TOREAD, TLA202x_CTRL_REG3, 1);

  Adafruit_BusIO_RegisterBits active_low_bit =
      Adafruit_BusIO_RegisterBits(&ctrl3, 1, 7);
  active_low_bit.write(active_low);
}
