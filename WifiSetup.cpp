#include <WiFi.h>
#include "WifiSetup.h"

const char* ssid = "your WiFi SSID";
const char* password = "your WiFI Password";

static volatile bool wifi_connected = false;

void wifi_connect() {
  WiFi.begin(ssid, password);
}

void wifi_update() {
  if (WiFi.status() != WL_CONNECTED && wifi_connected == true) {
    wifi_connected = false;
    wifi_connect();
  }
  if (WiFi.status() == WL_CONNECTED && wifi_connected == false) {
    wifi_connected = true;
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}