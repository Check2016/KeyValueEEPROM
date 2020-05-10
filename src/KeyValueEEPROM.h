#ifndef KeyValueEEPROM_h
#define KeyValueEEPROM_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>

#ifndef KeyValueEEPROM_SIZE
  #if defined(ESP8266) || defined(ESP32) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    #define KeyValueEEPROM_SIZE 4096
  #endif
  #if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega32U4__)
    #define KeyValueEEPROM_SIZE 1024
  #endif
  #if defined(__AVR_ATmega168__)
    #define KeyValueEEPROM_SIZE 512
  #endif
#endif

class KeyValueEEPROMClass {
  public:
        // Public methods
        void begin() {
                // If this method was already called, return.
                if (started) return;

                // Read JSON data from the EEPROM.
                String json = readEEPROM();

                // Parse JSON data.
                DeserializationError error = deserializeJson(jsonDocument, json);

                // If JSON couldn't be parsed,
                if (error) {
                    // then create an new key-value store.
                    clear();
                }
        };
        void clear(bool allEEPROM = false) {
                // Clear buffer.
                jsonDocument.clear();

                // Clear EEPROM if requested
                if (allEEPROM)
                    clearEEPROM();
        };
        void remove(String key) {
                // Remove key.
                jsonDocument.remove(key);
        };
        void apply() {
                // Extract JSON data from the root object.
                String json;
                serializeJson(jsonDocument, json);

                // Write JSON data to the EEPROM.
                writeEEPROM(json);
        };
        bool exists(String key) {
                return jsonDocument.containsKey(key);
        };
        template <typename T>
        T get(String key) {
                T value = jsonDocument[key].as<T>();
                return value;
        };
        template <typename T>
        void set(String key, T value) {
                jsonDocument[key] = value;
        };

  private:
    // Private methods
    void clearEEPROM() {
            #if defined(ESP8266) || defined(ESP32)
            // Begin EEPROM library.
            EEPROM.begin(KeyValueEEPROM_SIZE);
            #endif

            // Write all bytes to zero.
            for (int i = 0; i < KeyValueEEPROM_SIZE; i++) EEPROM.write(i, '\0');

            // End EEPROM library.
            EEPROM.end();
    };
    String readEEPROM() {
            #if defined(ESP8266) || defined(ESP32)
            // Begin EEPROM library.
            EEPROM.begin(KeyValueEEPROM_SIZE);
            #endif

            // Read JSON data until null-termination.
            String string = ""; unsigned int i; unsigned char byte;
            for (i = 0; i < KeyValueEEPROM_SIZE; i++) {
                    byte = EEPROM.read(i);
                    if (byte == '\0') {
                            break;
                    } else {
                            string += char(byte);
                    }
            }

            // End EEPROM library.
            EEPROM.end();

            // Return JSON data.
            return string;
    };
    void writeEEPROM(String json) {
            #if defined(ESP8266) || defined(ESP32)
            // Begin EEPROM library.
            EEPROM.begin(KeyValueEEPROM_SIZE);
            #endif

            // Write JSON data.
            unsigned int i;
            for (i = 0; i < json.length(); i++) {
                    EEPROM.write(i, json.charAt(i));
            }

            // If the length of the JSON data is smaller than the EEPROM size,
            if (json.length() < KeyValueEEPROM_SIZE)
                    // than write at the last position a null-termination.
                    EEPROM.write(json.length(), '\0');

            // End EEPROM library.
            EEPROM.end();
    }

    // Properties
    StaticJsonDocument<KeyValueEEPROM_SIZE> jsonDocument;
    bool started = false;
};

static KeyValueEEPROMClass KeyValueEEPROM;
extern KeyValueEEPROMClass KeyValueEEPROM;

#endif
