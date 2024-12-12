#include "MAX31855.h"

// datasheet specifies 100ns so 10us are easily enough
const unsigned int SPI_WAIT_TIME = 10U;

// Error code of enum MAX31855_ERROR is entry in array
const String errorCode2String[] = {
    "ERROR_NONE",
    "ERROR_OPEN_CIRCUIT",
    "ERROR_GND_SHORT",
    "ERROR_VCC_SHORT",
    "ERROR_READ",
    "ERROR_NOT_INITIALIZED",
};


/// @brief Initialize the pins of the module
/// @param SCK  clock pin
/// @param CS   chip select pin, normally high
/// @param MISO slave out pin / data pin
MAX31855::MAX31855(uint8_t SCK, uint8_t CS, uint8_t MISO) : sck(SCK), cs(CS), miso(MISO), error(MAX31855_NOT_INITIALIZED) {}

/// @brief Configure the pins of the module
void MAX31855::begin(void)
{
    pinMode(sck,  OUTPUT);
    pinMode(cs,   OUTPUT);
    pinMode(miso, INPUT);

    // disable reading
    digitalWrite(cs, HIGH);
    // set clock to LOW, next bit is presented on negative clock edge
    digitalWrite(sck, LOW);

    error = MAX31855_ERROR_NONE;
}

/// @brief Get the temperature of the attached thermocouple as float in degrees Celsius
/// @return The read temperatur or NAN on error, call getError() for more information
float MAX31855::getTemperature(void)
{
    uint32_t d;
    float t;

    if (error == MAX31855_NOT_INITIALIZED) return NAN;

    d = readSensor();
    if (setError(d)) return NAN;

    // temperature bits are D30-D18
    d >>= 18;

    // grab lower 13 bits
    t = (float)(d & 0x1FFF);

    // if sign bit is set invert value
    if (d & (1 << 13)) t *= -1.0f;

    // resolution is 0.25 Celsius per bit
    return t * 0.25f;
}

/// @brief Get the temperature of the attached thermocouple as float in degrees Fahrenheit
/// @return The read temperatur or NAN on error, call getError() for more information
float MAX31855::getTemperatureFahrenheit(void)
{
    float t;

    t = getTemperature();
    t = t * 1.8f + 32.0f;

    return t;
}

/// @brief Get the temperature of the module itself as float in degrees Celsius
/// @return The read temperatur or NAN on read error
float MAX31855::getInternalTemperature(void)
{
    uint32_t d;
    float t;

    if (error == MAX31855_NOT_INITIALIZED) return NAN;

    d = readSensor();
    // ignore other errors since thermocouple doesn't need 
    // to be attached to read internal temperature
    if (setError(d) && error == MAX31855_ERROR_READ) return NAN;

    // internal temperature bits are D14-D4
    d >>= 4;

    // grab lower 11 bits
    t = (float)(d & 0x7FF);

    // if sign bit is set invert value
    if (d & (1 << 11)) t *= -1.0f;

    // resolution is 0.0625 Celsius per bit
    return t * 0.0625f;
}

/// @brief Get the temperature of the module itself as float in degrees Fahrenheit
/// @return The read temperatur or NAN on read error
float MAX31855::getInternalTemperatureFahrenheit(void)
{
    float t;

    t = getInternalTemperature();
    t = t * 1.8f + 32.0f;

    return t;
}

/// @brief Get the error code, see MAX3188::ERROR enum for all values
/// @return The error code
int MAX31855::getErrorCode(void)
{
    return error;
}

/// @brief Get the error as string, see errorCode2String[] for values
/// @return The error code as string
String MAX31855::getErrorString(void)
{
    return errorCode2String[error];
}

/// @brief Read a full datagram (32 bits) from the module
/// @return The datagram that was read, first read bit is MSB
uint32_t MAX31855::readSensor(void)
{
    uint32_t data;
    int8_t i;

    // force stop any on-going conversion process, this also presents the first bit on the miso pin
    digitalWrite(cs, LOW);
    delayMicroseconds(SPI_WAIT_TIME);

    // read datagram
    data = 0;
    for (i = 31; i >= 0; i--) {
        // read bit
        if (digitalRead(miso)) {
            // shift bit into result
            data |= (1UL << i);
        }

        // this doesn't present a new bit
        digitalWrite(sck, HIGH);
        delayMicroseconds(SPI_WAIT_TIME);
        // ask for next bit
        digitalWrite(sck, LOW);
        delayMicroseconds(SPI_WAIT_TIME);
    }
    
    // initiate a new conversion proccess
    digitalWrite(cs, HIGH);

    return data;
}

/// @brief Check datagram for errors
/// @param data The data from the module
/// @return true if an error occured, otherwise false
bool MAX31855::setError(uint32_t data)
{
    uint8_t fault, oc, scg, scv;

    error = MAX31855_ERROR_NONE;

    // D16 is fault bit
    fault = (data >> 16UL) & 1;
    // D0 is open circuit bit
    oc    = (data >>  0UL) & 1;
    // D1 is short circuit to GND bit
    scg   = (data >>  1UL) & 1;
    // D2 is short circuit to VCC bit
    scv   = (data >>  2UL) & 1;

    // D17 and D3 are reserved and should always be zero
    if ((data >> 17UL) & 1 || (data >> 3UL) & 1) error = MAX31855_ERROR_READ;
    // if one of the error bits is set but not fault bit data is corrupted
    else if ((oc || scg || scv) && !fault)   error = MAX31855_ERROR_READ;
    // if fault bit is set but none of the error bits data is corrupted
    else if (fault && !(oc || scg || scv))   error = MAX31855_ERROR_READ;
    // data is probably not corrupted so check for remaining errors
    else if (oc)  error = MAX31855_ERROR_OPEN_CIRCUIT;
    else if (scg) error = MAX31855_ERROR_GND_SHORT;
    else if (scv) error = MAX31855_ERROR_VCC_SHORT;

    // if no error occured error = 0
    return error;
}