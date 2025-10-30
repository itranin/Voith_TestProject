#include <iostream>
#include "signal_gen.h"
#include "signal_proc.h"
#include "event_det.h"

SignalGenerator signalGenerator;
AquisitionModule aquisitionModule;

/// @brief Main function
/// @param argv [1] raw signals data file
/// @param argv [2] filtered data file
/// @param argv [3] log file 
/// @param argv [4] num signals 
/// @param argv [5] number of iterations
/// @return 0 on success, -1 on invalid parameters, -2 on file error
int main(int argc, char* argv[]) {
    if(argc<5) {
        std::cerr << "Usage with params: <<raw_data.csv> <filtered_data.csv> <log.txt> <num_signals> <iterations>" << std::endl;
        return -1;
    }
    std::cout << "Starting acquisition..." << std::endl;
    int numSignals = std::stoi(argv[4]);
    int iterations = std::stoi(argv[5]);

    signalGenerator.initSignals(numSignals);
    aquisitionModule.initSignals(numSignals);
    if(aquisitionModule.saveDataTo(argv[1], argv[2],argv[3])) return -2;

    for(int i=0;i<iterations;i++) {
        aquisitionModule.acquisitionFrom(&signalGenerator);
    }
    std::cout << iterations << "proceeded. End" << std::endl;
    return 0;
}