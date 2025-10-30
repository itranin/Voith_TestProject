#ifndef EVENT_DET_H
#define EVENT_DET_H

#include "signal_gen.h"
#include "signal_proc.h"

#define ANOMALY_DETECTION_PERIOD 10 // in iterations
#define ANOMALY_DETECTION_COUNT 3    // number of anomalies to log event
#define LED_ON_PERIOD 500            // in iterations

class AquisitionModule {
    uint32_t counter;
    uint32_t counter500;
    int anomalySignals0;
    std::ofstream rawDataFile, filteredDataFile, logFile, debugFile, statFile;
public:
    bool ledState;
    // constructor and destructor
    ~AquisitionModule();
    void initSignals(int numSignals);
    std::vector<ZSignal> signals;
    int saveDataTo(const char* rawData, const char* filteredData, const char* logFilePath);
    void acquisitionFrom(SignalGenerator *generator);
};


#endif // EVENT_DET_H