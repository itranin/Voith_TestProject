#include <iostream>
#include <sstream>
#include <fstream>
#include "signal_proc.h"


ZSignal::ZSignal() {
    counter = 0;
    windowSize = WINDOW_SIZE;
    value = 0.0f;
    value0 = 0.0f;
    anomalyCounter = 0;
    // Initialize Kalman filter
    kalmanFilter.init(KALMAN_Q, KALMAN_ERR);
}

//**********************************************************************************************************
/// @brief Signal processing: scaling, filtering, outlier detection
/// @param rawValue adc raw value
/// @param filtered float reference to store filtered value
/// @return 0 if no errors detected otherwise returns anomalyCounter
uint32_t ZSignal::processValue(float rawvalue, float &filtered) {
    value0 = value;
    rawValue = rawvalue;
    // Apply filtering
    value = kalmanFilter.process(rawValue);
    // calculate signal statistic (average and variance in window)
    if(!stat.process(value)) anomalyCounter++;
    else { 
        if(anomalyCounter) stat.reset();
        anomalyCounter=0;
    }
    filtered = value;
    return anomalyCounter;
}

//**********************************************************************************************************