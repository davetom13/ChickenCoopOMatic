#ifndef DOORMANAGER_H
#define DOORMANAGER_H

#include <Adafruit_MotorShield.h>
#include "lightsensor.h"
#include "latchingswitch.h"
#include "display.h"
#include "utils.h"

enum State { None, Open, Closing, Closed, Opening, RunOnOpen, RunOnClosed, Invalid };

enum CompOperator {
  GreaterThan = 0, 
  LessThan = 1,
  Count = 2
};

const char CompOperatorSymbols[CompOperator::Count] = {'>', '<'};
const int RUNON_TIME = 500;

String doorStateToString(State doorState) {
  switch(doorState) {
    case None:
      return "None";
      break;
    case Closed: 
      return "Closed";
      break;
    case Open:
      return "Open";
      break;
    case Closing:
      return "Closing";
      break;
    case Opening:
      return "Opening";
      break;
    case RunOnOpen:
      return "Running on";
      break;
    case RunOnClosed:
      return "Running on";
      break;
    default:
      return "Invalid";
      break;
  }
}

class DoorManager;

class DoorState {
  protected:
    DoorManager* _doorManager;
    State _state;
  public:
    DoorState(State thisState, DoorManager *doorManager) {
      _state = thisState;
      _doorManager = doorManager;
    }
    virtual ~DoorState() {};
    virtual State work() = 0;
    String getName() {
      return doorStateToString(_state);
    }
};

class DoorManager {
  private:
    LatchingSwitch* _topSwitch;
    LatchingSwitch* _bottomSwitch;
    Adafruit_StepperMotor* _motor;
    DoorState* _state;
    LightSensor* _lightSensor;
    Display* _display;
    int _openLightLevel;
    int _closeLightLevel;
    String _lastStatus;

    void println(String displayString) {
      if (_lastStatus != displayString) {
        Serial.println(displayString);
        _lastStatus = displayString;
      }
    }

  public:
    DoorManager(
        LatchingSwitch* topSwitch,
        LatchingSwitch* bottomSwitch,
        LightSensor* lightSensor,
        Display* display,
        Adafruit_StepperMotor* motor) {
      Serial.println("DoorManager in it");
      _bottomSwitch = bottomSwitch;
      _topSwitch = topSwitch;
      _lightSensor = lightSensor;
      _motor = motor;
      _display = display;
    }

    DoorState* createState(State newState);

    void start() {
      _state = createState(Opening);
    }

    void setState(DoorState* state) {
      delete _state;
      _state = state;
      _topSwitch->reset();
      _bottomSwitch->reset();
    }

    DoorState* getState() {
      return _state;
    }

    void move(int direction) {
      _motor->step(MOVE_STEPS, direction, STEP_TYPE);
    }

    void release() {
      _motor->release();
    }

    void setLightLevel(int direction, int level) {
      switch (direction) {
        case UP:
          _openLightLevel = level;
          break;
        case DOWN:
          _closeLightLevel = level;
          break;
      }
    }

    int getLightLevel(int direction) {
      switch (direction) {
        case UP:
          return _openLightLevel;
          break;
        case DOWN:
          return _closeLightLevel;
          break;
      }
    }

    void displayStatus(String status) {
      _display->printStatus(_state->getName(), status, false);
      println(status);
    }

    void displayProgress(String progress){
      _display->printStatus(_state->getName(), progress, true);
      println(progress);
    }

    void work() {
      State newState = _state->work();
      if (newState != None) {
        Serial.println("Switching states - " + String(freeRam(), DEC) + " bytes free");
        DoorState* oldState = _state;
        _state = createState(newState);
        Serial.println("Freeing old state");
        delete oldState;
      }
    }
};

class NullState : public DoorState {
  public:
    NullState(State thisState, DoorManager* doorManager) :
    DoorState(thisState, doorManager) {
      doorManager->displayStatus("Null state, program frozen");
    }
    State work(){
      return None;
    }
};

class StaticState : public DoorState {
  private:
    unsigned long _lastTime;
    unsigned long _changeDelay;
    LightSensor* _sensor;
    int _trigLightLevel;
    int _newDoorDirection;
    CompOperator _op;
  public:
    StaticState(
        State thisState,
        LightSensor* sensor, 
        CompOperator op, 
        int lightLevel, 
        int newDoorDirection, 
        unsigned long changeDelay, 
        DoorManager* doorManager) :
      DoorState(thisState, doorManager) 
    {
      _sensor = sensor;
      _trigLightLevel = lightLevel;
      _lastTime = millis();
      _newDoorDirection = newDoorDirection;
      _changeDelay = changeDelay;
      _op = op;
      _doorManager->release();
      Serial.println("Door direction: " + directionToString(_newDoorDirection) + " Operator: " + String(_op, DEC) + " or " + CompOperatorSymbols[_op]);
    }
    void printState(int level);
    virtual State work();
};

