#ifndef _VND_SOLVER_H
#define _VND_SOLVER_H

#include "shared/ProgramOptions.h"
#include "shared/DataInput.h"
#include "shared/DataOutput.h"
#include "shared/Neighborhood.h"
#include "shared/Select.h"

#include "solver/RandomSolver.h"
#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"
#include "solver/PassiveSolver.h"

#include <type_traits>

template <template<unsigned int, typename> class SolverExecution>
class GenerateVNDProgramOption {

public:

	template <unsigned int Size, typename Logger>
	class VNDProgramOption : public BasicProgramOption<Size, Logger> {

		typedef void(*SolverFunction)(const VNDProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&);

	public:

		VNDProgramOption() : _solver(nullptr) {

		}


		void parse(int argc, const char** argv) {
			if(argc < 3)
				throw std::runtime_error("Not enough argument, usage : "+usage());

			BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

			std::string selectOption = "first";
			std::string neighborhoodOption = "exchange,swap,insert";
			std::string initOption = "rnd";

			for(unsigned int i=3; i<argc; i += 2) {
				std::string current(argv[i]);

				if(i>=argc) {
					throw std::runtime_error("no option parameter "+current+", usage : "+usage());
				}

				std::string next(argv[i+1]);

				if(current == "-select")
					selectOption = next;
				else if(current == "-neighborhood-order")
					neighborhoodOption = next;
				else if(current == "-init")
					initOption = next;
				else
					throw std::runtime_error("unknown option "+current+", usage : "+usage());
			}

			if(initOption == "rnd")
				decideInitOption<RandomSolver>(selectOption, neighborhoodOption);
			else if(initOption == "edd")
				decideInitOption<EDDSolver>(selectOption, neighborhoodOption);
			else if(initOption == "mdd")
				decideInitOption<MDDSolver>(selectOption, neighborhoodOption);
			else if(initOption == "none")
				decideInitOption<PassiveSolver>(selectOption, neighborhoodOption);
			else
				throw std::runtime_error("unknown option parameter "+initOption+", usage : "+usage());
		}

		template<typename Init>
		void decideInitOption(const std::string& selectOption, const std::string& neighborhoodOption) {
			if(selectOption == "first")
				decideSelectOption<Init, SelectFirst>(neighborhoodOption);
			else if(selectOption == "best")
				decideSelectOption<Init, SelectBest>(neighborhoodOption);
			else
				throw std::runtime_error("unknown option parameter "+selectOption+", usage : "+usage());
		}

		template<typename Init, template<unsigned int, typename> class Select>
		void decideSelectOption(const std::string& neighborhoodOption) {
			decideNeighborhoodOption<Init, Select, 3>(neighborhoodOption, 0);
		}

		template<typename Init, template<unsigned int, typename> class Select, unsigned int MaxDeep, template<unsigned int, typename> class... Neighborhood>
		typename std::enable_if<(MaxDeep == 0)>::type decideNeighborhoodOption(const std::string& neighborhoodOption, std::size_t cursor) {
			throw std::runtime_error("Max neighborhood length reach (current : "+std::to_string(MaxDeep)+")");
		}

		template<typename Init, template<unsigned int, typename> class Select, unsigned int MaxDeep, template<unsigned int, typename> class... Neighborhood>
		typename std::enable_if<(MaxDeep > 0)>::type decideNeighborhoodOption(const std::string& neighborhoodOption, std::size_t cursor) {
			std::size_t pos = neighborhoodOption.find(',', cursor);


			std::string str = neighborhoodOption.substr(cursor, (pos == std::string::npos ? neighborhoodOption.size() : pos)-cursor);

			if(pos == std::string::npos) {
				if(str == "swap")
					_solver = SolverExecution<Size, Logger>::template solve<VNDProgramOption, Init, Select, Neighborhood..., NeighborhoodSwap>;
				else if(str == "insert")
					_solver = SolverExecution<Size, Logger>::template solve<VNDProgramOption, Init, Select, Neighborhood..., NeighborhoodInsert>;
				else if(str == "exchange") {
					_solver = SolverExecution<Size, Logger>::template solve<VNDProgramOption, Init, Select, Neighborhood..., NeighborhoodExchange>;
				}
				else
					throw std::runtime_error("unknown option parameter "+str+", usage : "+usage());
			}
			else {
				if(str == "swap")
					decideNeighborhoodOption<Init, Select, MaxDeep-1, Neighborhood..., NeighborhoodSwap>(neighborhoodOption, pos == std::string::npos ? pos : pos+1);
				else if(str == "insert")
					decideNeighborhoodOption<Init, Select, MaxDeep-1, Neighborhood..., NeighborhoodInsert>(neighborhoodOption, pos == std::string::npos ? pos : pos+1);
				else if(str == "exchange") {
					decideNeighborhoodOption<Init, Select, MaxDeep-1, Neighborhood..., NeighborhoodExchange>(neighborhoodOption, pos == std::string::npos ? pos : pos+1);
				}
				else
					throw std::runtime_error("unknown option parameter "+str+", usage : "+usage());
			}
		}


