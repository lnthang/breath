/**************************************************************************/
/*!
@file     MQ135.h
@author   G.Krocker (Mad Frog Labs)
@license  GNU GPLv3

First version of an Arduino Library for the MQ135 gas sensor
TODO: Review the correction factor calculation. This currently relies on
the datasheet but the information there seems to be wrong.

@section  HISTORY

v1.0 - First release
*/
/**************************************************************************/
#ifndef MQ135_H
#define MQ135_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
// #include "WProgram.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <driver/adc.h>
#endif

/// The load resistance on the board
#define RLOAD 1.0
/// Calibration resistance at atmospheric CO2 level
#define RZERO 52.1690//33.976285
/// Parameters for calculating ppm of CO2 from sensor resistance
#define PARA 116.6020682
#define PARB 2.769034857

/// Parameters to model temperature and humidity dependence
// #define CORA 0.00035
// #define CORB 0.02718
// #define CORC 1.39538
// #define CORD 0.0018

// #define CORA33 0.000351299074397f
// #define CORB33 (-0.026020377005075f)
// #define CORC33 1.387122539739613f

    
// #define CORA5 0.000313998905691f 
// #define CORB85 (-0.023431434060233f)
// #define CORC85 1.253477179168087f


/// Atmospheric CO2 level for calibration purposes
#define ATMOCO2 409.01

#define MAX_ADC 4095.0//1023.0
#define VCC     3.3//5.0

class MQ135 {
 private:
  uint8_t _pin;

 public:
  MQ135(uint8_t pin);
  float getRawADC();
  float getCorrectionFactor(float t, float h);
  float getResistance();
  float getCorrectedResistance(float t, float h);
  float getPPM();
  float getCorrectedPPM(float t, float h);
  float getRZero();
  float getCorrectedRZero(float t, float h);
};
#endif
