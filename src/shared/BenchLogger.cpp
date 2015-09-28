#include "shared/BenchLogger.h"

unsigned int BenchLogger::_verifier_call_number;

void BenchLogger::initialize() {

}

void BenchLogger::reset() {
	_verifier_call_number = 0;
}

void BenchLogger::verifier() {
	_verifier_call_number++;
}

unsigned int BenchLogger::cost_call_number() {
	return _verifier_call_number;
}
