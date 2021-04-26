

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

// define DHTPIN D3 // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11

const char* ssid = "Apush";
const char* password = "18022017";
// Push data on this interval
const int dataPostDelay = 900000;     // 5 minutes = 15 * 60 * 1000

// The ID below comes from Google Sheets.
// Towards the bottom of this page, it will explain how this can be obtained
String GScriptId = "AKfycbzzmugwWJU6D9VIQ0Gq8K89_ccbm9NpaUGSH5eUY5DJjn9f5lJDeUKCRMAwJ0surMcb";

const char* host = "script.google.com";
const int httpsPort = 443;

unsigned long tempoDeEnvioDHT = millis(); //tempo decorrido
unsigned long tempoDeEnvioLumus = millis(); //tempo decorrido
//calculo do tempo para comparar (1000 x 60 x minutos desejadio) = tempo em minutos
int tempoDHT = 1000 * 60 * 2;
int tempoLumus = 1000 * 60 * 1;

// Use WiFiClientSecure class to create TLS connection
BearSSL::WiFiClientSecure client;
String url = String("/macros/s/") + GScriptId + "/exec?";

const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
//Current web app URL: https://script.google.com/macros/s/–SgScript_ID–/exec

char *tags[] = {"temperature", "humidity", "luminosidade"};

float humidity, temperature;

void setup()
{
Serial.begin(115200);
client.setInsecure();

//connecting to internet
Serial.print("connecting to ");
Serial.println(ssid);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");
Serial.println("IP address: ");
Serial.println(WiFi.localIP());

dht.begin();
}

void loop()
{
//This void loop works even if there is no DHT sensor
temperature = random(15, 35);
float h = dht.readHumidity();
float t = dht.readTemperature();
sendData(t, tags[0]);
sendData(h, tags[1]);

delay(dataPostDelay);
}

//enviar dados para o Google Spreadsheet
void sendData(float value, String tag){

if (!client.connect(host, httpsPort)) {
Serial.println("connection failed");
return;
}

String string_tag= String(tag);
String string_value = String(value, 1);

Serial.print(string_tag);
Serial.print(": ");
Serial.println(value);

String url = "/macros/s/" + GScriptId + "/exec?tag=" + string_tag + "&value=" + string_value;
//String url = url + "tag=" + tag + "&value=" + String(string_value);
Serial.println(url);

Serial.print("requesting URL temp: ");
client.print(String("GET ") + url + " HTTP/1.1\r\n" +
"Host: " + host + "\r\n" +
"User-Agent: BuildFailureDetectorESP8266\r\n" +
"Connection: close\r\n\r\n");
delay(500);
Serial.println("request sent");
}
