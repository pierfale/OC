#ifndef _VERIFIER_H
#define _VERIFIER_H

#include <algorithm>

class Verifier {

public:
	template<unsigned int Size>
	static unsigned int process(const DataInput<Size>& input, const DataOutput<Size>& output) {
		return std::accumulate(&output.job_score[0], &output.job_score[Size], 0);
	}
private:
	Verifier();
};



#endif
