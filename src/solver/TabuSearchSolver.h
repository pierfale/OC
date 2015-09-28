#ifndef _TABU_SEARCH_H
#define _TABU_SEARCH_H

#include <stdexcept>

#include "shared/DataInput.h"
#include "shared/DataOutput.h"
#include "shared/Verifier.h"
#include "shared/Neighborhood.h"
#include "solver/RandomSolver.h"
#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"
#include "shared/ProgramOptions.h"

template <unsigned int Size, typename Logger, unsigned int HistorySize>
class SelectTabu {
public:
	template<template<unsigned int, typename, unsigned int> class Neighborhood>
	static bool process(const DataInput<Size>& input, DataOutput<Size>& output, Neighborhood<Size, Logger, HistorySize>& neighborhood, unsigned int* best_global_score) {
		DataOutput<Size> initial_output(output);
		DataOutput<Size> best_output(output);
		unsigned int best_score = Verifier::process<Size, Logger>(input, output);
		unsigned int initial_score = best_score;

		while(neighborhood.process(input, output, *best_global_score)) {

			unsigned int current_score;
			if((current_score = Verifier::process<Size, Logger>(input, output)) < best_score) {
				best_output = output;
				best_score = current_score;
			}
			output = initial_output;
		}

		neighborhood.selected();

		output = best_output;
		*best_global_score = best_score;

		return best_score < initial_score;
	}
};

template <unsigned int Size, typename Logger, unsigned int HistorySize>
class NeighborhoodSwapTabu : public NeighborhoodDoubleIndex<Size, Logger> {

public:
	NeighborhoodSwapTabu() : NeighborhoodDoubleIndex<Size, Logger>() {
		for(unsigned int i=0; i<HistorySize; i++) {
			_history[i] = -1;
		}
		_history_cursor = 0;

	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output, unsigned int best_global_score) {

		do {
			if(!NeighborhoodDoubleIndex<Size, Logger>::incrementIndex())
				return false;

			std::swap(output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index1], output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index2]);
			output.compute_score(input, std::min(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2), std::max(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2));

			if(Verifier::process<Size, Logger>(input, output) > best_global_score) {
				_history[(_history_cursor+1)%HistorySize] = NeighborhoodDoubleIndex<Size, Logger>::_index1;
				_history[(_history_cursor+1)%HistorySize] = NeighborhoodDoubleIndex<Size, Logger>::_index2;

				return true;
			}
			else {
				std::swap(output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index1], output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index2]);
			}

		} while(std::find(&_history[0], &_history[HistorySize], NeighborhoodDoubleIndex<Size, Logger>::_index1) != &_history[HistorySize] && std::find(&_history[0], &_history[HistorySize], NeighborhoodDoubleIndex<Size, Logger>::_index2) != &_history[HistorySize]);

		_history[(_history_cursor+1)%HistorySize] = NeighborhoodDoubleIndex<Size, Logger>::_index1;
		_history[(_history_cursor+1)%HistorySize] = NeighborhoodDoubleIndex<Size, Logger>::_index2;

		std::swap(output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index1], output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index2]);
		output.compute_score(input, std::min(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2), std::max(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2));

		return true;
	}

private:
	unsigned int _history[HistorySize];
	unsigned int _history_cursor;
};

template <unsigned int Size, typename Logger>
class TabuSearchProgramOption : public BasicProgramOption<Size, Logger>  {

	typedef void(*SolverFunction)(const TabuSearchProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&);

public:
	void parse(int argc, const char** argv) {
		if(argc < 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

		std::string initOption = "mdd";

		for(unsigned int i=3; i<argc; i += 2) {
			std::string current(argv[i]);

			if(i>=argc) {
				throw std::runtime_error("no option parameter "+current+", usage : "+usage());
			}

			std::string next(argv[i+1]);

			if(current == "-init")
				initOption = next;
			else
				throw std::runtime_error("unknown option "+current+", usage : "+usage());
		}

		if(initOption == "rnd")
			_solver = solverFunction<NeighborhoodSwapTabu, RandomSolver>;
		else if(initOption == "edd")
			_solver = solverFunction<NeighborhoodSwapTabu, EDDSolver>;
		else if(initOption == "mdd") {
			_solver = solverFunction<NeighborhoodSwapTabu, MDDSolver>;
		}
		else
			throw std::runtime_error("unknown option "+initOption+", usage : "+usage());
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename> [-init(rnd | edd | mdd)]");
	}

	SolverFunction solverFunction() const {
		return _solver;
	}

	template<template<unsigned int, typename, unsigned int> class Neighborhood, typename Init>
	static void solverFunction(const TabuSearchProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		Init::process(program_options, input, output);
		unsigned int best_solution = Verifier::process<Size, Logger>(input, output);

		Neighborhood<Size, Logger, 50> neighborhood;

		while(SelectTabu<Size, Logger, 50>::process(input, output, neighborhood, &best_solution));
	}

private:
	SolverFunction _solver;

};


class TabuSearchSolver {

public:

	template<unsigned int Size, typename Logger>
	static void process(const TabuSearchProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output);
	}
};

#endif
