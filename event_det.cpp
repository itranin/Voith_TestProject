#include <iostream>
#include "event_det.h"

//**********************************************************************************************************
AquisitionModule::~AquisitionModule() {
    if(rawDataFile.is_open()) rawDataFile.close();
    if(filteredDataFile.is_open()) filteredDataFile.close();
    if(logFile.is_open()) logFile.close();    
}
//**********************************************************************************************************
void AquisitionModule::initSignals(int numSignals) {
    counter = 0;
    counter500 = 0;
    ledState = false;
    anomalySignals0 = 0;
    for(int i=0;i<numSignals;i++) {
        ZSignal sig;
        signals.push_back(sig);      
    }
}
//**********************************************************************************************************
int AquisitionModule::saveDataTo(char const* rawFilePath, char const* filteredFilePath, char const* logFilePath) {
    
    debugFile.open("debug.csv");
    statFile.open("stat.csv");

    rawDataFile.open(rawFilePath);
    if (!rawDataFile.is_open()) {
        std::cerr << "Error opening raw data file: " << rawFilePath << std::endl;
        return -1;
    }
 
    filteredDataFile.open(filteredFilePath);
    if (!filteredDataFile.is_open()) {
        std::cerr << "Error opening filtered data file: " << filteredFilePath << std::endl;
        return -2;
    }
    logFile.open(logFilePath);
    if (!logFile.is_open()) {
        std::cerr << "Error opening log file: " << logFilePath << std::endl;
        return -3;
    }

   // write headers
    static const char* strSignal = "SIGNAL";
    static const char* strTimestamp = "Timestamp";

    rawDataFile      << strTimestamp;
    filteredDataFile << strTimestamp;
    for (int i=0; i< signals.size(); i++) {
        rawDataFile     << ","  << strSignal<<i;
        filteredDataFile << "," << strSignal<<i;
    }
    rawDataFile      << std::endl;
    filteredDataFile << std::endl;

    return 0;
}
//**********************************************************************************************************
void AquisitionModule::acquisitionFrom(SignalGenerator* signalGenerator) {
    uint32_t timestamp = counter; // assuming 1ms interval
    float filteredValue;
    float rawValue;
    uint32_t retCode;


    int debugChannel =0;
    debugFile << timestamp;
    statFile << timestamp;
    rawDataFile << timestamp;
    filteredDataFile << timestamp;

    int anomalySignals = 0;
    signalGenerator->processSignals();

    for (int i=0; i<signals.size(); i++)
    {
        ZSignal &sig = signals[i];
        rawValue = signalGenerator->signals[i].value;
        
        if(sig.processValue(rawValue, filteredValue)>=ANOMALY_DETECTION_PERIOD) anomalySignals++;
        
        rawDataFile << "," << rawValue;
        filteredDataFile << "," << filteredValue;
        statFile << "," << sig.anomalyCounter;

        if(i==debugChannel) {
            // (0)timestamp,(1)rawValue, (2)filteredValue, (3)avgValue, (4)sigma, (5)anomalyCounter
            debugFile << "," << rawValue << "," << filteredValue <<"," << sig.stat.avg << "," << sig.stat.sigma <<"," << sig.anomalyCounter<< std::endl;
        }
    }
    
    rawDataFile << std::endl;
    filteredDataFile << std::endl;
    statFile << std::endl;

    if(anomalySignals>=ANOMALY_DETECTION_COUNT && !ledState) {
        ledState = true;
        counter500 = 0;
        logFile << "[" << timestamp << "ms] LED=ON"<< std::endl;
    }
    
    if(anomalySignals < ANOMALY_DETECTION_COUNT && ledState) counter500++;
    if(counter500>=LED_ON_PERIOD && ledState) { 
        ledState = false; 
        counter500 = 0;
        logFile << "[" << timestamp << "ms] LED=OFF"<< std::endl;
    }

    anomalySignals0 = anomalySignals;
    counter++;
}
