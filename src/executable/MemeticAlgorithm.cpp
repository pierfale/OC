#include "solver/MemeticAlgorithmSolver.h"

int main(int argc, const char** argv) {
	return Solver<MemeticAlgorithm, MemeticAlgorithmProgramOption>::execute(argc, argv);
}
