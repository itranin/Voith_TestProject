#ifndef  SIGNAL_PROC_H
#define SIGNAL_PROC_H

#include <cmath>
#include <vector>
#include <fstream>
#include "signal_gen.h"


#define WINDOW_SIZE 100 // moving average window size
#define KALMAN_Q 0.01f // Kalman filter process quality
#define KALMAN_ERR 1.0f // Kalman filter measurement error

// Kalman filter class
class Kalman {
public:
  float G,Q,ErrMes,ErrEst,LastEst,CurEst;
  Kalman() 
  { 
    ErrMes=0;
    CurEst=0;
    LastEst=0;
  };

// q - process quality 0..1
// errMes - measurement error in mV
  void init(float q,float errMes)
  {
    Q=q;
    ErrMes=errMes;
    ErrEst=ErrMes;
  }
  float process(float mes)
  {
    G = ErrEst/(ErrEst+ErrMes);
    CurEst = LastEst+G*(mes - LastEst);   
    ErrEst = (1.0-G)*ErrEst+fabs(LastEst-CurEst)*Q;
    LastEst=CurEst;
    return CurEst;
  }
};


// Signal statistics class for moving average and variance calculation
class SignalStatistic {
  double n;
public:
    float avg;
    float variance;
    float sigma;
    SignalStatistic( int windowSize = WINDOW_SIZE) {
        avg = 0.0f;
        reset();
    }
    void reset() {
        variance = 0.0f;
        sigma = 0.0f;
        n = 0;
    }
    bool process (float value) {
      //if(isAnomaly(value)) return false; // do not process anomalies
      avg = avg * (n / (n + 1)) + value / (n + 1);
      if (n < WINDOW_SIZE) n += 1; 
      variance = variance * (n - 1) / n + (value - avg) * (value - avg) / n;
      sigma = sqrt(variance);
      if(isAnomaly(value)) return false; // do not process anomalies
      return true;
    }
    // Check if value > 3 sigma
    bool isAnomaly(float value) {
        if ((variance > 0) && (n >= WINDOW_SIZE)) {
            return fabs(value - avg) > (3.0 * sigma);
        }
        return false;
    }
};


class ZSignal {
  uint32_t counter;
  int windowSize;
  Kalman kalmanFilter;
public:
  uint32_t anomalyCounter;
  int   index;
  float rawValue;   // input signal value
  float value;      // filtered signal value
  float value0;     // previous filtered signal value
  SignalStatistic stat;

  ZSignal();
  uint32_t processValue(float rawValue, float &filtered); // returns anomaly counter
};


#endif // SIGNAL_PROC_H
