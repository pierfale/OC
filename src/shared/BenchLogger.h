#ifndef _BENCH_LOGGER_H
#define _BENCH_LOGGER_H

#include <fstream>

#include "shared/DataInput.h"
#include "shared/DataOutput.h"
#include "shared/Verifier.h"
#include "shared/PassiveLogger.h"


class BenchLogger {

public:
	static void initialize();
	static void reset(unsigned int optimal_score, const std::string& update_filename);

	static void verifier();
	static void enableUpgrade(bool state);
	template<unsigned int Size>
	static void upgradeSolution(const DataInput<Size>& input, DataOutput<Size>& output) {
		if(!_enable_upgrade)
			return;
		unsigned int current_score = Verifier::process<Size, PassiveLogger>(input, output);
		_update_file << _verifier_call_number << " " << (_optimal_score == 0 ? current_score : 100.0*(current_score-_optimal_score)/_optimal_score) << std::endl;
	}

	static unsigned long long cost_call_number();

private:
	BenchLogger();

	static unsigned long long _verifier_call_number;
	static unsigned int _optimal_score;
	static bool _enable_upgrade;
	static std::ofstream _update_file;
};

#endif
