#include "solver/VNDSolver.h"

int main(int argc, const char** argv) {

	return Solver<VNDSolver, GenerateVNDProgramOption<VNDSolverExecution>::VNDProgramOption>::execute(argc, argv);
}
