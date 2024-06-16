#ifndef Z906_H
#define Z906_H

#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <vector>
#include "mqtt.h"
class Z906 {
private:
  HardwareSerial* serialAmp;
  HardwareSerial* serialConsole;

  long _timer;
  //timeout when multibyte request by console
  const long SERIAL_TIMEOUT = 500;

  //timeouts to response to mqtt broker
  const long MQTT_TIMEOUT = 2500;
  const long MQTT_POWER_TIMEOUT = 15000;

  //esp GPIO Header
  const int ADC_PIN_LOW = 35;
  const int ADC_PIN_CON_BOOT = 32;
  const int ADC_PIN_CABLE_CONNECTED = 34;
  const int8_t RX_CONSOLE_GPIO = 16;
  const int8_t TX_CONSOLE_GPIO = 17;
  const int8_t RX_AMP_GPIO = 22;
  const int8_t TX_AMP_GPIO = 23;

  const unsigned long BAUD_RATE_LOGITECH = 57600;

public:
  //Helper types
  //Effect States
  enum class EFFECT_TYPE : uint8_t {
    EFFECT_3D = 0x00,
    EFFECT_2_1 = 0x01,
    EFFECT_4_1 = 0x02,
    EFFECT_NONE = 0x03,
  };
  //Input State
  enum class INPUT_TYPE : uint8_t {
    INPUT_1 = 0x02,
    INPUT_2 = 0x05,
    INPUT_3 = 0x03,
    INPUT_4 = 0x04,
    INPUT_5 = 0x06,
    INPUT_6 = 0x07,
  };
  //Serial Headers
  //https://github.com/nomis/logitech-z906/blob/main/protocol.rst
  enum class SERIAL_HEADER : uint8_t {
    SELECT_INPUT_1 = 0x02,
    SELECT_INPUT_2 = 0x05,
    SELECT_INPUT_3 = 0x03,
    SELECT_INPUT_4 = 0x04,
    SELECT_INPUT_5 = 0x06,
    SELECT_INPUT_6 = 0x07,
    VOLUME_UP = 0x08,
    VOLUME_DOWN = 0x09,
    POWER_OFF = 0x10,
    POWER_ON = 0x11,
    SELECT_EFFECT_3D = 0x14,
    SELECT_EFFECT_4_1 = 0x15,
    SELECT_EFFECT_2_1 = 0x16,
    VOLUME_BASS_UP = 0x0A,
    VOLUME_BASS_DOWN = 0x0B,
    VOLUME_CENTER_UP = 0x0C,
    VOLUME_CENTER_DOWN = 0x0D,
    VOLUME_REAR_UP = 0x0E,
    VOLUME_REAR_DOWN = 0x0F,
    AMP_MUTED = 0x20,
    AMP_UNMUTED = 0x21,
    START_SPEAKER_TEST = 0x22,
    READ_AMP_TEMPERATURE = 0x25,
    READ_INPUT_VOLUME = 0x2F,
    RESET_IDLE_TIME = 0x30,
    READ_IDLE_TIME = 0x31,
    STOP_SPEAKER_TEST = 0x33,
    READ_STATUS = 0x34,
    SELECT_EFFECT_NONE = 0x35,
    SAVE = 0x36,
    UNKNOWN = 0x37,
    MUTE_ON = 0x38,
    MUTE_OFF = 0x39,
    HEADPHONES = 0x3F,
    MULTIBYTE_REPLY = 0xAA,
  };
  //Multibyte Types
  enum class MULTIBYTE_TYPE : uint8_t {
    CONFIGURATION_AMP = 0x0A,
    CONFIGURATION_CONSOLE = 0x0E,
    READ_IDLE_TIME = 0x0F,
    SPEAKER_TEST = 0x07,
    READ_INPUT_VOLUME = 0x08,
    READ_AMP_TEMPERATUR = 0x0C,
    CONFIG_RESET_IDLE = 0x0E,
  };
  enum class DIGITAL_SIGNAL_AMP : uint8_t {
    DIGITAL_NONE = 0x00,
    DIGITAL_5_1 = 0x01,
    DIGITAL_5_1_2_0 = 0x0C,
    DIGITAL_UNKNOWN = 0x0E,
  };
  //Digital Signal Console
  enum class DIGITAL_SIGNAL_CONSOLE : uint8_t {
    DIGITAL_UNKNOWN = 0x00,
    DIGITAL_NONE = 0x01,
    DIGITAL_5_1 = 0x02,
    DIGITAL_4_1 = 0x03,
    DIGITAL_3_1 = 0x04,
    DIGITAL_2_1 = 0x05,
    DIGITAL_2_0 = 0x06,
    DIGITAL_5_1_2_0 = 0x07,
    DETECTED = 0x08,
  };
  //Decode States
  enum class DECODE_STATES : uint8_t {
    DECODE_5_1 = 0x17,
    DECODE_NO_SIGNAL = 0x18,
    DECODE_4_1 = 0x19,
    DECODE_3_1 = 0x1A,
    DECODE_2_1 = 0x1B,
    DECODE_2_0 = 0x1C,
    DECODE_DETECTED = 0x1D,
    DECODE_DETECTED_5_1_2_0 = 0x1E,
    DECODE_UNKNOWN = 0x1F,
  };
  //Test Speaker Select
  enum class SPEAKER_TEST : uint8_t {
    NONE = 0x00,
    FRONT_LEFT = 0x01,
    FRONT_RIGHT = 0x02,
    CENTER = 0x10,
    BASS = 0x20,
    REAR_LEFT = 0x04,
    REAR_RIGHT = 0x08,
  };
private:
  //Hardware States (Entitiys for HomeAssistant)
  //Copy States used so we can keep track of volume when muted
  uint8_t _volume;
  uint8_t _volume_bass;
  uint8_t _volume_center;
  uint8_t _volume_rear;
  bool _power;
  bool _mute;
  INPUT_TYPE _input;
  EFFECT_TYPE _effect;

