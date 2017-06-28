
/* DHT library

MIT license
written by Adafruit Industries
*/
#ifndef DHT_H
#define DHT_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 // #include "WProgram.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#endif


// Uncomment to enable printing out nice debug messages.
//#define DHT_DEBUG

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

// Setup debug printing macros.
#ifdef DHT_DEBUG
  #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
  #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
  #define DEBUG_PRINT(...) {}
  #define DEBUG_PRINTLN(...) {}
#endif

enum heatIndexClassification_t
{
  _NORMAL = 0,
  _FATIGUE,
  _POSSIBLE_EXHAUSTION,
  _LIKELY_HEAT_STROKE,
  _HIGHLY_HEAT_STROKE,
  _TOO_DANGEROUS,
};


const static float heatIndexClassificationInF[(int)_TOO_DANGEROUS+2] = {0, 80, 90, 103, 124, 125, 200};
const static float heatIndexClassificationInC[(int)_TOO_DANGEROUS+2] = {-17.78, 26.67, 32.22, 39.94, 51.11, 51.67, 93.33};


// Define types of sensors.
#define DHT11 11
#define DHT22 22
#define DHT21 21
#define AM2301 21


class DHT 
{
  public:
    DHT(uint8_t pin, uint8_t type, uint8_t count=6);
    void begin(void);
    float readTemperature(bool S=false, bool force=false);
    float convertCtoF(float);
    float convertFtoC(float);
    float computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit=true);
    float readHumidity(bool force=false);
    float computeDewPointInCelcius(float temperature, float percentHumidity);
    heatIndexClassification_t heatIndexClassification(float temperature, bool isFahrenheit);
    bool read(bool force=false);

  private:
    uint8_t data[5];
    uint8_t _pin, _type;
    #ifdef __AVR
    // Use direct GPIO access on an 8-bit AVR so keep track of the port and bitmask
    // for the digital pin connected to the DHT.  Other platforms will use digitalRead.
    uint8_t _bit, _port;
    #endif
    uint32_t _lastreadtime, _maxcycles;
    bool _lastresult;

    uint32_t expectPulse(bool level);
};

class InterruptLock 
{
  public:
   InterruptLock() {
//    noInterrupts();
   }
   ~InterruptLock() {
//    interrupts();
   }

};

#endif
