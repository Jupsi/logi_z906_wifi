#include "z906.h"
#include "mqtt.h"

Z906::Z906()
  //Initialize Volume with ~20% (= 9, 43 Max)
  : _volume(9),
    _volume_bass(9),
    _volume_center(9),
    _volume_rear(9),
    _power(false),
    _input(Z906::INPUT_TYPE::INPUT_3),
    _effect(Z906::EFFECT_TYPE::EFFECT_3D),
    _mute(false),
    _is_connected(false),
    _mqtt_timeout(millis() + Z906::MQTT_TIMEOUT),
    _mqtt_power_timeout(millis() + Z906::MQTT_POWER_TIMEOUT),
    _timer(0),
    _has_value_changed(false) {
  serialConsole = new HardwareSerial(1);
  serialAmp = new HardwareSerial(2);

  serialConsole->begin(BAUD_RATE_LOGITECH, SERIAL_8O1, RX_CONSOLE_GPIO, TX_CONSOLE_GPIO);
  serialAmp->begin(BAUD_RATE_LOGITECH, SERIAL_8O1, RX_AMP_GPIO, TX_AMP_GPIO);

  if (IsSerialPossible()) {
    _Serial_Request_Config();
  }
}

void Z906::Subscribe(MQTT* mqttClient) {
  //When connected to the Broker Subscribe to our Set Topics
  mqttClient->Subscribe("homeassistant/number/z906/volume/set");
  mqttClient->Subscribe("homeassistant/number/z906/volume/bass/set");
  mqttClient->Subscribe("homeassistant/number/z906/volume/center/set");
  mqttClient->Subscribe("homeassistant/number/z906/volume/rear/set");
  mqttClient->Subscribe("homeassistant/select/z906/input/set");
  mqttClient->Subscribe("homeassistant/select/z906/effect/set");
}

void Z906::PublishCurrentStates(MQTT* mqttClient, bool force) {
  //When reconnected to the Broker we publish our current states
  //This is done because if we switch anything in the meantime we want to let the broker know
  if (_is_connected && (_has_value_changed || force)) {
    mqttClient->Publish("homeassistant/number/z906/volume/state", String(Z906::_Helper_ConvertVolumeToPercent(_volume)).c_str(), true);
    mqttClient->Publish("homeassistant/number/z906/volume/bass/state", String(Z906::_Helper_ConvertVolumeToPercent(_volume_bass)).c_str(), true);
    mqttClient->Publish("homeassistant/number/z906/volume/center/state", String(Z906::_Helper_ConvertVolumeToPercent(_volume_center)).c_str(), true);
    mqttClient->Publish("homeassistant/number/z906/volume/rear/state", String(Z906::_Helper_ConvertVolumeToPercent(_volume_rear)).c_str(), true);
    mqttClient->Publish("homeassistant/select/z906/input/state", String(Z906::_Helper_ConvertInputToString(_input)).c_str(), true);
    mqttClient->Publish("homeassistant/select/z906/effect/state", String(Z906::_Helper_ConvertEffectToString(_effect)).c_str(), true);
    //Mute is kept track in the Console and not by amp
    //mqttClient->Publish("homeassistant/binary_sensor/z906/mute/state", String(Z906::_Helper_ConvertBooleanToString(_mute)).c_str(), true);

    _has_value_changed = false;
  }
  if (force) {
    PublishPowerState(mqttClient);
  }
}

void Z906::PublishPowerState(MQTT* mqttClient) {
  mqttClient->Publish("homeassistant/binary_sensor/z906/power/state", String(Z906::_Helper_ConvertBooleanToString(_is_connected)).c_str(), true);
}

bool Z906::IsSerialPossible() const {
  /*
  This part here is a little complicated and still doesnt work correct

  //The Theory behind this is
  [Pin 08] [ADC_PIN_CON_BOOT]         [3V3] [Console 2-3 Seconds after Power off || Console starting up 2 Second after Power on]
  [Pin 15] [ADC_PIN_LOW]              [0.5V] [Console is off]
  [Pin 15] [ADC_PIN_LOW]              [0.0V] [Console is on] (0.0V also at booting - serial data is garbage)
  [Pin 11] [ADC_PIN_CABLE_CONNECTED]  [3V3] [When Cable is plugged in]

  Pin 8 and Pin 15 should work together to filter garbage on Serial (undefined state)

  4095 = 3V3 ESP ADC Value
  */
  float voltageConBoot = analogRead(ADC_PIN_CON_BOOT) * (3.3 / 4095.0);
  float voltageCableConnected = analogRead(ADC_PIN_CABLE_CONNECTED) * (3.3 / 4095.0);
  float voltageLow = analogRead(ADC_PIN_LOW) * (3.3 / 4095.0);

  if (voltageConBoot > 3.0) {
    return false;
  }

  if (voltageCableConnected < 3.0) {
    return false;
  }

  if (voltageLow > 0.1) {
    return false;
  }

  return true;
}

void Z906::Update(MQTT* mqttClient) {
  //update mqtt data
  if (millis() > _mqtt_timeout) {
    _mqtt_timeout = millis() + Z906::MQTT_TIMEOUT;
    PublishCurrentStates(mqttClient);
  }

  if (millis() > _mqtt_power_timeout) {
    _mqtt_power_timeout = millis() + Z906::MQTT_POWER_TIMEOUT;
    PublishPowerState(mqttClient);
  }

  bool serial_Possible = IsSerialPossible();
  if (serial_Possible == false) {
    if (serialAmp->available()) {
      serialAmp->read();  //discard undefined bytes
    }
    if (_is_connected == true) {
      _is_connected = false;
    }
  } else {
    if (_is_connected == false) {
      _is_connected = true;
    }
    _Serial_Process_Console();
    _Serial_Process_Amplifier(mqttClient);
  }

  //TX RX Timeout
  delay(5);
}