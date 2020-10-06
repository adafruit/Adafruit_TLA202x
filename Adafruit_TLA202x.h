/*!
 *  @file Adafruit_TLA202x.h
 *
 * 	I2C Driver for the Adafruit library for the TLA202x ADCs from TI
 *
 * 	This is a library for the Adafruit TLA202x breakout:
 * 	https://www.adafruit.com/products/47XX
 *
 * 	Adafruit invests time and resources providing this open source code,
 *  please support Adafruit and open-source hardware by purchasing products from
 * 	Adafruit!
 *
 *
 *	BSD license (see license.txt)
 */
  // "requires_busio": "y",
  //   "requires_sensor": "y",
#ifndef _ADAFRUIT_TLA202x_H
#define _ADAFRUIT_TLA202x_H

#include "Arduino.h"
#include <Adafruit_BusIO_Register.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>#define TLA202x_I2CADDR_DEFAULT 0x60 ///< TLA202x default i2c address
#define TLA202x_CHIP_ID 0x60 ///< TLA202x default device id from WHOAMI

#define TLA202x_WHOAMI 0x00 ///< Chip ID register

///////////////////////////////////////////////////////////////
/**
 * @brief
 *
 * Allowed values for `setDataRate`.
 */
typedef enum {
  TLA202x_RATE_ONE_SHOT,
  TLA202x_RATE_1_HZ,
  TLA202x_RATE_7_HZ,
  TLA202x_RATE_12_5_HZ,
  TLA202x_RATE_25_HZ,
} tla202x_rate_t;

class Adafruit_TLA202x;



/** Adafruit Unified Sensor interface for temperature component of TLA202x */
class Adafruit_TLA202x_Temp : public Adafruit_Sensor {
public:
  /** @brief Create an Adafruit_Sensor compatible object for the temp sensor
      @param parent A pointer to the TLA202x class */
  Adafruit_TLA202x_Temp(Adafruit_TLA202x *parent) { _theTLA202x = parent; }
  bool getEvent(sensors_event_t *);
  void getSensor(sensor_t *);

private:
  int _sensorID = 2024;
  Adafruit_TLA202x *_theTLA202x = NULL;
};

/** Adafruit Unified Sensor interface for the pressure sensor component of TLA202x
 */
class Adafruit_TLA202x_Pressure : public Adafruit_Sensor {
public:
  /** @brief Create an Adafruit_Sensor compatible object for the pressure sensor
      @param parent A pointer to the TLA202x class */
  Adafruit_TLA202x_Pressure(Adafruit_TLA202x *parent) { _theTLA202x = parent; }
  bool getEvent(sensors_event_t *);
  void getSensor(sensor_t *);

private:
  int _sensorID = 2024+1;
  Adafruit_TLA202x *_theTLA202x = NULL;
};



/*!
 *    @brief  Class that stores state and functions for interacting with
 *            the TLA202x 12-bit ADCs
 */
class Adafruit_TLA202x {
public:
  Adafruit_TLA202x();
  ~Adafruit_TLA202x();

  bool begin_I2C(uint8_t i2c_addr = TLA202x_I2CADDR_DEFAULT,
                 TwoWire *wire = &Wire, int32_t sensor_id = 0);

  bool begin_SPI(uint8_t cs_pin, SPIClass *theSPI = &SPI,
                 int32_t sensor_id = 0);
  bool begin_SPI(int8_t cs_pin, int8_t sck_pin, int8_t miso_pin,
                 int8_t mosi_pin, int32_t sensor_id = 0);
  
  void reset(void);
  void interruptsActiveLow(bool active_low);

  
  tla202x_rate_t getDataRate(void);

  void setDataRate(tla202x_rate_t data_rate);
  bool getEvent(sensors_event_t *pressure, sensors_event_t *temp);

  Adafruit_Sensor *getTemperatureSensor(void);
  Adafruit_Sensor *getPressureSensor(void);

protected:
  void _read(void);
  virtual bool _init(int32_t sensor_id);

  float unscaled_temp,   ///< Last reading's temperature (C) before scaling
      unscaled_pressure; ///< Last reading's pressure (hPa) before scaling

  uint16_t _sensorid_pressure, ///< ID number for pressure
      _sensorid_temp;          ///< ID number for temperature

  Adafruit_I2CDevice *i2c_dev = NULL; ///< Pointer to I2C bus interface
  Adafruit_SPIDevice *spi_dev = NULL; ///< Pointer to SPI bus interface

  Adafruit_TLA202x_Temp *temp_sensor = NULL; ///< Temp sensor data object
  Adafruit_TLA202x_Pressure *pressure_sensor =
      NULL; ///< Pressure sensor data object

private:
  friend class Adafruit_TLA202x_Temp;     ///< Gives access to private members to
                                        ///< Temp data object
  friend class Adafruit_TLA202x_Pressure; ///< Gives access to private
                                        ///< members to Pressure data
                                        ///< object

  void fillPressureEvent(sensors_event_t *pressure, uint32_t timestamp);
  void fillTempEvent(sensors_event_t *temp, uint32_t timestamp);
};


#endif