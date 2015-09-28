#ifndef _PARSER_H
#define _PARSER_H

#include <fstream>
#include <stdexcept>
#include "shared/ProgramOptions.h"

template<unsigned int Size, typename Logger>
class BasicParser {

public:
	BasicParser(const BasicProgramOption<Size, Logger>& program_options) : _file(program_options.filename(), std::ios::in) {
		if (!_file.is_open()) {
			throw std::runtime_error("Unable to find input data file \""+program_options.filename()+"\"");
		}
	}

	~BasicParser() {
		_file.close();
	}

	bool read(DataInput<Size>& input) {
		for(unsigned int i=0; i<Size; i++) {
			_file >> input.job_execution_times[i];
		}

		for(unsigned int i=0; i<Size; i++) {
			_file >> input.job_weights[i];
		}

		for(unsigned int i=0; i<Size; i++) {
			_file >> input.job_deadlines[i];
		}

		return !_file.eof();
	}

private:
	BasicParser(const BasicParser& that);
	BasicParser& operator=(const BasicParser& that);

	std::ifstream _file;
};

template<unsigned int Size, typename Logger>
class BigParser {

public:
	BigParser(const BasicProgramOption<Size, Logger>& program_options) : _file(program_options.filename(), std::ios::in) {
		if (!_file.is_open()) {
			throw std::runtime_error("Unable to find input data file \""+program_options.filename()+"\"");
		}
	}

	~BigParser() {
		_file.close();
	}

	bool read(DataInput<Size>& input) {
		for(unsigned int i=0; i<Size; i++) {
			_file >> input.job_execution_times[i];
			_file.ignore(1,',');
			_file >> input.job_weights[i];
			_file.ignore(1,',');
			_file >> input.job_deadlines[i];
		}

		return !_file.eof();
	}

private:
	BigParser(const BigParser& that);
	BigParser& operator=(const BigParser& that);

	std::ifstream _file;
};

#endif
