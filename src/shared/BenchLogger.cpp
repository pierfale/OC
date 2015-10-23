#include "shared/BenchLogger.h"

unsigned long long BenchLogger::_verifier_call_number(0);
unsigned int BenchLogger::_optimal_score;
bool BenchLogger::_enable_upgrade(true);
std::ofstream BenchLogger::_update_file;

void BenchLogger::initialize() {

}

void BenchLogger::reset(unsigned int optimal_score, const std::string& update_filename) {
	_verifier_call_number = 0;
	_optimal_score = optimal_score;
	_enable_upgrade = true;

	if(_update_file.is_open())
		_update_file.close();

	_update_file.open(update_filename, std::ios::out | std::ios::trunc);
}

void BenchLogger::verifier() {
	_verifier_call_number++;
}

unsigned long long BenchLogger::cost_call_number() {
	return _verifier_call_number;
}

void BenchLogger::enableUpgrade(bool state) {
	_enable_upgrade = state;
}
