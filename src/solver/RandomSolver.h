#ifndef _RANDOM_SOLVER_H
#define _RANDOM_SOLVER_H

#include <algorithm>
#include <chrono>

#include "shared/Solver.h"

class RandomSolver {

public:
	template<unsigned int Size, typename Logger>
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		std::random_shuffle(&output.job_execution_order[0], &output.job_execution_order[Size]);
		output.compute_score(input);
	}

private:
	RandomSolver();

};

#endif
