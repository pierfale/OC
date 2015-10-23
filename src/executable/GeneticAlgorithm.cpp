#include "solver/GeneticAlgorithmSolver.h"

int main(int argc, const char** argv) {
	return Solver<GeneticAlgorithm, GeneticAlgorithmProgramOption>::execute(argc, argv);
}