/*
class StaticState : public DoorState {
  private:
    unsigned long _lastTime;
    unsigned long _changeDelay;
    LightSensor* _sensor;
    int _trigLightLevel;
    int _newDoorDirection;
  public:
    StaticState(LightSensor* sensor, int lightLevel, int newDoorDirection, unsigned long changeDelay, DoorManager* doorManager) :
      DoorState(doorManager) {
      _sensor = sensor;
      _trigLightLevel = lightLevel;
      _lastTime = millis();
      _newDoorDirection = newDoorDirection;
    }

    virtual State work();
};*/

class MovingState : public DoorState {
  private:
    LatchingSwitch* _toSwitch;
    int _direction;
  public:
    MovingState(
      State thisState,
      LatchingSwitch* toSwitch, 
      int direction, 
      DoorManager* doorManager
    ) : DoorState(thisState, doorManager) {
      _toSwitch = toSwitch;
      _direction = direction;
      doorManager->displayStatus("Moving " + directionToString(direction));
      _toSwitch->reset(LOW);
    }

    virtual State work();
};

class RunOnState : public DoorState {
  private:
    unsigned long _endTime;
    int _direction;
    State _nextState;
  public:
    RunOnState(
      State thisState,
      State nextState,
      int direction,
      unsigned long time,
      DoorManager* doorManager
    ) : DoorState (thisState, doorManager) {
      _endTime = millis() + time;
      _direction = direction;
      _nextState = nextState;
    }
   
    virtual State work() {
      if ((int)millis() - _endTime >= 0) {
        return _nextState;
      } 
      else {
        _doorManager->move(_direction);
      }
      return None;
    }
};

State MovingState::work() {
  if (_toSwitch->state() != HIGH) {
    _doorManager->move(_direction);
  }
  else {
    Serial.println("Have arrived!");
    State nextState; //= State::Open;
    Serial.println("Direction: " + String(_direction, DEC) + " or " + directionToString(_direction));
    Serial.println("DOWN == " + String(DOWN, DEC) + " UP == " + String(UP, DEC));
    switch (_direction) {
      case DOWN: 
        nextState = State::RunOnClosed;
        break;
      case UP:
        nextState = State::RunOnOpen;
        break;
      default:
        nextState = State::None;
    }
    Serial.println("Returning new state: " + doorStateToString(nextState));
    return nextState;
  }
  return None;
}


void StaticState::printState(int level) {
  _doorManager->displayStatus("Trig @ " + String(level, DEC) + " "  + CompOperatorSymbols[_op] + " " + String(_trigLightLevel, DEC));
}

State StaticState::work() {
  unsigned long newTime = millis();
  if (newTime - _lastTime >= _changeDelay) {
    int level = _sensor->read();
    _lastTime = newTime;
    printState(level);
    switch (_op) {
      //case DOWN: //proper
      case LessThan:
        if (level < _trigLightLevel) {
          return Closing;
        }
        break;
      //case UP: //proper
      case GreaterThan:
        if (level > _trigLightLevel) {
          return Opening;
        }
        break;
    }
  }
  return None;
}

DoorState* DoorManager::createState(State newState) {
  switch (newState) {
    case Opening:
      println("Switch to opening");
      return new MovingState(newState, _topSwitch, UP, this);
      break;
    case Closing:
      println("Switch to closing");
      return new MovingState(newState, _bottomSwitch, DOWN, this);
      break;
    case Closed:
      println("Switch to closed");
      return new StaticState(newState, _lightSensor, GreaterThan, _openLightLevel, UP, 1000, this);
      break;
    case Open:
      println("Switch to open");
      return new StaticState(newState, _lightSensor, LessThan, _closeLightLevel, DOWN, 1000, this);
      break;
    case RunOnOpen:
      println("Running on");
      return new RunOnState(newState, State::Open, DOWN, RUNON_TIME, this);
      break; 
    case RunOnClosed:
      println("Running on");
      return new RunOnState(newState, State::Closed, DOWN, RUNON_TIME, this);
      break; 
    default:
      println("Switch to Null");   
      return new NullState(newState, this);
      break;
  }
}

#endif