  bool _is_connected;
  bool _has_value_changed;
  long _mqtt_timeout;
  long _mqtt_power_timeout;

  void _Set_Serial_Timeout();
  bool _Is_Serial_Timeout_Over();
  //AutoDisovery for each Entitity
  void _AutoDiscovery_Volume(MQTT* mqttClient);
  void _AutoDiscovery_Volume_Bass(MQTT* mqttClient);
  void _AutoDiscovery_Volume_Center(MQTT* mqttClient);
  void _AutoDiscovery_Volume_Rear(MQTT* mqttClient);
  void _AutoDiscovery_Power(MQTT* mqttClient);
  void _AutoDiscovery_Mute(MQTT* mqttClient);
  void _AutoDiscovery_Input(MQTT* mqttClient);
  void _AutoDiscovery_Effect(MQTT* mqttClient);

  //Helpers
  static bool _Helper_IsMultiByteRequest(SERIAL_HEADER& header);
  static int8_t _Helper_ConvertVolumeToPercent(const int8_t volume);
  static int8_t _Helper_ConvertPercentToVolume(const int8_t percent);
  static bool _Helper_ConvertStringToBoolean(const char* message);
  static char* _Helper_ConvertBooleanToString(const bool var);
  static char* _Helper_ConvertInputToString(Z906::INPUT_TYPE input);
  static Z906::SERIAL_HEADER _Helper_ConvertEffectToSerialHeader(Z906::EFFECT_TYPE effect);
  static Z906::INPUT_TYPE _Helper_ConvertStringToInput(const char* message);
  static char* _Helper_ConvertEffectToString(Z906::EFFECT_TYPE effect);
  static Z906::EFFECT_TYPE _Helper_ConvertStringToEffect(const char* message);
  static Z906::EFFECT_TYPE _Helper_IsEffectPossible(Z906::INPUT_TYPE input, Z906::EFFECT_TYPE effect);
  static StaticJsonDocument<512> _Helper_BuildBaseAutoDisocvery(const char* name, const char* uniqueId, const char* state_topic, const char* command_topic = NULL, const char* unit_of_measurement = NULL);


  //OnMessage Handlers For each Set Topic
  void _OnMQTTMessage_SetVolume(MQTT* mqttClient, const char* message);
  void _OnMQTTMessage_SetVolume_Bass(MQTT* mqttClient, const char* message);
  void _OnMQTTMessage_SetVolume_Center(MQTT* mqttClient, const char* message);
  void _OnMQTTMessage_SetVolume_Rear(MQTT* mqttClient, const char* message);
  //void _OnMQTTMessage_SetSwitch_Power(MQTT* mqttClient, const char* message);
  //void _OnMQTTMessage_SetSwitch_Mute(MQTT* mqttClient, const char* message);
  void _OnMQTTMessage_SetSelect_Input(MQTT* mqttClient, const char* message);
  void _OnMQTTMessage_SetSelect_Effect(MQTT* mqttClient, const char* message);

  static int8_t GetMaxVolume() {
    //43 is max Volume
    //https://github.com/nomis/logitech-z906/blob/main/protocol.rst

    return 43;
  }

  std::vector<uint8_t> _serial_console_buffer;

  void _Serial_Send_To_Console(const uint8_t header);
  void _Serial_Send_To_Amplifier(const uint8_t header);
  void _Serial_Send_To_Console(const uint8_t* buffer, const size_t size);
  void _Serial_Send_To_Amplifier(const uint8_t* buffer, const size_t size);

  void _Serial_Set_Input(INPUT_TYPE type);

  void _Serial_Process_Console();
  void _Serial_Process_Amplifier(MQTT* mqttClient);
  void _On_Serial_Amplifier_Message(MQTT* mqttClient, SERIAL_HEADER header);
  void _On_Serial_Multibyte_Message(MQTT* mqttClient);
  uint8_t _Serial_Amplifier_ReadByte_With_Wait();
  void _Serial_Request_Config();
public:
  Z906();

  void OnMQTTMessage(MQTT* mqttClient, const char* topic, const char* message);

  void Subscribe(MQTT* mqttClient);
  void AutoDiscovery(MQTT* mqttClient);
  void PublishCurrentStates(MQTT* mqttClient, bool force = false);
  void PublishPowerState(MQTT* mqttClient);

  bool IsSerialPossible() const;

  void Update(MQTT* mqttClient);
};
#endif