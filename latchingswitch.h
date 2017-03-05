#ifndef LATCHINGSWITCH_H
#define LATCHINGSWITCH_H

#include "utils.h"

const int DEBOUNCE_TIME = 500;

class LatchingSwitch {
  private:
    boolean _latched;
    int _pin;
    int _state;
    int _lastRead;
    int _lastTime;
    int _deboundMS;
    String _name;
  public:
    LatchingSwitch(int pin, String switchName) {
      _latched = false;
      _pin = pin;
      _name = switchName;
      reset();
    }

    void reset() {
      reset(digitalRead(_pin));
    }

    void reset(int state) {
      _latched = false;
      _state = state;
      _lastRead = _state;
      Serial.println("Switch " + _name + " reset to " + stateToString(_state));
    }

    int state() {
      if (!_latched) {
        int now = millis();
        int newState = digitalRead(_pin);
  
        if (newState == _lastRead) {
          if (now - _lastTime > DEBOUNCE_TIME) {
            if (newState != _state) {
              Serial.println("Switch " + _name + " has gone from " + stateToString(_state) + " to " + stateToString(newState));
            }
            if (newState != _state) {
              _latched = true;
              Serial.println(_name + " latched " + stateToString(newState));
            }
            _state = newState;
          }
          else {
            Serial.println("DEBOUNCE!");
          }
        }
        else {
          _lastTime = now;
          _lastRead = newState;
        }
      }
      return _state;
    }
};

#endif

