#ifndef _MDD_SOLVER_H
#define _MDD_SOLVER_H

#include <algorithm>
#include <chrono>
#include <limits>

#include "shared/Solver.h"

class MDDSolver {

public:
	template<unsigned int Size>
	static void process(const BasicProgramOption<Size>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {

		srand(23); //determinist seed

		unsigned int current_time = 0;
		unsigned int remain_inded[Size];
		std::copy(&output.job_execution_order[0], &output.job_execution_order[Size], &remain_inded[0]);


		for(unsigned int i=0; i<Size; i++) {
			unsigned int* current_ptr = std::min_element(&remain_inded[0], &remain_inded[Size-i], [&](unsigned int job1, unsigned int job2) {

						unsigned int i1 = std::max(current_time+input.job_execution_times[job1], input.job_deadlines[job1]);
						unsigned int i2 = std::max(current_time+input.job_execution_times[job2], input.job_deadlines[job2]);

						 if(i1 == i2) //tie-break
							return rand() % 2 == 1;

						 return i1 < i2;
			 });

			unsigned int current_index = current_ptr-remain_inded;
			unsigned int current_value = *current_ptr;

			current_time += input.job_execution_times[current_value];
			output.job_execution_order[i] = current_value;
			std::swap(remain_inded[current_index], remain_inded[Size-i-1]);

		}
		output.compute_score(input);
	}

private:
	MDDSolver();

};

#endif
