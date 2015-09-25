#include "solver/RandomSolver.h"

int main(int argc, const char** argv) {
	return Solver<RandomSolver, BasicProgramOption>::execute(argc, argv);
}
