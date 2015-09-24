#include "solver/VNDSolver.h"


int main(int argc, const char** argv) {
	return Solver<VNDPipedSolver, GenerateVNDProgramOption<VNDPipedSolverExecution>::VNDProgramOption>::execute(argc, argv);
}
