#include "solver/EDDSolver.h"

int main(int argc, const char** argv) {
	return Solver<EDDSolver, BasicProgramOption>::execute(argc, argv);
}
