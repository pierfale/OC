#ifndef _HILL_CLIMBING_SOLVER_H
#define _HILL_CLIMBING_SOLVER_H

#include <algorithm>
#include <chrono>

#include "shared/Solver.h"
#include "solver/RandomSolver.h"
#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"
#include "solver/PassiveSolver.h"

#include "shared/Neighborhood.h"
#include "shared/Select.h"

template <unsigned int Size, typename Logger>
class HillClimbingProgramOption : public BasicProgramOption<Size, Logger> {

	typedef void(*SolverFunction)(const HillClimbingProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&);

public:

	HillClimbingProgramOption() : _solver(nullptr) {

	}


	void parse(int argc, const char** argv) {
		if(argc < 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

		std::string selectOption = "first";
		std::string neighborhoodOption = "first";
		std::string initOption = "rnd";

		for(unsigned int i=3; i<argc; i += 2) {
			std::string current(argv[i]);

			if(i>=argc) {
				throw std::runtime_error("no option parameter "+current+", usage : "+usage());
			}

			std::string next(argv[i+1]);

			if(current == "-select")
				selectOption = next;
			else if(current == "-neighborhood")
				neighborhoodOption = next;
			else if(current == "-init")
				initOption = next;
			else
				throw std::runtime_error("unknown option "+current+", usage : "+usage());
		}

		if(selectOption == "first")
			decideSelectOption<SelectFirst>(neighborhoodOption, initOption);
		else if(selectOption == "best")
			decideSelectOption<SelectBest>(neighborhoodOption, initOption);
		else
			throw std::runtime_error("unknown option "+selectOption+", usage : "+usage());

	}

	template<template<unsigned int, typename> class Select>
	void decideSelectOption(const std::string& neighborhoodOption, const std::string& initOption) {
		if(neighborhoodOption == "insert")
			decideNeighborhoodOption<Select, NeighborhoodInsert>(initOption);
		else if(neighborhoodOption == "swap")
			decideNeighborhoodOption<Select, NeighborhoodSwap>(initOption);
		else if(neighborhoodOption == "exchange") {
			decideNeighborhoodOption<Select, NeighborhoodExchange>(initOption);
		}
		else
			throw std::runtime_error("unknown option "+neighborhoodOption+", usage : "+usage());
	}

	template<template<unsigned int, typename> class Select, template<unsigned int, typename> class Neighborhood>
	void decideNeighborhoodOption(const std::string& initOption) {
		if(initOption == "rnd")
			_solver = hillClimbingSolverFunction<Select, Neighborhood, RandomSolver>;
		else if(initOption == "edd")
			_solver = hillClimbingSolverFunction<Select, Neighborhood, EDDSolver>;
		else if(initOption == "mdd")
			_solver = hillClimbingSolverFunction<Select, Neighborhood, MDDSolver>;
		else if(initOption == "none")
			_solver = hillClimbingSolverFunction<Select, Neighborhood, PassiveSolver>;
		else
			throw std::runtime_error("unknown option "+initOption+", usage : "+usage());
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename> [-select (first | best)] [-neighborhood (insert | swap | exchange)] [-init(rnd | edd | mdd)]");
	}

	SolverFunction solverFunction() const {
		return _solver;
	}

	template<template<unsigned int, typename> class Select, template<unsigned int, typename> class Neighborhood, typename Init>
	static void hillClimbingSolverFunction(const HillClimbingProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		Init::process(program_options, input, output);
		Neighborhood<Size, Logger> neighborhood;

		while(Select<Size, Logger>::process(input, output, neighborhood)) Logger::upgradeSolution(input, output);
	}

private:
	SolverFunction _solver;
};

class HillClimbingSolver {

public:
	HillClimbingSolver() {

	}

	template<unsigned int Size, typename Logger>
	static void process(const HillClimbingProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output);
	}

};

#endif
