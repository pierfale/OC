#ifndef _RANDOM_SOLVER_H
#define _RANDOM_SOLVER_H

#include <algorithm>
#include <chrono>

#include "shared/Solver.h"

class RandomSolver {

public:
	template<unsigned int Size>
	static void process(const BasicProgramOption<Size>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		shuffle(&output.job_execution_order[0], &output.job_execution_order[Size], std::default_random_engine(seed));
		output.compute_score(input);
	}

private:
	RandomSolver();

};

#endif
