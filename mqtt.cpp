#include "mqtt.h"
#include <WiFi.h>

//forward declare callback functions from .ino file
void MQTT_Message_Callback(const char*, const char*);
void MQTT_Reconnect_Callback();

//PubsubClient callback, called when a new message arrives from broker
void callback_client(char* topic, uint8_t* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  MQTT_Message_Callback((const char*)topic, message.c_str());
}

//has to be global so ESP32 won't crash
WiFiClient espClient;

MQTT::MQTT() {
  //Do NOT change this to non-pointer Object - It will crash the ESP32
  client = new PubSubClient(espClient);
}

void MQTT::Setup(IPAddress _ipAddress, uint16_t _port, const char* _user, const char* _password) {
  //Setup with IP Address from Broker
  ipAddress = _ipAddress;
  port = _port;
  user = String(_user);
  password = String(_password);

  client->setCallback(::callback_client);
  client->setServer(ipAddress, port);
}

void MQTT::Setup(const char* _broker, uint16_t _port, const char* _user, const char* _password) {
  //Setup with Domain from Broker
  broker = String(_broker);
  port = _port;
  user = String(_user);
  password = String(_password);

  client->setCallback(::callback_client);
  client->setServer(broker.c_str(), port);
}

String IpAddress2String(const IPAddress& ipAddress) {
  //Format IP to String for Output
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") + String(ipAddress[2]) + String(".") + String(ipAddress[3]);
}

void MQTT::Connect() {
  //This Method handles the connect to the MQTT Broker
  if (!client->connected()) {
    //generate a unique client ID
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.print("Attempting to connect MQTT... ");

    //determine which way we want to connect to the Broker
    if (broker.length() == 0) {
      Serial.print("IP: ");
      Serial.print(::IpAddress2String(ipAddress));
    } else {
      Serial.print("Domain: ");
      Serial.print(broker);
    }
    Serial.print(" Port: ");
    Serial.println(port);
    Serial.print("Auth with User: ");
    Serial.println(user);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("Client ID: ");
    Serial.println(client_id);

    if (client->connect(client_id.c_str(), user.c_str(), password.c_str())) {
      Serial.println("MQTT Connected!");
      //When connected we call the Reconnect Callback to fire Events
      MQTT_Reconnect_Callback();
    } else {
      Serial.print("failed with state ");
      Serial.println(client->state());
    }
  }
}

//all These Messages check for connection and only execute if connected
//this is due the fact even when the Broker is down we want to keep the rest alive and not block the ESP until broker is back online
//only Update will try to reconnect to the broker

void MQTT::Update() {
  //Check if Broker connection is lost
  if (!client->connected()) {
    Serial.println("Reconnect to MQTT Broker");
    Connect();
  }

  //if connected update the PubsubClient
  if (client->connected()) {
    client->loop();
  }
}

//Wrapper Code below
void MQTT::Subscribe(char* topic) {
  if (client->connected()) {
    client->subscribe(topic);
  }
}

void MQTT::Publish(const char* topic, const char* buffer, bool retained) {
  if (client->connected()) {
    client->publish(topic, buffer, retained);
  }
}

void MQTT::Publish(const char* topic, const char* buffer) {
  if (client->connected()) {
    client->publish(topic, buffer);
  }
}

void MQTT::SetBufferSize(uint16_t size) {
  client->setBufferSize(size);
}