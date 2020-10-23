#include <Arduino.h>

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain
#include <ArduinoJson.h>
// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor
#include <IotWebConf.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#define DHTPIN D1     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.
const char thingName[] = "scale";
const char wifiInitialApPassword[] = "12345678";
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.

DNSServer dnsServer;
WebServer server(80);
HTTPUpdateServer httpUpdater;
String enrollID;
IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword, CONFIG_VERSION);
String Website, data, Javascript, XML;
DHT dht(DHTPIN, DHTTYPE);

void WebsiteContent()
{
  if (data == "")
  {
    Serial.println("Unable to contact the scale");

    data = "Unable to contact the scale";
  }
  else
  {
  }

  server.sendHeader("access-control-allow-origin", "*");
  server.send(200, "text/html", data);
}
void handleRoot2()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 04 Update Server</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change values.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}
void handle_NotFound()
{
  server.send(404, "text/plain", "404 Not found.");
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();
  iotWebConf.setupUpdateServer(&httpUpdater);
  iotWebConf.getApTimeoutParameter()->visible = true;
  // iotWebConf.setWifiConnectionTimeoutMs(&httpUpdater);
  // -- Initializing the configuration.
  iotWebConf.init();
  //timer.setInterval(300L, WebsiteContent);
  Serial.println("\nSoftware serial test started");
  server.on("/hardware", WebsiteContent);
  // server.on("/", );
  server.on("/config", [] { iotWebConf.handleConfig(); });
  
  server.onNotFound(handle_NotFound);
  Serial.println("HTTP server started");

}

void loop() {
    iotWebConf.doLoop();
  server.handleClient();
  // Wait a few seconds between measurements.
  
  DynamicJsonDocument root(100);
    root["temp"] = String(t);
    root["hum"] = String(h);
    serializeJson(root, data);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
}
