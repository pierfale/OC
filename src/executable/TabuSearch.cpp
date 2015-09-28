#include "solver/TabuSearchSolver.h"

int main(int argc, const char** argv) {
	return Solver<TabuSearchSolver, TabuSearchProgramOption>::execute(argc, argv);
}
