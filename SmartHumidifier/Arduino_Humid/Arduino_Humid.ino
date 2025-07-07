#include "U8glib.h"
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

#include "DHT.h"

#define DHTPIN A0
#define DHTTYPE DHT11

#define jiashi 2
#define button 12

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  pinMode(jiashi, OUTPUT);
  pinMode(button, INPUT);
  dht.begin();
}

void loop() {
  float Humid = dht.readHumidity();
  float Temp = dht.readTemperature();

  // Display humidity and temperature on Serial Monitor
  Serial.print("Temp: ");
  Serial.print(Temp);
  Serial.print("°C, ");
  Serial.print("Humid: ");
  Serial.print(Humid);
  Serial.println("%");
  delay(1000);

  // Fuzzy Logic Membership Functions (Simple)
  // Define "Low", "Medium", and "High" for Humidity
  float lowHumid = max(0, min(1, (80 - Humid) / 30));  // Membership for "Low"
  float medHumid = max(0, min(1, (Humid - 50) / 30, (110 - Humid) / 30));  // "Medium"
  float highHumid = max(0, min(1, (Humid - 80) / 30));  // "High"

  // Fuzzy Rules for Controlling Humidifier
  // Output is proportional to fuzzy membership of "Low"
  float humidifierPower = lowHumid * 255 + medHumid * 128;  // Output range: 0-255

  // Control Humidifier Output
  analogWrite(jiashi, (int)humidifierPower);

  // Display temperature and humidity on OLED
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_gdr14r);
    u8g.setPrintPos(25, 18);
    u8g.print("DKARDU");

    u8g.setFont(u8g_font_9x18);
    u8g.setPrintPos(1, 40);
    u8g.print("Temp: ");
    u8g.print(Temp);
    u8g.print("'C");

    u8g.setPrintPos(1, 60);
    u8g.print("Humid: ");
    u8g.print(Humid);
    u8g.print("%");

    u8g.setPrintPos(1, 80);
    u8g.print("Fan: ");
    u8g.print(humidifierPower);
  } while (u8g.nextPage());
}
