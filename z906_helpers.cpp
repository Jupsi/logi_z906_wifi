#include <math.h>
#include "z906.h"

int8_t Z906::_Helper_ConvertVolumeToPercent(const int8_t volume) {
  int8_t percent = round(100. / Z906::GetMaxVolume() * volume);
  if (percent < 0) {
    percent = 0;
  } else if (percent > 100) {
    percent = 100;
  }
  return percent;
}

int8_t Z906::_Helper_ConvertPercentToVolume(const int8_t percent) {
  if (percent < 0) {
    return 0;
  } else if (percent > 100) {
    return Z906::GetMaxVolume();
  }
  return round(static_cast<double>(percent) / 100 * Z906::GetMaxVolume());
}

StaticJsonDocument<512> Z906::_Helper_BuildBaseAutoDisocvery(const char* name, const char* uniqueId, const char* state_topic, const char* command_topic, const char* unit_of_measurement) {
  StaticJsonDocument<512> doc;
  doc["name"] = name;
  doc["state_topic"] = state_topic;
  if (command_topic != NULL) {
    doc["command_topic"] = command_topic;
  }
  if (unit_of_measurement != NULL) {
    doc["unit_of_measurement"] = unit_of_measurement;
  }
  doc["unique_id"] = uniqueId;
  JsonObject device = doc.createNestedObject("device");
  device["name"] = "Z906";
  device["model"] = "Z906";
  device["manufacturer"] = "Logitech";

  JsonArray identifiers = doc["device"].createNestedArray("identifiers");
  identifiers.add("Z906");
  return doc;
}

bool Z906::_Helper_ConvertStringToBoolean(const char* message) {
  if (strcmp(message, "ON") == 0) {
    return true;
  }
  return false;
}

char* Z906::_Helper_ConvertBooleanToString(const bool var) {
  if (var == true) {
    return "ON";
  }
  return "OFF";
}

char* Z906::_Helper_ConvertInputToString(Z906::INPUT_TYPE input) {
  switch (input) {
    case Z906::INPUT_TYPE::INPUT_1: return "Input 1";
    case Z906::INPUT_TYPE::INPUT_2: return "Input 2";
    case Z906::INPUT_TYPE::INPUT_3: return "Input 3";
    case Z906::INPUT_TYPE::INPUT_4: return "Input 4";
    case Z906::INPUT_TYPE::INPUT_5: return "Input 5";
    case Z906::INPUT_TYPE::INPUT_6: return "Input 6";
  }
  return "Input 1";
}

Z906::INPUT_TYPE Z906::_Helper_ConvertStringToInput(const char* message) {
  if (strcmp("Input 1", message) == 0) {
    return Z906::INPUT_TYPE::INPUT_1;
  } else if (strcmp("Input 2", message) == 0) {
    return Z906::INPUT_TYPE::INPUT_2;
  } else if (strcmp("Input 3", message) == 0) {
    return Z906::INPUT_TYPE::INPUT_3;
  } else if (strcmp("Input 4", message) == 0) {
    return Z906::INPUT_TYPE::INPUT_4;
  } else if (strcmp("Input 5", message) == 0) {
    return Z906::INPUT_TYPE::INPUT_5;
  } else if (strcmp("Input 6", message) == 0) {
    return Z906::INPUT_TYPE::INPUT_6;
  }
  return Z906::INPUT_TYPE::INPUT_1;
}

char* Z906::_Helper_ConvertEffectToString(Z906::EFFECT_TYPE effect) {
  switch (effect) {
    case Z906::EFFECT_TYPE::EFFECT_3D: return "3D";
    case Z906::EFFECT_TYPE::EFFECT_2_1: return "2.1";
    case Z906::EFFECT_TYPE::EFFECT_4_1: return "4.1";
    case Z906::EFFECT_TYPE::EFFECT_NONE: return "None";
  }
  return "None";
}

Z906::EFFECT_TYPE Z906::_Helper_ConvertStringToEffect(const char* message) {
  if (strcmp("3D", message) == 0) {
    return Z906::EFFECT_TYPE::EFFECT_3D;
  } else if (strcmp("2.1", message) == 0) {
    return Z906::EFFECT_TYPE::EFFECT_2_1;
  } else if (strcmp("4.1", message) == 0) {
    return Z906::EFFECT_TYPE::EFFECT_4_1;
  } else if (strcmp("None", message) == 0) {
    return Z906::EFFECT_TYPE::EFFECT_NONE;
  }
  return Z906::EFFECT_TYPE::EFFECT_NONE;
}

Z906::SERIAL_HEADER Z906::_Helper_ConvertEffectToSerialHeader(Z906::EFFECT_TYPE effect) {
  switch (effect) {
    case Z906::EFFECT_TYPE::EFFECT_3D: return Z906::SERIAL_HEADER::SELECT_EFFECT_3D;
    case Z906::EFFECT_TYPE::EFFECT_4_1: return Z906::SERIAL_HEADER::SELECT_EFFECT_4_1;
    case Z906::EFFECT_TYPE::EFFECT_2_1: return Z906::SERIAL_HEADER::SELECT_EFFECT_2_1;
    case Z906::EFFECT_TYPE::EFFECT_NONE: return Z906::SERIAL_HEADER::SELECT_EFFECT_NONE;
  }
  return Z906::SERIAL_HEADER::SELECT_EFFECT_NONE;
}

Z906::EFFECT_TYPE Z906::_Helper_IsEffectPossible(Z906::INPUT_TYPE input, Z906::EFFECT_TYPE effect) {
  switch (input) {
    case Z906::INPUT_TYPE::INPUT_1:
    case Z906::INPUT_TYPE::INPUT_2:
    case Z906::INPUT_TYPE::INPUT_6:
      return effect;
  }
  return Z906::EFFECT_TYPE::EFFECT_NONE;
}

bool Z906::_Helper_IsMultiByteRequest(SERIAL_HEADER& header) {
  switch (header) {
    case Z906::SERIAL_HEADER::READ_STATUS:
      //case Z906::SERIAL_HEADER::READ_IDLE_TIME:
      return true;
  }
  return false;
}