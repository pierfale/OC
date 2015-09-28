#ifndef _DATA_OUTPUT_H
#define _DATA_OUTPUT_H

#include <cstring>

template<unsigned int Size>
struct DataOutput {

public:
	DataOutput() {

	}

	DataOutput(const DataOutput<Size>& that) {
		memcpy(job_execution_order, that.job_execution_order, Size*sizeof(unsigned int));
		memcpy(job_execution_timer, that.job_execution_timer, Size*sizeof(unsigned int));
		memcpy(job_score, that.job_score, Size*sizeof(unsigned int));
	}

	DataOutput& operator=(const DataOutput<Size>& that) {
		memcpy(job_execution_order, that.job_execution_order, Size*sizeof(unsigned int));
		memcpy(job_execution_timer, that.job_execution_timer, Size*sizeof(unsigned int));
		memcpy(job_score, that.job_score, Size*sizeof(unsigned int));
	}

	void reset() {
		for(int i=0; i<Size; i++)
			job_execution_order[i] = i;
	}

	void compute_score(const DataInput<Size>& input, unsigned int min = 0, unsigned int max = Size-1) {
		unsigned int acc = min == 0 ? 0 : job_execution_timer[min-1];
		for(int i=min; i<=max; ++i) {
			unsigned int job = job_execution_order[i];
			acc += input.job_execution_times[job];
			job_execution_timer[i] = acc;
			job_score[i] = (acc > input.job_deadlines[job] ? input.job_weights[job]*(acc-input.job_deadlines[job]) : 0);
		}
	}


	unsigned int job_execution_order[Size];
	unsigned int job_execution_timer[Size];
	unsigned int job_score[Size];
};

#endif
