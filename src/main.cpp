#include "credentials.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <esp_wpa2.h>
#include <wifi.h>

#define LED_BUILTIN 2
#define REQUEST_FREQ 10000
#define REQUEST_DATA_OPEN_CHAR_INDEX 9

unsigned long lastRequest = 0;

const char *ssid = "eduroam";

void setup() {
  Serial.begin(115200);

  // configure built in LED
  pinMode(LED_BUILTIN, OUTPUT);

  // connect to WiFi
  Serial.println("Connecting to eduroam...");

  // wifi credentials (identity, username, password) must be defined in a credentials.h file
  WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);

  // wait till connection succeeds
  while (WiFi.status() != WL_CONNECTED)
    ;

  // successfully connected to WiFi
  Serial.println("Connected to WiFi!");
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // make request with a preiod of REQUEST_FREQ
  if (millis() - lastRequest > REQUEST_FREQ || millis() < REQUEST_FREQ ) {
    // OV door endpoint
    String URL = "https://omegav.no/api/dooropen.php";

    // prepare http client
    HTTPClient http;
    http.begin(URL.c_str());

    // make request
    int status = http.GET();

    if (status == 200) {
      // get request payload
      String payload = http.getString();
      bool isOpen = atoi(&payload[REQUEST_DATA_OPEN_CHAR_INDEX]);

      // turn on built in LED if OV is open
      if (isOpen)
        digitalWrite(LED_BUILTIN, HIGH);
      else
        digitalWrite(LED_BUILTIN, LOW);
    } else {
      Serial.println("Request failed :(");
      digitalWrite(LED_BUILTIN, LOW);
    }

    http.end();
    lastRequest = millis();
  }
}