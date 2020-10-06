// Basic demo for accelerometer readings from Adafruit TLA202x

Adafruit_TLA202x tla202x;


}



// Basic demo for readings from Adafruit TLA202x
#include <Wire.h>
#include <Adafruit_TLA202x.h>

// For SPI mode, we need a CS pin
#define TLA202X_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define TLA202X_SCK 13
#define TLA202X_MISO 12
#define TLA202X_MOSI 11

Adafruit_TLA202x  tla202x; // TODO FIX NAME
void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit TLA202x test!");

  // Try to initialize!
  if (!tla202x.begin_I2C()) {
  //if (!tla202x.begin_SPI(TLA202X_CS)) {
  //if (!tla202x.begin_SPI(TLA202X_CS, TLA202X_SCK, TLA202X_MISO, TLA202X_MOSI)) {
    Serial.println("Failed to find TLA202x chip");
    while (1) { delay(10); }
  }
  Serial.println("TLA202x Found!");

//  tla202x.setDataRate(TLA202x_RATE_12_5_HZ);
  Serial.print("Data rate set to: ");
  switch (tla202x.getDataRate()) {
    case TLA202x_RATE_ONE_SHOT: Serial.println("One Shot"); break;
    case TLA202x_RATE_1_HZ: Serial.println("1 Hz"); break;
    case TLA202x_RATE_7_HZ: Serial.println("7 Hz"); break;

  }

void loop() {

  sensors_event_t temp;
  sensors_event_t pressure;
  tla202x.getEvent(&pressure, &temp);// get pressure
  Serial.print("Temperature: ");Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.print("Pressure: ");Serial.print(pressure.pressure);Serial.println(" hPa");
  Serial.println("");
  delay(100);
}
 sensors_event_t temp;
  sensors_event_t pressure;
  tla202x.getEvent(&pressure, &temp);// get pressure
  Serial.print(" Temperature: ");Serial.print(temp.temperature);Serial.println(" degrees C");
  Serial.print(" Pressure: ");Serial.print(pressure.pressure);Serial.println(" hPa");
  Serial.println("");
  delay(100);
}
