/*
  CryptoSML - SMLDevice

  TODO
*/

#pragma once

#include "AllConfig.h"

#include <Device.h>
#include <PersistentMemory.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <sml_file.h>

const char COMMAND_RAWSML[] = "rawsml";
const char COMMAND_PARSEDSML[] = "parsedsml";

const char SML_START[] = "\x1B\x1B\x1B\x1B\x01\x01\x01\x01";
const char SML_STOP[] = "\x1B\x1B\x1B\x1B\x1A";

class SMLDevice : public Device {
  public:
    //Here we process the plaintext commands and generate an answer for the client.
    SMLDevice(const char * name, int8_t _rxPin, int8_t _txPin, bool _invert=false);
    ProcessMessageStruct processMessage(String &message);
    String getName();
    String getStatus();
	String getType();
    void loop();
    void setup();

  private:
    SoftwareSerial serial;
    int rxPin;
    int txPin;
    bool invert;
    int buffer_len = 0;
    String devicename;
    String parseSML(uint8_t * buffer, int buffer_len);

    DynamicJsonDocument json;
    uint8_t buffer[800];
};
