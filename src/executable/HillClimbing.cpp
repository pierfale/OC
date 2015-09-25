#include "solver/HillClimbingSolver.h"


int main(int argc, const char** argv) {
	return Solver<HillClimbingSolver, HillClimbingProgramOption>::execute(argc, argv);
}
