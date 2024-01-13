#define BLYNK_PRINT Serial

#define BLYNK_USE_DIRECT_CONNECT

#include <Adafruit_NeoPixel.h>
#include <Adafruit_BME280.h>
#include <BlynkSimpleEsp32_BLE.h>
#include <BLEDevice.h>
#include <BLEServer.h>

Adafruit_NeoPixel rgb(1, 16); 

Adafruit_BME280 bme;

char auth[] = "YourAuthToken";

long prevMillis = 0;
int red, green, blue;
boolean colorChange = false;

BLYNK_WRITE(V0)
{
  red = param[0].asInt() / 10;
  green = param[1].asInt() / 10;
  blue = param[2].asInt() / 10;

  colorChange = true;
}

void setup()
{
  // Debug console
  Serial.begin(115200);

  rgb.begin();

  if (!bme.begin()) {
    Serial.println("Failed to find Hibiscus Sense BME280 chip");
  }

  Serial.println("Waiting for connections…");

  Blynk.setDeviceName("Blynk");

  Blynk.begin(auth);

  rgb.show();
}

void loop()
{
  Blynk.run();

  if (colorChange == true) {
    colorChange = false;

    rgb.setPixelColor(0, red, green, blue);
    rgb.show();

    Serial.print("RGB color updated!");
  }

  if (millis() – prevMillis > 2000) {

    float pressure = bme.readPressure() / 100;
    float humidity = bme.readHumidity();
    float temperature = bme.readTemperature();

    Serial.print("Barometric Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");
    Blynk.virtualWrite(1, pressure);

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %RH");
    Blynk.virtualWrite(2, humidity);

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");
    Blynk.virtualWrite(3, temperature);

    Serial.println();

    prevMillis = millis();
  }
}
