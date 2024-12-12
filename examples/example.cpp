#include <Arduino.h>
#include "MAX31855.h"

// Can be any pins
#define SCK_PIN  10
#define CS_PIN   11
#define MISO_PIN 12

MAX31855 tc(SCK_PIN, CS_PIN, MISO_PIN);

void setup()
{
    Serial.begin(9600);
    Serial.println("MAX31855 Example");

    tc.begin();
}

void loop()
{
    float temp, tempf, internal_temp, internal_tempf;

    temp  = tc.getTemperature(); // Get temperature in Celsius
    // Check for errors
    if (isnanf(temp)) {
        Serial.print("Failed to read temperature: "); 
        Serial.println(tc.getErrorString());
    } else {
        Serial.print("temp = ");
        Serial.print(temp);
        Serial.print(" Celsius");
        Serial.println();
    }

    tempf = tc.getTemperatureFahrenheit(); // Get temperature in Fahrenheit
    // Check for errors
    if (isnanf(tempf)) {
        Serial.print("Failed to read temperature: "); 
        Serial.println(tc.getErrorString());
    } else {
        Serial.print("temp = ");
        Serial.print(tempf);
        Serial.print(" Fahrenheit");
        Serial.println();
    }

    internal_temp  = tc.getInternalTemperature(); // Get temperature of module in Celsius
    // Check for errors
    if (isnanf(internal_temp)) {
        Serial.print("Failed to read temperature: "); 
        Serial.println(tc.getErrorString());
    } else {
        Serial.print("internal temp = ");
        Serial.print(internal_temp);
        Serial.print(" Celsius");
        Serial.println();
    }

    internal_tempf = tc.getInternalTemperatureFahrenheit(); // Get temperature of module in Fahrenheit
    // Check for errors
    if (isnanf(internal_tempf)) {
        Serial.print("Failed to read temperature: "); 
        Serial.println(tc.getErrorString());
    } else {
        Serial.print("internal temp = ");
        Serial.print(internal_tempf);
        Serial.print(" Fahrenheit");
        Serial.println();
    }

    delay(1000);
}