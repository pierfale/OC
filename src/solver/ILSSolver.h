#ifndef _ILS_SOLVER_H
#define _ILS_SOLVER_H

#include <algorithm>
#include <chrono>

#include "shared/Solver.h"
#include "solver/RandomSolver.h"
#include "solver/EDDSolver.h"
#include "solver/MDDSolver.h"

#include "shared/Neighborhood.h"
#include "shared/Select.h"

#include "solver/VNDSolver.h"

template <unsigned int Size, typename Logger>
class AcceptBest {

public:
	static bool process(const DataInput<Size>& input, DataOutput<Size>& best_output, DataOutput<Size>& new_output) {
		return Verifier::process<Size, Logger>(input, best_output) > Verifier::process<Size, Logger>(input, new_output);
	}
};

template<template<unsigned int, typename> class Neighborhood>
class GeneratePertubation {

	template <unsigned int Size, typename Logger>
	class Perturbation {

	public:
		static void process(const DataInput<Size>& input, DataOutput<Size>& output) {

			unsigned int n = rand()%9+3;

			Neighborhood<Size, Logger> neighborhood;
			for(unsigned int i=0; i<n; i++) {
				neighborhood.rand();
				neighborhood.process(input, output);
			}

		}
	};
};


template <unsigned int Size, typename Logger>
class ILSProgramOption : public BasicProgramOption<Size, Logger> {

	typedef void(*SolverFunction)(const ILSProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&, void* local_search_program_option);

public:

	ILSProgramOption() : _solver(nullptr) {

	}


	void parse(int argc, const char** argv) {
		if(argc < 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

		std::string localSearchOption = "first";
		std::string pertubationOption = "swap";
		std::string stopOption = "update";

		for(unsigned int i=3; i<argc; i += 2) {
			std::string current(argv[i]);

			if(i>=argc) {
				throw std::runtime_error("no option parameter "+current+", usage : "+usage());
			}

			std::string next(argv[i+1]);

			if(current == "-local-search")
				localSearchOption = next;
			else if(current == "-perturbation")
				pertubationOption = next;
			else if(current == "-stop")
				stopOption = next;
			else
				throw std::runtime_error("unknown option "+current+", usage : "+usage());
		}

		GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption<Size, Logger>* local_search_program_option = new GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption<Size, Logger>();

#define N_ARG 9
		const char* local_search_args[N_ARG];
		local_search_args[0] = argv[0];
		local_search_args[1] = argv[1];
		local_search_args[2] = argv[2];
		local_search_args[3] = "-init";
		local_search_args[4] = "none";
		local_search_args[5] = "-select";
		local_search_args[6] = "best";
		local_search_args[7] = "-neighborhood-order";
		local_search_args[8] = "exchange,insert,swap";


		local_search_program_option->parse(N_ARG, local_search_args);
		_local_search_program_option = local_search_program_option;

		if(pertubationOption == "swap")
			selectStopOption<MDDSolver, VNDSolver, GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption, GeneratePertubation<NeighborhoodSwap>::Perturbation, AcceptBest>(stopOption);
		else if(pertubationOption == "insert")
			selectStopOption<MDDSolver, VNDSolver, GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption, GeneratePertubation<NeighborhoodInsert>::Perturbation, AcceptBest>(stopOption);
		else if(pertubationOption == "exchange")
			selectStopOption<MDDSolver, VNDSolver, GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption, GeneratePertubation<NeighborhoodExchange>::Perturbation, AcceptBest>(stopOption);
		else
			throw std::runtime_error("unknown option "+pertubationOption+", usage : "+usage());
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Perturbation, template<unsigned int, typename> class Acceptance>
	void selectStopOption(const std::string& stopOption) {
		if(stopOption == "update")
			_solver = ILSSolverFunctionCountUpdate<Init, LocalSearch, LocalSearchProgramOption, Perturbation, Acceptance>;
		else if(stopOption == "time")
			_solver = ILSSolverFunctionCountTime<Init, LocalSearch, LocalSearchProgramOption, Perturbation, Acceptance>;
		else
			throw std::runtime_error("unknown option "+stopOption+", usage : "+usage());
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename> [-perturbation (insert | swap | exchange)] [-stop (update | time)]");
	}

	SolverFunction solverFunction() const {
		return _solver;
	}

	void* localSearchProgramOption() const {
		return _local_search_program_option;
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Perturbation, template<unsigned int, typename> class Acceptance>
	static void ILSSolverFunctionCountUpdate(const ILSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {
		Init::process(program_options, input, output);

		LocalSearch::process(*reinterpret_cast<LocalSearchProgramOption<Size, Logger>*>(local_search_program_options), input, output);

		unsigned int count_since_last = 0;
		do {
			DataOutput<Size> current_output = output;
			Perturbation<Size, Logger>::process(input, current_output);
			LocalSearch::process(*reinterpret_cast<LocalSearchProgramOption<Size, Logger>*>(local_search_program_options), input, current_output);

			if(Acceptance<Size, Logger>::process(input, output, current_output)) {
				output = current_output;
				count_since_last = 0;
			}
			else
				 count_since_last++;

		} while(count_since_last < 100);
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Perturbation, template<unsigned int, typename> class Acceptance>
	static void ILSSolverFunctionCountTime(const ILSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {

		auto t_start = std::chrono::high_resolution_clock::now();
		auto t_end = std::chrono::high_resolution_clock::now();
		Init::process(program_options, input, output);

		Logger::enableUpgrade(false);

		LocalSearch::process(*reinterpret_cast<LocalSearchProgramOption<Size, Logger>*>(local_search_program_options), input, output);

		do {
			DataOutput<Size> current_output = output;
			Perturbation<Size, Logger>::process(input, current_output);
			LocalSearch::process(*reinterpret_cast<LocalSearchProgramOption<Size, Logger>*>(local_search_program_options), input, current_output);

			if(Acceptance<Size, Logger>::process(input, output, current_output)) {
				output = current_output;

				Logger::enableUpgrade(true);
				Logger::upgradeSolution(input, output);
				Logger::enableUpgrade(false);
			}

			t_end = std::chrono::high_resolution_clock::now();
		} while(std::chrono::duration_cast<std::chrono::seconds>(t_end-t_start).count() < 1);
	}

private:
	SolverFunction _solver;
	void* _local_search_program_option;

};

class ILSSolver {

public:
	ILSSolver() {

	}

	template<unsigned int Size, typename Logger>
	static void process(const ILSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output, program_options.localSearchProgramOption());
	}

};

#endif
