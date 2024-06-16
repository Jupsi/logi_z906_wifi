#ifndef MQTT_H
#define MQTT_H
#include <WiFi.h>
#include <PubSubClient.h>

class MQTT {
private:
  IPAddress ipAddress;
  String broker;
  uint16_t port;
  String user;
  String password;

  PubSubClient* client;
public:
  MQTT();
  void Setup(IPAddress _ipAddress, uint16_t _port, const char* _user, const char* _password);
  void Setup(const char* _broker, uint16_t _port, const char* _user, const char* _password);
  void Connect();
  void Update();
  void Subscribe(char* topic);
  void Publish(const char* topic, const char* buffer, bool retained);
  void Publish(const char* topic, const char* buffer);
  void SetBufferSize(uint16_t size);
  PubSubClient* GetClient() {
    return client;
  }
};
#endif