#ifndef _PASSIVE_SOLVER_H
#define _PASSIVE_SOLVER_H

#include <algorithm>
#include <chrono>

#include "shared/Solver.h"

class PassiveSolver {

public:
	template<unsigned int Size, typename Logger>
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {

	}

private:
	PassiveSolver();

};

#endif
