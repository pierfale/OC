#ifndef _PASSIVE_LOGGER_H
#define _PASSIVE_LOGGER_H

#include "shared/DataInput.h"
#include "shared/DataOutput.h"


class PassiveLogger {

public:
	static void verifier();

	template<unsigned int Size>
	static void upgradeSolution(const DataInput<Size>& input, DataOutput<Size>& output) {

	}

	static void enableUpgrade(bool state);
};

#endif
