# MAX31855 Arduino Library

A simple Arduino library to communicate with the MAX31855 temperature module.

# Example

```C++
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
    float temp;

    temp = tc.getTemperature(); // Get temperature in Celsius

    // Check for errors
    if (isnanf(temp)) {
        Serial.println("Failed to read temperature: "); 
        Serial.println(tc.getErrorString());
    } else {
        Serial.print("temp = ");
        Serial.print(temp);
        Serial.print(" Celsius");
        Serial.println();
    }

    delay(500);
}
```

You can also check the [examples](examples) directory for examples.