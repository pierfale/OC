#ifndef _VNS_SOLVER_H
#define _VNS_SOLVER_H

#include <algorithm>

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

template <unsigned int Size, typename Logger>
class VNSProgramOption : public BasicProgramOption<Size, Logger> {

	typedef void(*SolverFunction)(const VNSProgramOption<Size, Logger>& program_options, const DataInput<Size>&, DataOutput<Size>&, void* local_search_program_option);

public:

	VNSProgramOption() : _solver(nullptr) {

	}


	void parse(int argc, const char** argv) {
		if(argc < 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		BasicProgramOption<Size, Logger>::_data_filename = std::string(argv[2]);

		std::string neighborhoodOrderOption = "swap";
		std::string stopOption = "update";

		for(unsigned int i=3; i<argc; i += 2) {
			std::string current(argv[i]);

			if(i>=argc) {
				throw std::runtime_error("no option parameter "+current+", usage : "+usage());
			}

			std::string next(argv[i+1]);

			if(current == "-neighborhood-order")
				neighborhoodOrderOption = next;
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
		local_search_args[8] = neighborhoodOrderOption.c_str();


		local_search_program_option->parse(N_ARG, local_search_args);
		_local_search_program_option = local_search_program_option;

		decideNeighborhoodOption<MDDSolver, VNDSolver, GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption, AcceptBest, 3>(neighborhoodOrderOption, 0, stopOption);
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance, unsigned int MaxDeep, template<unsigned int, typename> class... Neighborhood>
	typename std::enable_if<(MaxDeep == 0)>::type decideNeighborhoodOption(const std::string& neighborhoodOption, std::size_t cursor, const std::string& stopOption) {
		throw std::runtime_error("Max neighborhood length reach (current : "+std::to_string(MaxDeep)+")");
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance, unsigned int MaxDeep, template<unsigned int, typename> class... Neighborhood>
	typename std::enable_if<(MaxDeep > 0)>::type decideNeighborhoodOption(const std::string& neighborhoodOption, std::size_t cursor, const std::string& stopOption) {
		std::size_t pos = neighborhoodOption.find(',', cursor);


		std::string str = neighborhoodOption.substr(cursor, (pos == std::string::npos ? neighborhoodOption.size() : pos)-cursor);

		if(pos == std::string::npos) {
			if(str == "swap")
				selectStopOption<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood..., NeighborhoodSwap>(stopOption);
			else if(str == "insert")
				selectStopOption<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood..., NeighborhoodInsert>(stopOption);
			else if(str == "exchange") {
				selectStopOption<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood..., NeighborhoodExchange>(stopOption);
			}
			else
				throw std::runtime_error("unknown option parameter "+str+", usage : "+usage());
		}
		else {
			if(str == "swap")
				decideNeighborhoodOption<Init, LocalSearch, LocalSearchProgramOption, Acceptance, MaxDeep-1, Neighborhood..., NeighborhoodSwap>(neighborhoodOption, pos == std::string::npos ? pos : pos+1, stopOption);
			else if(str == "insert")
				decideNeighborhoodOption<Init, LocalSearch, LocalSearchProgramOption, Acceptance, MaxDeep-1, Neighborhood..., NeighborhoodInsert>(neighborhoodOption, pos == std::string::npos ? pos : pos+1, stopOption);
			else if(str == "exchange") {
				decideNeighborhoodOption<Init, LocalSearch, LocalSearchProgramOption, Acceptance, MaxDeep-1, Neighborhood..., NeighborhoodExchange>(neighborhoodOption, pos == std::string::npos ? pos : pos+1, stopOption);
			}
			else
				throw std::runtime_error("unknown option parameter "+str+", usage : "+usage());
		}
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance, template<unsigned int, typename> class... Neighborhood>
	void selectStopOption(const std::string& stopOption) {
		if(stopOption == "update")
			_solver = VNSSolverFunctionCountUpdate<Init, LocalSearch, LocalSearchProgramOption,Acceptance, Neighborhood...>;
		else if(stopOption == "time")
			_solver = VNSSolverFunctionCountTime<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood...>;
		else
			throw std::runtime_error("unknown option "+stopOption+", usage : "+usage());
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename> [-neighborhood-order neighborhood1,neighborhood2,...] [-stop (update | time)]");
	}

	SolverFunction solverFunction() const {
		return _solver;
	}

	void* localSearchProgramOption() const {
		return _local_search_program_option;
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance, template<unsigned int, typename> class... Neighborhood>
	static void VNSSolverFunctionCountUpdate(const VNSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {
		Init::process(program_options, input, output);

		unsigned count = 0;
		Logger::enableUpgrade(false);


		do {
			unsigned int score = Verifier::process<Size, Logger>(input, output);
			Logger::enableUpgrade(true);
			_VNSSolverFunction<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood...>(program_options, input, output, local_search_program_options);
			Logger::enableUpgrade(false);
			count = Verifier::process<Size, Logger>(input, output) > score ? 0 : count+1;
		}
		while(count < 100);
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance, template<unsigned int, typename> class... Neighborhood>
	static void VNSSolverFunctionCountTime(const VNSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {
		auto t_start = std::chrono::high_resolution_clock::now();
		auto t_end = std::chrono::high_resolution_clock::now();
		Init::process(program_options, input, output);

		Logger::enableUpgrade(false);
		do {
			Logger::enableUpgrade(true);
			_VNSSolverFunction<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood...>(program_options, input, output, local_search_program_options);
			Logger::enableUpgrade(false);

			t_end = std::chrono::high_resolution_clock::now();
		} while(std::chrono::duration_cast<std::chrono::seconds>(t_end-t_start).count() < 60);
	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance>
	static void _VNSSolverFunction(const VNSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {

	}

	template<typename Init, typename LocalSearch, template<unsigned int, typename>  class LocalSearchProgramOption, template<unsigned int, typename> class Acceptance, template<unsigned int, typename> class CurrentNeighborhood, template<unsigned int, typename> class... Neighborhood>
	static void _VNSSolverFunction(const VNSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output, void* local_search_program_options) {
		DataOutput<Size> init_output(output);

		CurrentNeighborhood<Size, Logger> neighborhood;

		neighborhood.rand();
		neighborhood.process(input, output);

		LocalSearch::process(*reinterpret_cast<LocalSearchProgramOption<Size, Logger>*>(local_search_program_options), input, output);

		if(Acceptance<Size, Logger>::process(input, init_output, output)) {
			return;
		}
		else {
			output = init_output;
			_VNSSolverFunction<Init, LocalSearch, LocalSearchProgramOption, Acceptance, Neighborhood...>(program_options, input, output, local_search_program_options);
		}
	}

private:
	SolverFunction _solver;
	void* _local_search_program_option;

};

class VNSSolver {

public:
	VNSSolver() {

	}

	template<unsigned int Size, typename Logger>
	static void process(const VNSProgramOption<Size, Logger>& program_options, const DataInput<Size>& input, DataOutput<Size>& output) {
		program_options.solverFunction()(program_options, input, output, program_options.localSearchProgramOption());
	}

};

#endif