		static std::string usage() {
			return std::string("<problem_size> <data_filename> [-init (rnd | edd | mdd)] [-select (first | best)] [-neighborhood-order neighborhood1,neighborhood2,...] ");
		}

		SolverFunction solverFunction() const {
			return _solver;
		}

	private:
		SolverFunction _solver;
	};
};

class VNDSolver {

public:
	VNDSolver() {

	}

	template<template<unsigned int, typename> class T, unsigned int Size, typename Logger>
	static void process(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output);
	}
};

template<unsigned int Size, typename Logger>
class VNDSolverExecution {

public:
	template<template<unsigned int, typename> class T, typename Init, template<unsigned int, typename> class Select, template<unsigned int, typename> class... Neighborhood>
	static void solve(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		Init::process(program_options, input, output);
		while(_VNDSolverFunction<T, Select, Neighborhood...>(program_options, input, output)) Logger::upgradeSolution(input, output);
	}

	template<template<unsigned int, typename> class T, template<unsigned int, typename> class Select>
	static bool _VNDSolverFunction(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		return false;
	}

	template<template<unsigned int, typename> class T, template<unsigned int, typename> class Select, template<unsigned int, typename> class CurrentNeighborhood,  template<unsigned int, typename> class... Neighborhood>
	static bool _VNDSolverFunction(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		CurrentNeighborhood<Size, Logger> neighborhood;
		if(Select<Size, Logger>::process(input, output, neighborhood)) {
			return true;
		}
		else {
			return _VNDSolverFunction<T, Select, Neighborhood...>(program_options, input, output);
		}
	}
};

class VNDPipedSolver {

public:
	VNDPipedSolver() {

	}

	template<template<unsigned int, typename> class T, unsigned int Size, typename Logger>
	static void process(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output);
	}

};

template<unsigned int Size, typename Logger>
class VNDPipedSolverExecution {

public:
	template<template<unsigned int, typename> class T, typename Init, template<unsigned int, typename> class Select, template<unsigned int, typename> class... Neighborhood>
	static void solve(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		Init::process(program_options, input, output);
		_VNDSolverFunction<T, Select, sizeof...(Neighborhood), Neighborhood...>(program_options, input, output);
	}


	template<template<unsigned int, typename> class T, template<unsigned int, typename> class Select, unsigned int NeighborhoodRemain, template<unsigned int, typename> class CurrentNeighborhood, template<unsigned int, typename> class... Neighborhood>
	static typename std::enable_if<(NeighborhoodRemain == 0)>::type _VNDSolverFunction(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {

	}

	template<template<unsigned int, typename> class T,  template<unsigned int, typename> class Select, unsigned int NeighborhoodRemain, template<unsigned int, typename> class CurrentNeighborhood, template<unsigned int, typename> class... Neighborhood>
	static typename std::enable_if<(NeighborhoodRemain > 0)>::type _VNDSolverFunction(const T<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {

		CurrentNeighborhood<Size, Logger> neighborhood;

		if(Select<Size, Logger>::process(input, output, neighborhood)) {
			Logger::upgradeSolution(input, output);
			while(Select<Size, Logger>::process(input, output, neighborhood)) Logger::upgradeSolution(input, output);
			_VNDSolverFunction<T, Select, sizeof...(Neighborhood)+1, Neighborhood..., CurrentNeighborhood>(program_options, input, output);
		}
		else
			_VNDSolverFunction<T, Select, NeighborhoodRemain-1, Neighborhood..., CurrentNeighborhood>(program_options, input, output);
	}
};
#endif
