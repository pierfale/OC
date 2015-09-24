#ifndef _PROGRAM_OPTIONS_H
#define _PROGRAM_OPTIONS_H

#include <string>
#include <stdexcept>

template <unsigned int Size>
class BasicProgramOption {

public:
	BasicProgramOption() : _data_filename() {

	}

	void parse(int argc, const char** argv) {
		if(argc != 3)
			throw std::runtime_error("Not enough argument, usage : "+usage());

		_data_filename = std::string(argv[2]);
	}

	const std::string& filename() const {
		return _data_filename;
	}

	static std::string usage() {
		return std::string("<problem_size> <data_filename>");
	}


protected:
	std::string _data_filename;

};

#endif
