#include "solver/MDDSolver.h"

int main(int argc, const char** argv) {
	return Solver<MDDSolver, BasicProgramOption>::execute(argc, argv);
}
