#ifndef MAX31855_H_
#define MAX31855_H_

#include <Arduino.h>

typedef enum {
    MAX31855_ERROR_NONE,         // No error occured
    MAX31855_ERROR_OPEN_CIRCUIT, // No thermocouple is attached
    MAX31855_ERROR_GND_SHORT,    // Thermocouple short to GND
    MAX31855_ERROR_VCC_SHORT,    // Thermocouple short to VCC
    MAX31855_ERROR_READ,         // Reading error occured
    MAX31855_NOT_INITIALIZED,    // Module is not initialized, call begin() first
} MAX31855_Error;

class MAX31855 {
public:
    MAX31855(uint8_t SCK, uint8_t CS, uint8_t MISO);

    void   begin(void);
    float  getTemperature(void);
    float  getTemperatureFahrenheit(void);
    float  getInternalTemperature(void);
    float  getInternalTemperatureFahrenheit(void);
    int    getErrorCode(void);
    String getErrorString(void);

private:
    uint8_t sck, cs, miso;
    int error;
    uint32_t readSensor(void);
    bool setError(uint32_t data);
};

#endif // MAX31855_H_