#ifndef _NEIGHBORHOOD_H
#define _NEIGHBORHOOD_H

template <unsigned int Size, typename Logger>
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

	void rand() {
		_index1 = ::rand()%Size;
		_index2 = ::rand()%Size;

		if(_index1 == _index2) {
			_index2++;
		}

		if(_index2 >= Size) {
			_index2 = 0;
		}
	}

protected:
	unsigned int _index1;
	unsigned int _index2;

	unsigned int _start_index1;
	unsigned int _start_index2;
};

template <unsigned int Size,  typename Logger>
class NeighborhoodSwap : public NeighborhoodDoubleIndex<Size, Logger> {

public:
	NeighborhoodSwap() : NeighborhoodDoubleIndex<Size, Logger>() {

	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output) {
		bool r = NeighborhoodDoubleIndex<Size, Logger>::incrementIndex();

		if(r) {
			std::swap(output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index1], output.job_execution_order[NeighborhoodDoubleIndex<Size, Logger>::_index2]);
			output.compute_score(input, std::min(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2), std::max(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2));
		}

		return r;
	}

};

template <unsigned int Size,  typename Logger>
class NeighborhoodInsert : public NeighborhoodDoubleIndex<Size, Logger>  {
public:
	NeighborhoodInsert() : NeighborhoodDoubleIndex<Size, Logger>() {

	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output) {
		bool r = NeighborhoodDoubleIndex<Size, Logger>::incrementIndex();

		unsigned int min_index = std::min(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2);
		unsigned int max_index = std::max(NeighborhoodDoubleIndex<Size, Logger>::_index1, NeighborhoodDoubleIndex<Size, Logger>::_index2);

		if(r) {
			unsigned int save_first = output.job_execution_order[min_index];
			unsigned int save_tail[Size-1];
			memcpy(save_tail, output.job_execution_order+min_index+1, (max_index-min_index)*sizeof(unsigned int));
			memcpy(output.job_execution_order+min_index, save_tail, (max_index-min_index)*sizeof(unsigned int));
			output.job_execution_order[max_index] = save_first;
			output.compute_score(input, min_index, max_index);
		}

		return r;
	}

};

template <unsigned int Size,  typename Logger>
class NeighborhoodExchange {

public:

	NeighborhoodExchange() : _index(0), _start_index(_index) {

	}

	void selected() {

	}

	void rand() {
		_index = ::rand()%Size;
	}

	bool process(const DataInput<Size>& input, DataOutput<Size>& output) {
		_index = (_index+1)%(Size);

		std::swap(output.job_execution_order[_index], output.job_execution_order[(_index+1)%Size]);

		if(_index+1 < Size)
			output.compute_score(input, _index, _index+1);
		else
			output.compute_score(input);

		return _index != _start_index;
	}

private:
	unsigned int _index;
	unsigned int _start_index;
};

#endif
