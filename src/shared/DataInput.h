#ifndef _DATA_INPUT_H
#define _DATA_INPUT_H

template<unsigned int Size>
struct DataInput {

public:
	DataInput() {

	}

	unsigned int job_execution_times[Size];
	unsigned int job_weights[Size];
	unsigned int job_deadlines[Size];
};

#endif
