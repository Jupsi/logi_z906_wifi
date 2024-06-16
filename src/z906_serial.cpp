#include "z906.h"
#include "mqtt.h"

void Z906::_Set_Serial_Timeout() {
  _timer = millis() + SERIAL_TIMEOUT;
}

bool Z906::_Is_Serial_Timeout_Over() {
  if (_timer == 0) {
    return true;
  }

  if (millis() > _timer) {
    _timer = 0;
    if (_serial_console_buffer.size() > 0) {
      //when the timeout is over and we have buffered data from the console we send it to the amp
      _Serial_Send_To_Amplifier(_serial_console_buffer.data(), _serial_console_buffer.size());
      _serial_console_buffer.clear();
    }
    return true;
  }

  return false;
}

void Z906::_Serial_Process_Console() {
  /*
  https://github.com/nomis/logitech-z906/blob/main/protocol.rst
  All Console Commands are 1 Byte 
  However some things like switching inputs sends multiple commands at once
  But this shouldnt be a problem with this way since AMP doenst care if its concat or seperate writes
  */
  if (serialConsole->available() && _Is_Serial_Timeout_Over()) {
    int counter = 0;
    while (serialConsole->available()) {
      if (counter > 50) {
        break;  //something is not right, we received 50 bytes in one loop - break and try again next loop
      }
      int data = serialConsole->read();
      _Serial_Send_To_Amplifier(data);


      SERIAL_HEADER header = static_cast<SERIAL_HEADER>(data);
      if (Z906::_Helper_IsMultiByteRequest(header)) {
        //Block Serial (according to documentation it confuses amp to send more in the meantime)
        _Set_Serial_Timeout();
      }
      counter++;
    }
  }
}

void Z906::_Serial_Process_Amplifier(MQTT* mqttClient) {
  if (serialAmp->available() && _Is_Serial_Timeout_Over()) {
    while (serialAmp->available()) {
      int data = serialAmp->read();

      _On_Serial_Amplifier_Message(mqttClient, static_cast<SERIAL_HEADER>(data));
    }
  }
}

