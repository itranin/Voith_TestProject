#include "signal_gen.h"	
#include <cmath>
#include <sstream>
#include <iostream>
#include <fstream>


uint32_t xorshift128(struct xorshift128_state *state)
{
	/* Algorithm "xor128" from p. 5 of Marsaglia, "Xorshift RNGs" */
	// КАК ПРОСИЛИ В ТЗ, ИСПОЛЬЗУЕТСЯ XORSHIFT128 ДЛЯ ГЕНЕРАЦИИ СЛУЧАЙНЫХ ЧИСЕЛ
	uint32_t t = state->d;

	uint32_t const s = state->a;
	state->d = state->c;
	state->c = state->b;
	state->b = s;

	t ^= t << 11;
	t ^= t >> 8;
	return state->a = t ^ s ^ (s >> 19);
}

XSignal::XSignal() {
	iteration = 0;
	index = 0;
	value = 0.0f;
	value0 = 0.0f;
	anomalyState0 = false;
	anomalyState = false;
	// random generated parameters		
	offset = SIGNAL_MIN_VALUE + getRandomFloat() * SIGNAL_SCALE;
	period = SIGNAL_PERIOD_MIN + getRandomFloat() * (SIGNAL_PERIOD_MAX - SIGNAL_PERIOD_MIN);
	amp    = getRandomFloat() * SIGNAL_SCALE / 2.0f;
	phase  = 2.0* M_PI * getRandomFloat();
	// Initialize random state with some default values
	// в нашем случае - просто неинициализированные значения памяти - так БОЛЕЕ случайно без привязки к таймеру
	/*
	randomSeed.a = 123456789;
	randomSeed.b = 362436069;
	randomSeed.c = 521288629;
	randomSeed.d = 88675123;
	*/
	return;
}

float XSignal::getRandomFloat()
{
	return static_cast<float>(xorshift128(&randomSeed)) / static_cast<float>(0xFFFFFFFF);
}

float XSignal::getNoiseValue() {
	return NOISE_MAX_LEVEL * value0* (getRandomFloat() - 0.5f);
}

float XSignal::getAnomalyValue() {
	// Generate a large spike as an anomaly
	return SIGNAL_MAX_VALUE * (0.5f + getRandomFloat());
}

float XSignal::getValue()
{
	if (anomalyState && !anomalyState0) {
		value0 = value+getAnomalyValue();
	}
	anomalyState0 = anomalyState;
	
	if(anomalyState) {
		value = value0 + getNoiseValue();
	}
	else {
		value0 = value;
		value= offset + amp * sin(static_cast<float>(iteration) * 2.0f * M_PI / period + phase) + getNoiseValue();
	}
	iteration++;
	return value;
}

//**********************************************************************************************************
void SignalGenerator::initSignals(int numSignals)
{
	signals.clear();
	for (int i = 0; i < numSignals; i++) {
		XSignal sig;
		sig.index = i;
		signals.push_back(sig);
	}
	globalIteration = 0;
	anomalyIteration = getNextAnomalyPeriod();
}

uint32_t SignalGenerator::getNextAnomalyPeriod()
{
	return signals[0].getRandomFloat() * ANOMALY_PERIOD;
}

void SignalGenerator::processSignals()
{
	// Check if it's time for the next anomaly period
	if(globalIteration == anomalyIteration) {
		anomalyDuration = signals[0].getRandomFloat() * ANOMALY_DURATION;
		int numAnomalySignals = ANOMALY_SIGNALS_MIN + float(signals[0].getRandomFloat() * (ANOMALY_SIGNALS_MAX - ANOMALY_SIGNALS_MIN));
		for(int i=0; i< numAnomalySignals; i++) {
			int sigIndex = signals[0].getRandomFloat() * signals.size();
			if(sigIndex >= signals.size()) sigIndex = signals.size() - 1;
			signals[sigIndex].anomalyState = true;
		}
	}
	// Check if anomaly duration has ended
	if(globalIteration == (anomalyIteration + anomalyDuration)) {
		// Reset anomaly states
		for(auto &sig : signals) {
			sig.anomalyState = false;
		}
		anomalyIteration = globalIteration + getNextAnomalyPeriod();
	}

	for (auto & sig : signals) {
		sig.getValue();
	}

	globalIteration++;
}