#ifndef _VERIFIER_H
#define _VERIFIER_H

class Verifier {

public:
	template<unsigned int Size>
	static unsigned int process(const DataInput<Size>& input, const DataOutput<Size>& output) {

		unsigned int current_time = 0;
		unsigned int total = 0;
		for(unsigned int i=0; i<Size; i++) {
			/*if(output.job_execution_order[i] >= Size)
				throw std::runtime_error("Incorrect output : invalid index at job_execution_order["+std::to_string(i)+"]");
*/
			current_time += input.job_execution_times[output.job_execution_order[i]];
			total += input.job_weights[output.job_execution_order[i]]*(current_time > input.job_deadlines[output.job_execution_order[i]] ? current_time-input.job_deadlines[output.job_execution_order[i]] : 0);
		}

		return total;
	}
private:
	Verifier();
};



#endif
