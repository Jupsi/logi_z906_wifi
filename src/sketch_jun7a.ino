#include "WifiSetup.h"
#include "mqtt.h"
#include "z906.h"

unsigned long timerMQTT = millis();
//Do NOT change this to non-pointer Objects - ESP will crash, but don't ask me why
MQTT* mqttClient = NULL;
Z906* LOGI = NULL;

void MQTT_Message_Callback(const char* topic, const char* message) {
  /*
  This function handles Messages from MQTT Broker
  */
  if (LOGI != NULL) {
    LOGI->OnMQTTMessage(mqttClient, topic, message);
  }
}

void MQTT_Reconnect_Callback() {
  /*
  This function handles the reconnect to the MQTT Broker
  */
  if (LOGI != NULL) {
    LOGI->AutoDiscovery(mqttClient);
    LOGI->Subscribe(mqttClient);
    LOGI->PublishCurrentStates(mqttClient, true);
  }
}

void setup() {
  //Connect to USB Serial Output
  Serial.begin(115200);
  Serial.println("Setup Begin");

  //Create Z906 Object
  LOGI = new Z906();

  //Connect to the WiFi
  wifi_connect();

  mqttClient = new MQTT();
  // Use this for IP
  mqttClient->Setup(IPAddress(192, 168, 0, 111), 1883, "your-mqtt-user", "your-mqtt-password");
  // Use this for domain
  // mqttClient.Setup("your-mqqt-domain", 1883, "your-mqtt-user", "your-mqtt-password");
  mqttClient->Connect();

  Serial.println("Setup finished");
}

void loop() {
  unsigned long currtime = millis();
  if (currtime - timerMQTT > 1000) {
    //check WiFi and MQTT Connection 1 / Sec
    timerMQTT = currtime;

    wifi_update();

    if (mqttClient != NULL) {
      mqttClient->Update();
    }
  }
  
  if (LOGI != NULL) {
    //do Logitech Z906 Logic
    LOGI->Update(mqttClient);
  }
}
