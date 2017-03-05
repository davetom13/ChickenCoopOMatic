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
      _lastRead = -1;
      Serial.println("Switch " + _name + " reset to " + stateToString(_state));
    }

    int state() {
      /*if (!_latched) {
        int newState = digitalRead(_pin);
        if (newState != _state) {
          _latched = true;
          Serial.println(_name + " latched " + stateToString(newState));
        }
        _state = newState;
      }*/
      
      if (!_latched) {
        int now = millis();
        int newState = digitalRead(_pin);
        if (newState != _state) {
          if (newState == _lastRead) {
            if ((int)now - _lastTime >= 0) {
              if (newState != _state) {
                _latched = true;
                Serial.println("Switch " + _name + " has gone from " + stateToString(_state) + " to " + stateToString(newState));
                Serial.println(_name + " latched " + stateToString(newState));
              }
              _state = newState;
            }
            else {
              Serial.println("DEBOUNCE! Time: " + String(now - _lastRead));
            }
          }
          else {
            _lastTime = now + DEBOUNCE_TIME;
            _lastRead = newState;
          }
        }
      }
      return _state;
    }
};

#endif

