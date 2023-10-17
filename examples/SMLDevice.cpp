/*
  CryptoSML - SMLDevice

  (implementation)
*/

#include "SMLDevice.h"
#include <sml_unit.h>

SMLDevice::SMLDevice(const char * name, int8_t _rxPin, int8_t _txPin, bool _invert) : serial(rxPin, txPin, invert), json(2048) {
  devicename = name;
  rxPin = _rxPin;
  txPin = _txPin;
  invert = _invert;
}

void SMLDevice::loop() {
  if (serial.available()) {
    printDebug("read");
    memset(buffer, 0, sizeof(buffer) / sizeof(buffer[0]));
    buffer_len = serial.readBytes(buffer, sizeof(buffer) / sizeof(buffer[0]));
    serial.flush();
  }
}

void SMLDevice::setup() {
  PersistentMemory pmem(devicename, true);
  serial.begin(9600, SWSERIAL_8N1, rxPin, -1, invert, 600);
  serial.enableTx(false);
  serial.enableRx(true);
  serial.setTimeout(50);
}

ProcessMessageStruct SMLDevice::processMessage(String &message) {

  if (message == COMMAND_RAWSML) {
    String out;
    out.reserve(buffer_len);
    for (int i = 0; i<buffer_len; i++){
      out += (char)buffer[i];
    }
    return {DATA, out, FLAG_BINARY};
  }

  if (message == COMMAND_PARSEDSML) {
    return {DATA, parseSML(buffer, sizeof(buffer) / sizeof(buffer[0]))};
  }

  return {ERR, "NO_COMMAND"};
}

String SMLDevice::getName() {
  return devicename;
}

String SMLDevice::getStatus() {
  return "";
}

String SMLDevice::getType() {
  return "SML";
}

String SMLDevice::parseSML(uint8_t * buffer, int buffer_len) {
  json.clear();
  JsonArray jsonArray = json.to<JsonArray>();
  if (buffer_len > 16) {
    sml_file *file = sml_file_parse(buffer + 8, buffer_len - 16);
    for (int i = 0; i < file->messages_len; i++) {
      sml_message *message = file->messages[i];
      if (*message->message_body->tag == SML_MESSAGE_GET_LIST_RESPONSE) {
        sml_list *entry;
        sml_get_list_response *body;
        body = (sml_get_list_response *)message->message_body->data;
        for (entry = body->val_list; entry != NULL; entry = entry->next) {
          if (!entry->value) { // do not crash on null value
            continue;
          }

          char obisIdentifier[32];
          sprintf(obisIdentifier, "%d-%d:%d.%d.%d/%d", entry->obj_name->str[0], entry->obj_name->str[1], entry->obj_name->str[2], entry->obj_name->str[3], entry->obj_name->str[4], entry->obj_name->str[5]);
          JsonObject jsonObj = jsonArray.createNestedObject();
          jsonObj["id"] = obisIdentifier;

          if (((entry->value->type & SML_TYPE_FIELD) == SML_TYPE_INTEGER) || ((entry->value->type & SML_TYPE_FIELD) == SML_TYPE_UNSIGNED)) {
            double value = sml_value_to_double(entry->value);
            int scaler = (entry->scaler) ? *entry->scaler : 0;
            value = value * pow(10, scaler);
            jsonObj["val"] = value;
          }

          if (entry->value->type == SML_TYPE_OCTET_STRING) {
            char *value;
            sml_value_to_strhex(entry->value, &value, true);
            jsonObj["val"] = value;
            free(value);
          }

          if (entry->value->type == SML_TYPE_BOOLEAN) {
            jsonObj["val"] = (int)entry->value->data.boolean;
          }

          const char *unit = NULL;
          if (entry->unit) {
            unit = dlms_get_unit((unsigned char) * entry->unit);
            jsonObj["unit"] = unit;
          }
        }
      }
    }
    sml_file_free(file);
  }
  String out;
  out.reserve(1024);
  serializeJson(json, out);
  out.replace("},{", "},\n{");
  return out;
}
