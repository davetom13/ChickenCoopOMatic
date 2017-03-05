#ifndef UTILS_H
#define UTILS_H

const int UP = FORWARD;
const int DOWN = BACKWARD;
const int MOVE_STEPS = 100;
const int STEP_TYPE = SINGLE; //DOUBLE, SINGLE, INTERLEAVE, MICROSTEP

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

String stateToString (int state) {
  if (state == HIGH) {
    return "HIGH";
  }
  else {
    return "LOW";
  }
}

String directionToString (int direction) {
  if (direction == UP) {
    return "UP";
  }
  else if (direction == DOWN) {
    return "DOWN";
  } else {
    return "NONE";
  }

}

String directionToSymbol (int direction) {
  if (direction == UP) {
    return ">";
  }
  else if (direction == DOWN ) {
    return "<";
  } else {
    return "-";
  }
}


#endif
