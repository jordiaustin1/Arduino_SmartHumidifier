#include "U8glib.h"
#include "DHT.h"

// OLED Display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

// DHT Sensor
#define DHTPIN A0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Humidifier Pin
#define jiashi 2
#define button 12

void setup() {
  Serial.begin(9600);
  pinMode(jiashi, OUTPUT);
  dht.begin();
}

void loop() {
  float Humid = dht.readHumidity();
  float Temp = dht.readTemperature();

  Serial.print("Temp: ");
  Serial.println(Temp);
  Serial.print("Humid: ");
  Serial.println(Humid);

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_gdr14r);
    u8g.setPrintPos(25, 18);
    u8g.print("AH-01");
    u8g.setFont(u8g_font_9x18);
    u8g.setPrintPos(1, 40);
    u8g.print("Temp: ");
    u8g.print(Temp);
    u8g.print("'C");
    u8g.setPrintPos(1, 60);
    u8g.print("Humid: ");
    u8g.print(Humid);
    u8g.print("%");
  } while (u8g.nextPage());

  // Fuzzify inputs
  float humidityFuzzy = fuzzifyHumidity(Humid);
  float tempFuzzy = fuzzifyTemperature(Temp);

  // Control signal from fuzzy logic
  float controlSignal = fuzzyControl(humidityFuzzy, tempFuzzy);

  // Control the humidifier
  if (controlSignal > 0.5) {
    digitalWrite(jiashi, HIGH); // Turn ON humidifier
  } else {
    digitalWrite(jiashi, LOW);  // Turn OFF humidifier
  }

  delay(1000);
}

// Fuzzification for Humidity
float fuzzifyHumidity(float humidity) {
  if (humidity <= 50) return 1.0;                // Low Humidity
  else if (humidity <= 80) return (80 - humidity) / 30.0; // Medium Humidity
  else return 0.0;                              // High Humidity
}

// Fuzzification for Temperature
float fuzzifyTemperature(float temperature) {
  if (temperature <= 15) return 0.0;            // Cold
  else if (temperature <= 25) return (temperature - 15) / 10.0; // Comfortable
  else return 1.0;                              // Hot
}

// Fuzzy Control Function
float fuzzyControl(float humidity, float temperature) {
  // Rule 1: If Humidity is Low, Output is High (Turn ON humidifier)
  // Rule 2: If Humidity is Medium, Output is Medium (Partial ON)
  // Rule 3: If Humidity is High, Output is Low (Turn OFF humidifier)

  float rule1 = humidity;                // High output for low humidity
  float rule2 = min(1 - humidity, 1);    // Medium output for medium humidity
  float rule3 = 1 - humidity;            // Low output for high humidity

  // Combine rules using weighted average (defuzzification)
  return (1.0 * rule1 + 0.5 * rule2 + 0.0 * rule3) / (rule1 + rule2 + rule3);
}

