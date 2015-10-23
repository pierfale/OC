#ifndef GENETIC_ALGORITHM_SOLVER_H
#define GENETIC_ALGORITHM_SOLVER_H

#include <vector>
#include <cassert>

#include "shared/Solver.h"

#include "shared/DataInput.h"
#include "shared/DataOutput.h"

#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"
#include "solver/RandomSolver.h"
#include "shared/Neighborhood.h"
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

template <unsigned int Size, typename Logger>
class CrossOverPositionBased {

public:
	static void process(const BasicProgramOption<Size, Logger>& program_options, const DataInput<Size>& input,
						const DataOutput<Size>& parent1, const DataOutput<Size>& parent2, DataOutput<Size>& child1, DataOutput<Size>& child2) {
		unsigned int k=Size/2;

		bool k_mask[Size];
		for(unsigned int k_index=0; k_index<Size; k_index++) {
			k_mask[k_index] = false;
		}

		for(unsigned int k_index=0; k_index<k; k_index++) {
			do {
				unsigned int current_position = ::rand()%Size;
				if(!k_mask[current_position]) {
					k_mask[current_position] = true;
					break;
				}
			} while(true);
		}

		std::vector<unsigned int> parent1_value;
		std::vector<unsigned int> parent2_value;

		for(unsigned int k_index=0; k_index<Size; k_index++) {
			if(k_mask[k_index]) {
				child1.job_execution_order[k_index] = parent2.job_execution_order[k_index];
				child2.job_execution_order[k_index] = parent1.job_execution_order[k_index];
				parent1_value.push_back(parent1.job_execution_order[k_index]);
				parent2_value.push_back(parent2.job_execution_order[k_index]);
			}

		}

		unsigned int cursor1 = 0;
		unsigned int cursor2 = 0;
		for(unsigned int k_index=0; k_index<Size; k_index++) {
			if(!k_mask[k_index]) {
				while(std::find(std::begin(parent2_value), std::end(parent2_value), parent1.job_execution_order[cursor1]) != std::end(parent2_value)) {
					cursor1++;
				}
				child1.job_execution_order[k_index] = parent1.job_execution_order[cursor1++];

				while(std::find(std::begin(parent1_value), std::end(parent1_value), parent2.job_execution_order[cursor2]) != std::end(parent1_value)) {
					cursor2++;
				}
				child2.job_execution_order[k_index] = parent2.job_execution_order[cursor2++];
			}
		}

		child1.compute_score(input);
		child2.compute_score(input);
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
class GeneticAlgorithmProgramOption : public BasicProgramOption<Size, Logger> {

	typedef void(*SolverFunction)(const GeneticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&);

public:

	void parse(int argc, const char** argv) {
		if(argc < 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

		_n = 100;
		_g = 1000;
		float mr = 0.1;

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
			else if(current == "-mutation-rate")
				mr = ::atof(next.c_str());
			else
				throw std::runtime_error("unknown option "+current+", usage : "+usage());
		}

		_m = _n*mr;


		_solver_func = _solve<InitializePopulation, CrossOverPositionBased, MutationSwap>;
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename> [-population Int] [-generation Int] [-mutation-rate Float]");
	}

	SolverFunction solverFunction() const {
		return _solver_func;
	}



private:
	template<template<unsigned int, typename> class Init, template<unsigned int, typename> class CrossOver, template<unsigned int, typename> class Mutation>
	static void _solve(const GeneticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {

		Logger::enableUpgrade(false);

		Population<Size, Logger> population;
		Init<Size, Logger>::process(program_options, input, population, program_options._n);

		for(unsigned int generation = 0; generation < program_options._g; generation++) {

			std::vector<DataOutput<Size>> population_crossover;


			for(unsigned int i = 0; i < program_options._n/2; i++) {
				std::vector<DataOutput<Size>> output_selected;
				population.rand(output_selected, 2);

				DataOutput<Size> output_crossover1;
				DataOutput<Size> output_crossover2;

				CrossOver<Size, Logger>::process(program_options, input, output_selected[0], output_selected[1], output_crossover1, output_crossover2);

				population_crossover.push_back(output_crossover1);
				population_crossover.push_back(output_crossover2);
			}

			std::vector<DataOutput<Size>> population_mutation;

			population.rand(population_mutation, program_options._m);

			for(auto it=std::begin(population_mutation); it != std::end(population_mutation); ++it) {
				Mutation<Size, Logger>::process(program_options, input, *it);
				population.add(*it);
			}

			for(auto it=std::begin(population_crossover); it != std::end(population_crossover); ++it) {
				population.add(*it);
			}

			population.keepBest(input, program_options._n);

			output = population.best(input);
			Logger::enableUpgrade(true);
			Verifier::process<Size, Logger>(input, output);
			Logger::enableUpgrade(false);
		}

	}

	SolverFunction _solver_func;

	unsigned int _n;
	unsigned int _g;
	unsigned int _m;
};

class GeneticAlgorithm {

public:
	GeneticAlgorithm() {

	}

	template<unsigned int Size, typename Logger>
	static void process(const GeneticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output);
	}
};

#endif
