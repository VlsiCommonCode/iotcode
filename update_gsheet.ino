// BY: Akshaya Niraula
// ON: Nov 11, 2016
// AT: http://www.embedded-lab.com/

#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS D2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define sensorPin D3
int sensorState = 0;
int lastState = 0;
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
//SimpleTimer timer;

const char* ssid = "Apush";
const char* password = "18022017";

// The ID below comes from Google Sheets.
// Towards the bottom of this page, it will explain how this can be obtained
const char *GScriptId = "AKfycbzzmugwWJU6D9VIQ0Gq8K89_ccbm9NpaUGSH5eUY5DJjn9f5lJDeUKCRMAwJ0surMcb=";

// Push data on this interval
const int dataPostDelay = 900000;     // 5 minutes = 15 * 60 * 1000

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";

const int httpsPort =     443;
HTTPSRedirect client(httpsPort);

// Prepare the url (without the varying data)
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";

// We will take analog input from A0 pin
const int AnalogIn                 = A0;

void setup() {
  Serial.begin(115200);
  Serial.println("Connecting to wifi: ");
  Serial.println(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print(String("Connecting to "));
  Serial.println(host);
  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying…");
  }

  // Connection Status, 1 = Connected, 0 is not.
  Serial.println("Connection Status: " + String(client.connected()));
  Serial.flush();

  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting…");
    Serial.flush();
    return;
  }

  // Data will still be pushed even certification don’t match.
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
}

// This is the main method where data gets pushed to the Google sheet
void postData(String tag, float value) {
  if (!client.connected()) {
    Serial.println("Connecting to client again…");
    client.connect(host, httpsPort);
  }
  String urlFinal = url + "tag=" + tag + "&value=" + String(value);
  Serial.println("final url is");
  Serial.println(urlFinal);
  client.printRedir(urlFinal, host, googleRedirHost);
}

// Continue pushing data at a given interval
void loop() {

  // Read analog value, in this case a soil moisture
  // int data = 1023 – analogRead(AnalogIn);
  //Read temprature and humidity value
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //if (isnan(h) || isnan(t)) {
  //  Serial.println("Failed to read from DHT sensor!");
  //  return;
 // }
  // Post these information
  postData("Temprature", h);
  //postData("Humidity", t);
  delay (dataPostDelay);
}
