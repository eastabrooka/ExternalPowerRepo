 /*************************************************************************
 * 
 * External Power Sensor
 * __________________
 * [2021] - Alex Eastabrook
 * Licence Soon.. 
 * 
 * This is the code for the ESP8266, ADS1115 and Adafruit ALS-PT19.  
 * The Package is installed in the Utility Box, Counting Pulses, and then relaying them to the ThingSpeak Platform. 
 * 
 */
  
#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include <Wire.h>
#include <Adafruit_ADS1015.h>

#include "SecureCreds.h"

#define SECOND 1000
#define MINUTE 60*SECOND
#define SAMPLE_WINDOW 60000
//5*MINUTE


Adafruit_ADS1115 ads(0x48);

// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = APIKEY;
const char* server = "api.thingspeak.com";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("\nPower Monitor Booting");

  Serial.println("Bringing External ADC Online");
  ads.begin();
}

// Post to Thingspeak. HTTP Push. 
void ThingSpeakTransmit()
{
  WiFiClient client;
  if (client.connect(server, 80)) {
    float t; 
    t = 69;
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    Serial.println("Sending data to Thingspeak");
  }
  client.stop();
}

//Enable Radios, and Trigger Send of Data. 
static unsigned long LastUpdate;
void DoUpdate()
{
  // Bring up the WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID ,WIFI_PSK);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Update Here");
  ThingSpeakTransmit();
  WiFi.mode( WIFI_OFF );
  WiFi.forceSleepBegin();
  Serial.println("WiFi is down");
}

//If Interval has Elapsed. Send Update.
void CheckForSendUpdate() {
  if (millis() - LastUpdate >= SAMPLE_WINDOW)
  {
    LastUpdate = millis();  //get ready for the next iteration
    Serial.println("Interval");
    DoUpdate();
  }
}

void loop() {
  CheckForSendUpdate();

  int16_t adc0, adc1, adc2, adc3;
 
adc0 = ads.readADC_SingleEnded(0);
Serial.print("AIN0: ");
Serial.println(adc0);
/*Serial.print("AIN1: ");
Serial.println(adc1);
Serial.print("AIN2: ");
Serial.println(adc2);
Serial.print("AIN3: ");
Serial.println(adc3);
Serial.println(" ");
 */
delay(50);


  
}
