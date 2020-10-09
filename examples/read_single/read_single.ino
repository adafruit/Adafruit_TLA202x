// Basic demo for basic ADC readings from a single channel of the TLA2024
// Written to work with the Adafruit TLA2024 Breakout:
// https://adafruit.com/product/4780
#include <Wire.h>
#include <Adafruit_TLA202x.h>

Adafruit_TLA202x  tla;
void setup(void) {
  Serial.begin(115200);
  while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit TLA202x test!");

  // Try to initialize!
  if (!tla.begin()) { 
    Serial.println("Failed to find TLA202x chip");
    while (1) { delay(10); }
  }
  Serial.println("TLA202x Found!");

//  tla.setDataRate(TLA202x_RATE_12_5_HZ);
  Serial.print("Data rate set to: ");
  switch (tla.getDataRate()) {

    case TLA202x_RATE_128_SPS: Serial.println("128 SPS");
    case TLA202x_RATE_250_SPS: Serial.println("250 SPS");
    case TLA202x_RATE_490_SPS: Serial.println("490 SPS");
    case TLA202x_RATE_920_SPS: Serial.println("920 SPS");
    case TLA202x_RATE_1600_SPS: Serial.println("1600 SPS");
    case TLA202x_RATE_2400_SPS: Serial.println("2400 SPS");
    case TLA202x_RATE_3300_SPS: Serial.println("3300 SPS");

  }
}

void loop() {
  Serial.print("Channel 0: ");Serial.print(tla.readVoltage(TLA202x_CHANNEL_0));Serial.println(" volts to GND");
  Serial.println("");
  delay(100);
}

