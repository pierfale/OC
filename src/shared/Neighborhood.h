#ifndef _NEIGHBORHOOD_H
#define _NEIGHBORHOOD_H

template <unsigned int Size>
class NeighborhoodDoubleIndex {

public:
	NeighborhoodDoubleIndex() : _index1(0), _index2(1), _start_index1(_index1), _start_index2(_index2) {

	}

	void selected() {
		_start_index1 = _index1;
		_start_index2 = _index2;
	}

	bool incrementIndex() {
		_index1++;

		if(_index1 == _index2) {
			_index1++;
		}

		if(_index1 >= Size) {
			_index2++;
			_index1 = 0;
		}

		if(_index1 == _index2) {
			_index1++;
		}

		if(_index2 >= Size) {
			_index1 = 0;
			_index2 = 1;
		}

		return _index1 != _start_index1 || _index2 != _start_index2;
	}

protected:
	unsigned int _index1;
	unsigned int _index2;

	unsigned int _start_index1;
	unsigned int _start_index2;
};

template <unsigned int Size>
class NeighborhoodSwap : public NeighborhoodDoubleIndex<Size> {

public:
	NeighborhoodSwap() : NeighborhoodDoubleIndex<Size>() {

	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output) {
		bool r = NeighborhoodDoubleIndex<Size>::incrementIndex();

		if(r)
			std::swap(output.job_execution_order[NeighborhoodDoubleIndex<Size>::_index1], output.job_execution_order[NeighborhoodDoubleIndex<Size>::_index2]);

		return r;
	}

};

template <unsigned int Size>
class NeighborhoodInsert : public NeighborhoodDoubleIndex<Size>  {
public:
	NeighborhoodInsert() : NeighborhoodDoubleIndex<Size>() {

	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output) {
		bool r = NeighborhoodDoubleIndex<Size>::incrementIndex();

		unsigned int min_index = std::min(NeighborhoodDoubleIndex<Size>::_index1, NeighborhoodDoubleIndex<Size>::_index2);
		unsigned int max_index = std::max(NeighborhoodDoubleIndex<Size>::_index1, NeighborhoodDoubleIndex<Size>::_index2);

		if(r) {
			unsigned int save_first = output.job_execution_order[min_index];
			unsigned int save_tail[Size-1];
			memcpy(save_tail, output.job_execution_order+min_index+1, (max_index-min_index)*sizeof(unsigned int));
			memcpy(output.job_execution_order+min_index, save_tail, (max_index-min_index)*sizeof(unsigned int));
			output.job_execution_order[max_index] = save_first;
		}

		return r;
	}

};

template <unsigned int Size>
class NeighborhoodExchange {

public:

	NeighborhoodExchange() : _index(0), _start_index(_index) {

	}

	void selected() {

	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output) {
		_index = (_index+1)%(Size);

		std::swap(output.job_execution_order[_index], output.job_execution_order[(_index+1)%Size]);

		return _index != _start_index;
	}

private:
	unsigned int _index;
	unsigned int _start_index;
};

#endif
