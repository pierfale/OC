#ifndef _EDD_SOLVER_H
#define _EDD_SOLVER_H

#include <algorithm>
#include <chrono>

#include "shared/Solver.h"

class EDDSolver {

public:
	template<unsigned int Size>
	static void process(const BasicProgramOption<Size>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		std::sort(&output.job_execution_order[0], &output.job_execution_order[Size], [&](unsigned int job1, unsigned int job2) {
					 return input.job_deadlines[job1] < input.job_deadlines[job2];
		 });
	}

private:
	EDDSolver();

};

#endif
