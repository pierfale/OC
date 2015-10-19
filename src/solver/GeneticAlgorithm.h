#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <vector>
#include "shared/DataInput.h"
#include "shared/DataOutput.h"

#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"
#include "solver/RandomSolver.h"

template <unsigned int Size, typename Logger>
class Population {

public:
	Population() : _population() {

	}

	void add(const DataOutput<Size>& output) {
		_population.push_back(output);
	}

private:
	std::vector<DataInput<Size>> _population;
};

template<unsigned int Size, typename Logger>
class InitializePopulation {

public:
	static void process(const GeneticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, Population<Size, Logger>& population, unsigned int n) {
		assert(n >= 2);

		DataOutput<Size> output_edd;
		EDDSolver::process(program_options, input, output_edd);
		population.add(output_edd);

		DataOutput<Size> output_mdd;
		MDDSolver::process(program_options, input, output_mdd);
		population.add(output_mdd);

		for(unsigned int i=0; i<n-2; i++) {
			DataOutput<Size> output_rnd;
			RandomSolver::process(program_options, input, output_rnd);
			population.add(output_rnd);
		}
	}
};

template <unsigned int Size, typename Logger>
class CrossOverPositionBased {

public:
	static void process(const GeneticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input,
						const DataOutput<Size>& parent1, const DataOutput<Size>& parent2, DataOutput<Size>& child1, DataOutput<Size>& child2) {
		unsigned int k=Size/2;

		std::vector<unsigned int> k_position;
		for(unsigned int k_index=0; k_index<k; k_index++) {
			do {
				unsigned int current_position = ::rand()%Size;
				if(std::find(std::begin(k_position), std::end( k_position), current_position) == std::end(k_position)) {
					k_position.push_back(current_position);
					break;
				}
			} while(true);
		}

		for(auto it_k = std::begin(k_position); it_k != std::end(k_position); ++it_k) {
			child1.job_execution_order[*it_k] = parent2.job_execution_order[*it_k];
			child2.job_execution_order[*it_k] = parent1.job_execution_order[*it_k];
		}
	}

};

template <unsigned int Size, typename Logger>
class GeneticAlgorithmProgramOption {

public:
	template<typename Init, typename CrossOver>
	static void process(const GeneticAlgorithmProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		Population<Size, Logger> population;
		Init::process(program_options, input, population, _n);

		for(unsigned int generation = 0; generation < _g; generation++) {
			Population<Size, Logger> population_crossover;
			for(unsigned int i = 0; i < _n/2; i++) {
				std::vector<DataOutput<Size, Logger>&> output_selected;
				population.rand(output_selected, 2);

				DataOutput<Size, Logger> output_crossover1;
				DataOutput<Size, Logger> output_crossover2;

				CrossOver::process(population_crossover[0], population_crossover[1], output_crossover1, output_crossover2);

				population_crossover.add(output_crossover1);
				population_crossover.add(output_crossover2);
			}
		}

	}

private:
	unsigned int _n;
	unsigned int _g;
};

class GeneticAlgorithm {

public:
	template<typename CrossOver>
	static void process();

private:

};

#endif
