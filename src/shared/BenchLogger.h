#ifndef _BENCH_LOGGER_H
#define _BENCH_LOGGER_H

class BenchLogger {

public:
	static void initialize();
	static void reset();

	static void verifier();

	static unsigned int cost_call_number();

private:
	BenchLogger();

	static unsigned int _verifier_call_number;
};

#endif
