#include <Arduino.h>
#include <HTTPClient.h>
#include <esp_wifi.h>
#include <wifi.h>

#define LED_BUILTIN 2
#define REQUEST_FREQ 10000
#define REQUEST_DATA_OPEN_CHAR_INDEX 9

unsigned long lastRequest = 0;

uint8_t macAdress[] = {0x44, 0x17, 0x93, 0x5e, 0x35, 0xcc};

void setup() {
  Serial.begin(115200);

  // set wifi mode
  WiFi.mode(WIFI_STA);

  // change mac adress
  esp_wifi_set_mac(WIFI_IF_STA, macAdress);

  // configure built in LED
  pinMode(LED_BUILTIN, OUTPUT);

  // connect to WiFi
  Serial.println("Connecting to WiFi...");
  while (!WiFi.begin("NTNU-IOT", ""))
    ;

  while (WiFi.status() != WL_CONNECTED)
    ;

  // successfully connected to WiFi
  Serial.println("Connected to WiFi!");
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // make request with a preiod of REQUEST_FREQ
  if (millis() - lastRequest > REQUEST_FREQ) {
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
    }

    http.end();
    lastRequest = millis();
  }
}