#ifndef _POPULATION_H
#define _POPULATION_H

template <unsigned int Size, typename Logger>
class Population {

public:
	Population() : _population() {

	}

	void add(const DataOutput<Size>& output) {
		_population.push_back(output);
	}

	template<typename Logger2>
	void add(const Population<Size, Logger2> population) {
		_population.insert(_population.end(), population.begin(), population.end());
	}

	unsigned int size() const {
		return _population.size();
	}

	template<template<typename, typename...> class Output>
	void rand(Output<DataOutput<Size>>& output, unsigned int n) {
		std::vector<unsigned int> _index;
		while(n>0) {
			unsigned int index;

			do {
				index = ::rand()%_population.size();
			} while(std::find(std::begin(_index), std::end(_index), index) != std::end(_index));

			_index.push_back(index);

			n--;
		}
		for(auto it=std::begin(_index); it != std::end(_index); ++it) {
			output.push_back(_population[*it]);
		}
	}

	void keepBest(const DataInput<Size>& input, unsigned int n) {
		std::sort(std::begin(_population), std::end(_population), [&input, this](const DataOutput<Size>& output1, const DataOutput<Size>& output2) {
			return Verifier::process<Size, Logger>(input, output1) < Verifier::process<Size, Logger>(input, output2);
		});

		_population.erase(std::begin(_population)+n, std::end(_population));
/*
		for(auto it=std::begin(_population); it != std::end(_population); ++it)
			std::cout << Verifier::process<Size, Logger>(input, *it) << " ";
		std::cout << std::endl;*/
	}

	DataOutput<Size>& best(const DataInput<Size>& input) {
		return *std::max_element(std::begin(_population), std::end(_population), [&input, this](const DataOutput<Size>& output1, const DataOutput<Size>& output2) {
			return Verifier::process<Size, Logger>(input, output1) < Verifier::process<Size, Logger>(input, output2);
		});
	}


private:
	std::vector<DataOutput<Size>> _population;
};

#endif
