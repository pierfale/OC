#ifndef _DATA_OUTPUT_H
#define _DATA_OUTPUT_H

#include <cstring>

template<unsigned int Size>
struct DataOutput {

public:
	DataOutput() {
		reset();

	}

	DataOutput(const DataOutput<Size>& that) {
		memcpy(job_execution_order, that.job_execution_order, Size*sizeof(unsigned int));
	}

	DataOutput& operator=(const DataOutput<Size>& that) {
		memcpy(job_execution_order, that.job_execution_order, Size*sizeof(unsigned int));
	}

	void reset() {
		for(int i=0; i<Size; i++)
			job_execution_order[i] = i;
	}


	unsigned int job_execution_order[Size];
};

#endif
