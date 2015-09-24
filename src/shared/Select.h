#ifndef _SELECT_H
#define _SELECT_H

#include "shared/Verifier.h"

template <unsigned int Size>
class SelectFirst {

public:
	template<template<unsigned int> class Neighborhood>
	static bool process(const DataInput<Size>& input, DataOutput<Size>& output, Neighborhood<Size>& neighborhood) {

		unsigned int initial_score = Verifier::process(input, output);
		DataOutput<Size> initial_output(output);

		do {
			output = initial_output;

			if(!neighborhood.process(input, output)) {
				output = initial_output;
				return false;
			}
		} while(Verifier::process(input, output) >= initial_score);

		neighborhood.selected();

		return true;
	}
};

template <unsigned int Size>
class SelectBest {

public:
	template<template<unsigned int> class Neighborhood>
	static bool process(const DataInput<Size>& input, DataOutput<Size>& output, Neighborhood<Size>& neighborhood) {
		DataOutput<Size> initial_output(output);
		DataOutput<Size> best_output(output);
		unsigned int best_score = Verifier::process(input, output);
		unsigned int initial_score = best_score;

		while(neighborhood.process(input, output)) {

			unsigned int current_score;
			if((current_score = Verifier::process(input, output)) < best_score) {
				best_output = output;
				best_score = current_score;
			}
			output = initial_output;
		}

		neighborhood.selected();

		output = best_output;

		return best_score < initial_score;
	}
};

#endif
