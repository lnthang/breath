/**************************************************************************/
/*!
@file     MQ135.cpp
@author   G.Krocker (Mad Frog Labs)
@license  GNU GPLv3

First version of an Arduino Library for the MQ135 gas sensor
TODO: Review the correction factor calculation. This currently relies on
the datasheet but the information there seems to be wrong.

@section  HISTORY

v1.0 - First release
*/
/**************************************************************************/

#include "include/MQ135.h"

/**************************************************************************/
/*!
@brief  Default constructor

@param[in] pin  The analog input pin for the readout of the sensor
*/
/**************************************************************************/

MQ135::MQ135(uint8_t pin) {
  _pin = pin;
  // initialize ADC
    adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_channel_atten((adc1_channel_t)_pin,ADC_ATTEN_11db);
}


/**************************************************************************/
/*!
@brief  Get the correction factor to correct for temperature and humidity

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The calculated correction factor
*/
/**************************************************************************/
static const float CORA33 = 0.000351299074397;
static const float CORB33 = (-0.026020377005075);
static const float CORC33 = 1.387122539739613;

    
static const float CORA85 = 0.000313998905691 ;
static const float CORB85 = (-0.023431434060233);
static const float CORC85 = 1.253477179168087;
float MQ135::getCorrectionFactor(float t, float h) {

  float factor33 = CORA33 * t * t + CORB33 * t + CORC33;
  float factor85 = CORA85 * t * t + CORB85 * t + CORC85;

  float factorAt1 = (factor33 - factor85) / (85. - 33.);

  return (factor33 - factorAt1 * (h - 33.));

// return CORA * t * t + CORB * t + CORC - (h-33.)*CORD;
}

/**************************************************************************/
/*!
@brief  Get the resistance of the sensor, ie. the measurement value

@return The sensor resistance in kOhm
*/
/**************************************************************************/
float MQ135::getResistance() {
  // int val = analogRead(_pin);
  int val = 0;

  #define MAX_ADC_READ 10
  int totalVal = 0;
  for (int i = 0; i < MAX_ADC_READ; i++)
  {
    totalVal += adc1_get_voltage((adc1_channel_t)_pin);
  }
  val = totalVal / MAX_ADC_READ;

  return ((MAX_ADC/(float)val) * VCC - 1.)*RLOAD;
}

/**************************************************************************/
/*!
@brief  Get the raw ADC of the sensor, ie. the measurement value

@return The voltage
*/
/**************************************************************************/
float MQ135::getRawADC() {
  // int val = analogRead(_pin);
  int val = 0, totalVal = 0;
  for (int i = 0; i < MAX_ADC_READ; i++)
  {
    totalVal += adc1_get_voltage((adc1_channel_t)_pin);
  }
  val = totalVal / MAX_ADC_READ;

  return val;
}

/**************************************************************************/
/*!
@brief  Get the resistance of the sensor, ie. the measurement value corrected
        for temp/hum

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The corrected sensor resistance kOhm
*/
/**************************************************************************/
float MQ135::getCorrectedResistance(float t, float h) {
  return getResistance()/getCorrectionFactor(t, h);
}

/**************************************************************************/
/*!
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air)

@return The ppm of CO2 in the air
*/
/**************************************************************************/
float MQ135::getPPM() {
  return PARA * pow((getResistance()/RZERO), -PARB);
}

/**************************************************************************/
/*!
@brief  Get the ppm of CO2 sensed (assuming only CO2 in the air), corrected
        for temp/hum

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The ppm of CO2 in the air
*/
/**************************************************************************/
float MQ135::getCorrectedPPM(float t, float h) {
  return PARA * pow((getCorrectedResistance(t, h)/RZERO), -PARB);
}

/**************************************************************************/
/*!
@brief  Get the resistance RZero of the sensor for calibration purposes

@return The sensor resistance RZero in kOhm
*/
/**************************************************************************/
float MQ135::getRZero() {
  return getResistance() * pow((ATMOCO2/PARA), (1./PARB));
}
//17.6 * 
/**************************************************************************/
/*!
@brief  Get the corrected resistance RZero of the sensor for calibration
        purposes

@param[in] t  The ambient air temperature
@param[in] h  The relative humidity

@return The corrected sensor resistance RZero in kOhm
*/
/**************************************************************************/
float MQ135::getCorrectedRZero(float t, float h) {
  return getCorrectedResistance(t, h) * pow((ATMOCO2/PARA), (1./PARB));
}
