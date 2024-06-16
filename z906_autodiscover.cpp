#include "z906.h"
#include "mqtt.h"

void Z906::AutoDiscovery(MQTT *mqttClient) {
  //DONT DELETE SetBufferSize(512) - ELSE BREAK AUTO DISCOVERY!
  //512 = JSON Size
  mqttClient->SetBufferSize(512);
  _AutoDiscovery_Volume(mqttClient);
  _AutoDiscovery_Volume_Bass(mqttClient);
  _AutoDiscovery_Volume_Center(mqttClient);
  _AutoDiscovery_Volume_Rear(mqttClient);
  _AutoDiscovery_Power(mqttClient);
  //Mute is kept track in Console
  //_AutoDiscovery_Mute(mqttClient);
  _AutoDiscovery_Input(mqttClient);
  _AutoDiscovery_Effect(mqttClient);
  //256 = Original Buffer Size
  mqttClient->SetBufferSize(256);
}

void Z906::_AutoDiscovery_Volume(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Volume",
    "Z906_Volume",
    "homeassistant/number/z906/volume/state",
    "homeassistant/number/z906/volume/set",
    "%");
  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/number/volume/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Volume_Bass(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Volume/Bass",
    "Z906_Volume_Bass",
    "homeassistant/number/z906/volume/bass/state",
    "homeassistant/number/z906/volume/bass/set",
    "%");

  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/number/volume/bass/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Volume_Center(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Volume/Center",
    "Z906_Volume_Center",
    "homeassistant/number/z906/volume/center/state",
    "homeassistant/number/z906/volume/center/set",
    "%");

  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/number/volume/center/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Volume_Rear(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Volume/Rear",
    "Z906_Volume_Rear",
    "homeassistant/number/z906/volume/rear/state",
    "homeassistant/number/z906/volume/rear/set",
    "%");

  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/number/volume/rear/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Power(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Power",
    "Z906_Power",
    "homeassistant/binary_sensor/z906/power/state");
  doc["expire_after"] = 30;
  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/binary_sensor/power/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Mute(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Mute",
    "Z906_Mute",
    "homeassistant/binary_sensor/z906/mute/state");

  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/binary_sensor/mute/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Input(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Input",
    "Z906_Input",
    "homeassistant/select/z906/input/state",
    "homeassistant/select/z906/input/set");
  JsonArray identifiers = doc.createNestedArray("options");
  identifiers.add("Input 1");
  identifiers.add("Input 2");
  identifiers.add("Input 3");
  identifiers.add("Input 4");
  identifiers.add("Input 5");
  identifiers.add("Input 6");

  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/select/input/config", payload.c_str(), true);
}

void Z906::_AutoDiscovery_Effect(MQTT *mqttClient) {
  StaticJsonDocument<512> doc = Z906::_Helper_BuildBaseAutoDisocvery(
    "Effect",
    "Z906_Effect",
    "homeassistant/select/z906/effect/state",
    "homeassistant/select/z906/effect/set");
  JsonArray identifiers = doc.createNestedArray("options");
  identifiers.add("3D");
  identifiers.add("2.1");
  identifiers.add("4.1");
  identifiers.add("None");

  String payload;
  serializeJson(doc, payload);
  mqttClient->Publish("homeassistant/select/effect/config", payload.c_str(), true);
}