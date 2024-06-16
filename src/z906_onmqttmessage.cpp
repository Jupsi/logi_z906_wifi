#include "z906.h"
#include "mqtt.h"

void Z906::OnMQTTMessage(MQTT* mqttClient, const char* topic, const char* message) {
  //Handles MQTT Broker Messages
  if (strcmp(topic, "homeassistant/number/z906/volume/set") == 0) {
    _OnMQTTMessage_SetVolume(mqttClient, message);
  } else if (strcmp(topic, "homeassistant/number/z906/volume/bass/set") == 0) {
    _OnMQTTMessage_SetVolume_Bass(mqttClient, message);
  } else if (strcmp(topic, "homeassistant/number/z906/volume/center/set") == 0) {
    _OnMQTTMessage_SetVolume_Center(mqttClient, message);
  } else if (strcmp(topic, "homeassistant/number/z906/volume/rear/set") == 0) {
    _OnMQTTMessage_SetVolume_Rear(mqttClient, message);
  } else if (strcmp(topic, "homeassistant/select/z906/input/set") == 0) {
    _OnMQTTMessage_SetSelect_Input(mqttClient, message);
  } else if (strcmp(topic, "homeassistant/select/z906/effect/set") == 0) {
    _OnMQTTMessage_SetSelect_Effect(mqttClient, message);
  } else {
    Serial.print("Unknown topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    Serial.println(message);
  }
}

void Z906::_OnMQTTMessage_SetVolume(MQTT* mqttClient, const char* message) {
  //Main Volume Broker Message Handle
  auto volume = Z906::_Helper_ConvertPercentToVolume(atoi(message));

  if (volume > _volume) {
    while (volume > _volume) {
      volume--;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_UP));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  } else {
    while (volume < _volume) {
      volume++;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_DOWN));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  }
  Serial.print("Volume Set to: ");
  Serial.println(volume);
}

void Z906::_OnMQTTMessage_SetVolume_Bass(MQTT* mqttClient, const char* message) {
  //Bass Volume Broker Message Handle
  auto volume = Z906::_Helper_ConvertPercentToVolume(atoi(message));

  if (volume > _volume_bass) {
    while (volume > _volume_bass) {
      volume--;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_BASS_UP));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  } else {
    while (volume < _volume_bass) {
      volume++;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_BASS_DOWN));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  }
  Serial.print("Bass Volume Set to: ");
  Serial.println(volume);
}

void Z906::_OnMQTTMessage_SetVolume_Center(MQTT* mqttClient, const char* message) {
  //Center Volume Broker Message Handle
  auto volume = Z906::_Helper_ConvertPercentToVolume(atoi(message));

  if (volume > _volume_center) {
    while (volume > _volume_center) {
      volume--;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_CENTER_UP));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  } else {
    while (volume < _volume_center) {
      volume++;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_CENTER_DOWN));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  }
  Serial.print("Center Volume Set to: ");
  Serial.println(volume);
}

void Z906::_OnMQTTMessage_SetVolume_Rear(MQTT* mqttClient, const char* message) {
  //Rear Volume Broker Message Handle
  auto volume = Z906::_Helper_ConvertPercentToVolume(atoi(message));

  if (volume > _volume_rear) {
    while (volume > _volume_rear) {
      volume--;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_REAR_DOWN));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  } else {
    while (volume < _volume_rear) {
      volume++;
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_REAR_DOWN));
      serialAmp->write(static_cast<uint8_t>(SERIAL_HEADER::RESET_IDLE_TIME));
    }
  }
  Serial.print("Rear Volume Set to: ");
  Serial.println(volume);
}

void Z906::_OnMQTTMessage_SetSelect_Input(MQTT* mqttClient, const char* message) {
  //Input Broker Message Handle
  auto input = Z906::_Helper_ConvertStringToInput(message);

  Serial.print("Input Set to: ");
  Serial.println(static_cast<int>(input));

  _Serial_Set_Input(input);
}
void Z906::_OnMQTTMessage_SetSelect_Effect(MQTT* mqttClient, const char* message) {
  //Effect Broker Message Handle
  auto effect = Z906::_Helper_ConvertStringToEffect(message);

  Serial.print("Effect Set to: ");
  Serial.println(static_cast<int>(effect));

  _Serial_Send_To_Amplifier(static_cast<uint8_t>(Z906::_Helper_ConvertEffectToSerialHeader(effect)));
}