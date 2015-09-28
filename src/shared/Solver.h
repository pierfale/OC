#ifndef _SOLVER_H
#define _SOLVER_H

#include <string>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <ctime>
#include <iomanip>
#include <chrono>

#include "shared/DataInput.h"
#include "shared/DataOutput.h"
#include "shared/Parser.h"
#include "shared/Verifier.h"
#include "shared/PassiveLogger.h"

template<typename SolverType, template<unsigned int, typename> class ProgramOptionType>
class Solver {

public:
	static const char* usage() {
		return "usage : <input_size> <input_filename>";
	}

	static int execute(int argc, const char** argv) {

		int problem_size;

		if(argc < 2)
			throw std::runtime_error("usage : "+ProgramOptionType<40, PassiveLogger>::usage());

		problem_size = atoi(argv[1]);

		switch(problem_size) {

		case 40:
			_execute<40, BasicParser>(argc, argv);
			break;
		case 50:
			_execute<50, BasicParser>(argc, argv);
			break;
		case 100:
			_execute<100, BasicParser>(argc, argv);
			break;
		case 1000:
			_execute<1000, BigParser>(argc, argv);
			break;
		default:
			throw std::runtime_error("Unhandled problem size");
		}

		return EXIT_SUCCESS;
	}

	template<unsigned int Size, template<unsigned int, typename> class ParserImpl>
	static int _execute(int argc, const char** argv) {
		DataInput<Size> input;
		DataOutput<Size> output;

		ProgramOptionType<Size, PassiveLogger> program_options;
		program_options.parse(argc, argv);

		ParserImpl<Size, PassiveLogger> parser(program_options);

		std::clock_t c_start = std::clock();
		auto t_start = std::chrono::high_resolution_clock::now();

		unsigned int cpt = 0;

		while(parser.read(input)) {
			output.reset();
			output.compute_score(input);

			SolverType::process(program_options, input, output);

			std::cout << "Instance " << (++cpt) << " : score=" << Verifier::process<Size, PassiveLogger>(input, output) << std::endl;
		}

		std::clock_t c_end = std::clock();
			auto t_end = std::chrono::high_resolution_clock::now();

			std::cout << std::fixed << std::setprecision(2) << "CPU time used: "
					  << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
					  << "Wall clock time passed: "
					  << std::chrono::duration<double, std::milli>(t_end-t_start).count()
					  << " ms\n";

	}

private:
	Solver();
	Solver(const Solver& that);
	Solver& operator=(const Solver& that);

};

#endif
