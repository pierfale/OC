#ifndef MEMETIC_ALGORITHM_SOLVER_H
#define MEMETIC_ALGORITHM_SOLVER_H

#include <vector>
#include <cassert>

#include "shared/Solver.h"

#include "shared/DataInput.h"
#include "shared/DataOutput.h"

#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"
#include "solver/RandomSolver.h"
#include "shared/Neighborhood.h"
#include "solver/HillClimbingSolver.h"
#include "shared/Population.h"

template<unsigned int Size, typename Logger>
class InitializePopulation {

public:
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, Population<Size, Logger>& population, unsigned int n) {
		assert(n >= 2);

		DataOutput<Size> output_edd;
		output_edd.reset();
		EDDSolver::process(program_options, input, output_edd);
		population.add(output_edd);


		DataOutput<Size> output_mdd;
		output_mdd.reset();
		MDDSolver::process(program_options, input, output_mdd);
		population.add(output_mdd);

		for(unsigned int i=0; i<n-2; i++) {
			DataOutput<Size> output_rnd;
			output_rnd.reset();
			RandomSolver::process(program_options, input, output_rnd);
			population.add(output_rnd);
		}
	}
};

template<unsigned int Size, typename Logger>
class ReplacePopulation {

public:
	template<template<typename, typename...> class Output>
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, Population<Size, Logger>& population, const Output<DataOutput<Size>>& population_new) {
		unsigned int n = population.size();
		for(auto it = std::begin(population_new); it != std::end(population_new); ++it)
			population.add(*it);
		population.keepBest(input, n);
	}
};

template <unsigned int Size, typename Logger>
class CrossOverOrderBased {

public:
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input,
						const DataOutput<Size>& parent1, const DataOutput<Size>& parent2, DataOutput<Size>& child) {
		unsigned int l=10;

		bool k_mask[Size];
		for(unsigned int k_index=0; k_index<Size; k_index++) {
			k_mask[k_index] = false;
		}

		unsigned int current_position = ::rand()%(Size-l);

		for(unsigned int k_index=current_position; k_index<current_position+l; k_index++) {
			k_mask[k_index] = true;
		}

		std::vector<unsigned int> parent_value;

		for(unsigned int k_index=0; k_index<Size; k_index++) {
			if(k_mask[k_index]) {
				child.job_execution_order[k_index] = parent2.job_execution_order[k_index];
				parent_value.push_back(parent2.job_execution_order[k_index]);
			}

		}

		unsigned int cursor = 0;
		for(unsigned int k_index=0; k_index<Size; k_index++) {
			if(!k_mask[k_index]) {
				while(std::find(std::begin(parent_value), std::end(parent_value), parent1.job_execution_order[cursor]) != std::end(parent_value)) {
					cursor++;
				}
				child.job_execution_order[k_index] = parent1.job_execution_order[cursor++];
			}
		}

		child.compute_score(input);
	}

};

template <unsigned int Size, typename Logger>
class MutationSwap {

public:
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		NeighborhoodSwap<Size, Logger> neighborhood;

		neighborhood.rand();
		neighborhood.process(input, output);
	}
};

template <unsigned int Size, typename Logger>
class MemeticAlgorithmProgramOption : public BasicProgramOption<Size, Logger> {

	typedef void(*SolverFunction)(const MemeticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&, void*);

public:

	void parse(int argc, const char** argv) {
		if(argc < 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

		float cr = 0.5;
		_n = 100;
		_g = 100;
		_p_mutation = 0.3;

		for(unsigned int i=3; i<argc; i += 2) {
			std::string current(argv[i]);

			if(i>=argc) {
				throw std::runtime_error("no option parameter "+current+", usage : "+usage());
			}

			std::string next(argv[i+1]);

			if(current == "-population")
				_n = ::atoi(next.c_str());
			else if(current == "-generation")
				_g = ::atoi(next.c_str());
			else if(current == "-creation-rate")
				cr = ::atof(next.c_str());
			else if(current == "-mutation-rate")
				_p_mutation = ::atof(next.c_str());
			else
				throw std::runtime_error("unknown option "+current+", usage : "+usage());
		}

		_c_rate = _n*cr;

		_solver_func = _solve<InitializePopulation, CrossOverOrderBased, MutationSwap, ReplacePopulation, HillClimbingSolver, HillClimbingProgramOption>;

		HillClimbingProgramOption<Size, Logger>* local_search_program_option = new HillClimbingProgramOption<Size, Logger>();

#define N_ARG 9
		const char* local_search_args[N_ARG];
		local_search_args[0] = argv[0];
		local_search_args[1] = argv[1];
		local_search_args[2] = argv[2];
		local_search_args[3] = "-init";
		local_search_args[4] = "none";
		local_search_args[5] = "-select";
		local_search_args[6] = "best";
		local_search_args[7] = "-neighborhood";
		local_search_args[8] = "swap";


		local_search_program_option->parse(N_ARG, local_search_args);
		_local_search_program_option = local_search_program_option;
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename>  [-population Int] [-generation Int] [-creation-rate Float] [-mutation-rate Float]");
	}

	SolverFunction solverFunction() const {
		return _solver_func;
	}

	void* localSearchProgramOption() const {
		return _local_search_program_option;
	}


private:
	template<template<unsigned int, typename> class Init, template<unsigned int, typename> class CrossOver, template<unsigned int, typename> class Mutation, template<unsigned int, typename> class Replace, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption>
	static void _solve(const MemeticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {
		Logger::enableUpgrade(false);
		Population<Size, Logger> population;
		Init<Size, Logger>::process(program_options, input, population, program_options._n);

		for(unsigned int generation = 0; generation < program_options._g; generation++) {

			std::vector<DataOutput<Size>> population_child;


			for(unsigned int i = 0; i < program_options._c_rate; i++) {
				std::vector<DataOutput<Size>> output_selected;
				population.rand(output_selected, 2);

				DataOutput<Size> output_child;

				CrossOver<Size, Logger>::process(program_options, input, output_selected[0], output_selected[1], output_child);

				if(((double)::rand()/RAND_MAX) < program_options._p_mutation)
					Mutation<Size, Logger>::process(program_options, input, output_child);

				LocalSearch::process(*reinterpret_cast<LocalSearchProgramOption<Size, Logger>*>(local_search_program_options), input, output_child);

				population_child.push_back(output_child);
			}

			Replace<Size, Logger>::process(program_options, input, population, population_child);


			output = population.best(input);
			Logger::enableUpgrade(true);
			Verifier::process<Size, Logger>(input, output);
			Logger::enableUpgrade(false);
		}

	}

	SolverFunction _solver_func;
	void* _local_search_program_option;

	unsigned int _n;
	unsigned int _g;
	unsigned int _p_mutation;
	unsigned int _c_rate;
};

class MemeticAlgorithm {

public:
	MemeticAlgorithm() {

	}

	template<unsigned int Size, typename Logger>
	static void process(const MemeticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output, program_options.localSearchProgramOption());
	}
};

#endif
