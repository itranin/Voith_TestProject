#ifndef SIGNAL_GEN_H
#define SIGNAL_GEN_H
#include <cstdint>
#include <vector>

//структура для генерирования случайных чисел
struct xorshift128_state {
  uint32_t a, b, c, d;
};

// Signal parameters
#define SIGNAL_MIN_VALUE 0.0f
#define SIGNAL_MAX_VALUE 100.0f
#define SIGNAL_SCALE (SIGNAL_MAX_VALUE - SIGNAL_MIN_VALUE)
#define NOISE_MAX_LEVEL 0.05f		// максимальный уровень шума (5% от амплитуды сигнала)
#define SIGNAL_PERIOD_MIN 1000.0f
#define SIGNAL_PERIOD_MAX 100000.0f

// Anomaly parameters
#define ANOMALY_PERIOD   5000		// период с гарантированной аномалиейции (в итерациях)
#define ANOMALY_DURATION 600		// длительность аномалии (в итерациях)
#define ANOMALY_SIGNALS_MIN  2		// количество сигналов с аномалией в периоде (минимум)
#define ANOMALY_SIGNALS_MAX  4		// количество сигналов с аномалией в периоде (максимум - в ТЗ 3)

// Class of model signal
class XSignal {
	xorshift128_state randomSeed;
	uint32_t iteration;

	float offset;	// смещение синусоиды
	float period;	// период синусоиды
	float amp;		// амплитуда синусоиды
	float phase;	// фаза синусоиды

	bool  anomalyState0; // previous anomaly state

public:
	float value;	// текущее значение синусоиды
	float value0;	// предыдущее значение синусоиды
	uint32_t index;
	bool anomalyState; // состояние аномалии

	float getRandomFloat();	// returns float random number between 0.0 and 1.0
	float getNoiseValue();
	float getAnomalyValue(); // generate anomaly value
	
	XSignal();
	float getValue();	// generate & returns current value of the signal
};

// Class for generating signals
class SignalGenerator {	
	uint32_t globalIteration;
	uint32_t anomalyIteration; 
	uint32_t anomalyDuration;
public:	
	std::vector <XSignal> signals;
	void initSignals(int numSignals);
	void processSignals(); // generate next values for all signals
	uint32_t getNextAnomalyPeriod();
};

#endif // SIGNAL_GEN_H