void Z906::_On_Serial_Amplifier_Message(MQTT* mqttClient, SERIAL_HEADER header) {
  switch (header) {
    case SERIAL_HEADER::SELECT_INPUT_1:
    case SERIAL_HEADER::SELECT_INPUT_2:
    case SERIAL_HEADER::SELECT_INPUT_3:
    case SERIAL_HEADER::SELECT_INPUT_4:
    case SERIAL_HEADER::SELECT_INPUT_5:
    case SERIAL_HEADER::SELECT_INPUT_6:
      _input = static_cast<INPUT_TYPE>(header);
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::VOLUME_UP:
      {
        if (_volume < Z906::GetMaxVolume()) {
          _volume++;
          _has_value_changed = true;
        }

        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::VOLUME_DOWN:
      {
        if (_volume > 0) {
          _volume--;
          _has_value_changed = true;
        }
        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::POWER_OFF:
      _power = false;
      _has_value_changed = true;
      //Publish here because after that the Console will be in off state and wont publish
      PublishCurrentStates(mqttClient);

      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::POWER_ON:
      _power = true;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::SELECT_EFFECT_3D:
      _effect = EFFECT_TYPE::EFFECT_3D;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::SELECT_EFFECT_4_1:
      _effect = EFFECT_TYPE::EFFECT_4_1;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::SELECT_EFFECT_2_1:
      _effect = EFFECT_TYPE::EFFECT_2_1;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::VOLUME_BASS_UP:
      {
        if (_volume_bass < Z906::GetMaxVolume()) {
          _volume_bass++;
          _has_value_changed = true;
        }
        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::VOLUME_BASS_DOWN:
      {
        if (_volume_bass > 0) {
          _volume_bass--;
          _has_value_changed = true;
        }
        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::VOLUME_CENTER_UP:
      {
        if (_volume_center < Z906::GetMaxVolume()) {
          _volume_center++;
          _has_value_changed = true;
        }
        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::VOLUME_CENTER_DOWN:
      {
        if (_volume_center > 0) {
          _volume_center--;
          _has_value_changed = true;
        }
        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::VOLUME_REAR_UP:
      {
        if (_volume_rear < Z906::GetMaxVolume()) {
          _volume_rear++;
          _has_value_changed = true;
        }
        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::VOLUME_REAR_DOWN:
      {
        if (_volume_rear > 0) {
          _volume_rear--;
          _has_value_changed = true;
        }

        _Serial_Send_To_Console(static_cast<uint8_t>(header));
      }
      break;
    case SERIAL_HEADER::SELECT_EFFECT_NONE:
      _effect = EFFECT_TYPE::EFFECT_NONE;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::MUTE_ON:
      _mute = true;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::MUTE_OFF:
      _mute = false;
      _has_value_changed = true;
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
    case SERIAL_HEADER::MULTIBYTE_REPLY:
      {
        _On_Serial_Multibyte_Message(mqttClient);
      }
      break;
    case SERIAL_HEADER::RESET_IDLE_TIME:
    case SERIAL_HEADER::AMP_MUTED:
    case SERIAL_HEADER::AMP_UNMUTED:
    case SERIAL_HEADER::SAVE:
    case SERIAL_HEADER::UNKNOWN:
    case SERIAL_HEADER::HEADPHONES:
    case SERIAL_HEADER::READ_STATUS:
    case SERIAL_HEADER::STOP_SPEAKER_TEST:
    case SERIAL_HEADER::READ_IDLE_TIME:
    case SERIAL_HEADER::READ_INPUT_VOLUME:
    case SERIAL_HEADER::READ_AMP_TEMPERATURE:
    case SERIAL_HEADER::START_SPEAKER_TEST:
    default:
      _Serial_Send_To_Console(static_cast<uint8_t>(header));
      break;
  }
}

void Z906::_Serial_Send_To_Console(const uint8_t header) {
  serialConsole->write(header);
}

void Z906::_Serial_Send_To_Console(const uint8_t* buffer, const size_t size) {
  serialConsole->write(buffer, size);
}

void Z906::_Serial_Send_To_Amplifier(const uint8_t header) {
  if (_Is_Serial_Timeout_Over()) {
    serialAmp->write(header);
  } else {
    _serial_console_buffer.push_back(header);
  }
}

void Z906::_Serial_Send_To_Amplifier(const uint8_t* buffer, const size_t size) {
  if (_Is_Serial_Timeout_Over()) {
    serialAmp->write(buffer, size);
  } else {
    for (size_t i = 0; i < size; i++) {
      _serial_console_buffer.push_back(buffer[i]);
    }
  }
}

uint8_t Z906::_Serial_Amplifier_ReadByte_With_Wait() {
  //This Method blocks the Process until bytes are available
  while (serialAmp->available() == 0) {
    delay(50);  //wait
  }
  return serialAmp->read();
}

void Z906::_On_Serial_Multibyte_Message(MQTT* mqttClient) {
  //this Method processes all different multibyte messages
  //it makes sure to read the complete message before answering to the console
  std::vector<uint8_t> data;
  data.push_back(static_cast<uint8_t>(SERIAL_HEADER::MULTIBYTE_REPLY));

  uint8_t type = _Serial_Amplifier_ReadByte_With_Wait();
  data.push_back(type);

  uint8_t length = _Serial_Amplifier_ReadByte_With_Wait();
  uint8_t read_length = 0;
  data.push_back(length);

  Serial.print("Multibyte Length: ");
  Serial.println(length, 10);

  switch (static_cast<MULTIBYTE_TYPE>(type)) {
    case MULTIBYTE_TYPE::CONFIGURATION_AMP:
      {
        _volume = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        data.push_back(_volume);
        _volume_rear = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        data.push_back(_volume_rear);
        _volume_center = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        data.push_back(_volume_center);
        _volume_bass = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        data.push_back(_volume_bass);

        uint8_t input = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        _input = static_cast<INPUT_TYPE>(input + 1);
        data.push_back(input);

        _mute = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        data.push_back(_mute);



        for (int i = 0; i < 6; i++) {
          uint8_t input_effect = _Serial_Amplifier_ReadByte_With_Wait();
          read_length++;

          //check for current input and set our effect according
          if (i + 1 == static_cast<int>(_input)) {
            //todo this is not correct order..
            //AMP Sends 4 5 2 6 1 3
            switch (input_effect) {
              case 0:
                _effect = EFFECT_TYPE::EFFECT_3D;
                break;
              case 1:
                _effect = EFFECT_TYPE::EFFECT_2_1;
                break;
              case 2:
                _effect = EFFECT_TYPE::EFFECT_4_1;
                break;
              case 3:
                _effect = EFFECT_TYPE::EFFECT_NONE;
                break;
            }
          }
          data.push_back(input_effect);
        }

        uint8_t digital_signal = _Serial_Amplifier_ReadByte_With_Wait();
        read_length++;
        data.push_back(digital_signal);

        for (int i = read_length; i < length; i++) {
          uint8_t unknown_data = _Serial_Amplifier_ReadByte_With_Wait();
          read_length++;
          data.push_back(unknown_data);
        }

        uint8_t lrc = _Serial_Amplifier_ReadByte_With_Wait();
        data.push_back(lrc);

        //activate for debug - if this is not correct your Console wont start or do crazy
        //Serial.print("Config Amp Received: ");
        //for (size_t i = 0; i < data.size(); i++) {
        //  Serial.print(data[i], 16);
        //  Serial.print(" ");
        //}
        //Serial.println(" ");

        Serial.println("Current Config:");
        Serial.print("Volume: ");
        Serial.println(_volume);
        Serial.print("Bass: ");
        Serial.println(_volume_bass);
        Serial.print("Center: ");
        Serial.println(_volume_center);
        Serial.print("Rear: ");
        Serial.println(_volume_rear);
        Serial.print("Input: ");
        Serial.println(Z906::_Helper_ConvertInputToString(_input));
        Serial.print("Effect: ");
        Serial.println(Z906::_Helper_ConvertEffectToString(_effect));

        _has_value_changed = true;
      }
      break;
    //case MULTIBYTE_TYPE::CONFIGURATION_CONSOLE: //according to docs console should not report this and amp gets confused by the response
    case MULTIBYTE_TYPE::READ_IDLE_TIME:
    case MULTIBYTE_TYPE::SPEAKER_TEST:
    case MULTIBYTE_TYPE::READ_INPUT_VOLUME:
    case MULTIBYTE_TYPE::READ_AMP_TEMPERATUR:
    case MULTIBYTE_TYPE::CONFIG_RESET_IDLE:
    default:
      {
        for (uint8_t i = 0; i < length; i++) {
          uint8_t unknown_data = _Serial_Amplifier_ReadByte_With_Wait();
          data.push_back(unknown_data);
        }
        uint8_t lrc = _Serial_Amplifier_ReadByte_With_Wait();
        data.push_back(lrc);
      }
      break;
  }

  //write the data to the console
  if (data.size() > 0) {
    _Serial_Send_To_Console(data.data(), data.size());
  }
}

void Z906::_Serial_Set_Input(INPUT_TYPE input) {
  /*
  https://github.com/nomis/logitech-z906/blob/main/protocol.rst#input-selection
  mute (09 09 09..)
  Input
  Effect
  unmute (08 08 08..)
  */

  std::vector<uint8_t> data;
  auto currentVolume = _volume;
  for (size_t i = 0; i < currentVolume; i++) {
    data.push_back(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_DOWN));
  }
  data.push_back(static_cast<uint8_t>(input));

  Z906::EFFECT_TYPE effect = Z906::_Helper_IsEffectPossible(input, _effect);

  data.push_back(static_cast<uint8_t>(Z906::_Helper_ConvertEffectToSerialHeader(effect)));
  for (size_t i = 0; i < currentVolume; i++) {
    data.push_back(static_cast<uint8_t>(SERIAL_HEADER::VOLUME_UP));
  }

  _Serial_Send_To_Amplifier(data.data(), data.size());
}

void Z906::_Serial_Request_Config() {
  _Serial_Send_To_Amplifier(static_cast<uint8_t>(SERIAL_HEADER::READ_STATUS));
}