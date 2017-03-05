#ifndef LIGHTSENSOR_H
#define LIGHTSENSOR_H 
 
#include "sensorbuffer.h"

class LightSensor {
  private:
    SensorBuffer<20> _buffer;
    int _pin;
  public:
    LightSensor (int analogPin) {
      _pin = analogPin;
    }

    int read() {
      int value = analogRead(_pin);
      _buffer.add(value);
      int result = _buffer.average();
      Serial.println("Sensor " + String(value, DEC) + " average " + String(result, DEC));
      return result;
    }

    void reset() {
      _buffer.reset();
    }
};

#endif
