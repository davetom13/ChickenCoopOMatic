#ifndef SENSORBUFFER_H
#define SENSORBUFFER_H

template <int N>
class SensorBuffer {
  private:
    int _buffer[N];
    int _currentIndex = 0;
    long _sum = 0;
    int _samples = 0;
    
  public:
    SensorBuffer() {
      reset();
    }

    void add(int value) {
      if (value < 0) {
        value = 0;
      }
      if (_samples < N) {
        //Serial.println("Buffer filling. Samples: " + String(_samples, DEC) + " Max: " + String(N, DEC));
        _buffer[_samples++] = value;
        _currentIndex = _samples;
        _sum += value;
      }
      else {
        _currentIndex = _currentIndex % N;
        int& oldest = _buffer[_currentIndex];
        _sum += value - oldest;      
        //Serial.println("Buffer full, index: " + String(_currentIndex, DEC) + " oldest:" + String(oldest, DEC) + " _sum: " + String(_sum, DEC));
        _currentIndex++;
        //Serial.println("Next index: " + String(_currentIndex, DEC));
        oldest = value;
      }
    }

    int average() {
      return _sum / _samples;  
    }

    void reset() {
      _currentIndex = 0;
      _sum = 0;
      _samples = 0;      
    }
};

#endif
