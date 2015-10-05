#include "solver/ILSSolver.h"


int main(int argc, const char** argv) {
	return Solver<ILSSolver, ILSProgramOption>::execute(argc, argv);
}
