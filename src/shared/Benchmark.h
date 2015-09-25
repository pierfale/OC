#ifndef _BENCHMARK_H
#define _BENCHMARK_H

#include <cassert>
#include <chrono>
#include <map>
#include <vector>

template<unsigned int RunNumber>
class Benchmark {

	typedef unsigned int Time;
	typedef float Score;

public:

	Benchmark() {

	}


	template<typename SolverType, template<unsigned int> class ProgramOptionType>
	int execute(int argc, const char** argv, const char* best_file_pathname, const std::string& bench_name) {

		int problem_size;

		if(argc < 2)
			throw std::runtime_error("usage : "+ProgramOptionType<40>::usage());

		problem_size = atoi(argv[1]);

		switch(problem_size) {

		case 40:
			_execute<SolverType, ProgramOptionType, 40, BasicParser>(argc, argv, best_file_pathname, bench_name);
			break;
		case 50:
			_execute<SolverType, ProgramOptionType, 50, BasicParser>(argc, argv, best_file_pathname, bench_name);
			break;
		case 100:
			_execute<SolverType, ProgramOptionType, 100, BasicParser>(argc, argv, best_file_pathname, bench_name);
			break;
		case 1000:
			_execute<SolverType, ProgramOptionType, 1000, BigParser>(argc, argv, best_file_pathname, bench_name);
			break;
		default:
			throw std::runtime_error("Unhandled problem size");
		}

		return EXIT_SUCCESS;
	}

	template<typename SolverType, template<unsigned int> class ProgramOptionType, unsigned int Size, template<unsigned int> class ParserImpl>
	void _execute(int argc, const char** argv, const char* best_file_pathname, const std::string& bench_name) {
		DataInput<Size> input;
		DataOutput<Size> output;

		ProgramOptionType<Size> program_options;
		program_options.parse(argc, argv);

		ParserImpl<Size> parser(program_options);

		std::cout  << "Launch " << bench_name << std::endl;

		/*
		 *	Read optimals
		 */
		std::ifstream best_file(best_file_pathname, std::ios::in);

		if(!best_file.is_open()) {
			throw std::runtime_error("Unable to open \""+std::string(best_file_pathname)+"\"");
		}

		std::vector<Score> best_score;
		while(!best_file.eof()) {
			Score current_score;
			best_file >> current_score;
			best_score.push_back(current_score);
		}

		best_file.close();

		_result.insert(std::pair<std::string, std::vector<Result>>(bench_name, std::vector<Result>()));

		unsigned int cpt  = 0;

		while(parser.read(input)) {
			Score score_run_list[RunNumber];
			Time time_run_list[RunNumber];

			for(unsigned int run=0; run<RunNumber; run++) {
				output.reset();

				auto t_start = std::chrono::high_resolution_clock::now();
				SolverType::process(program_options, input, output);
				auto t_end = std::chrono::high_resolution_clock::now();

				score_run_list[run] = Verifier::process(input, output);

				assert(score_run_list[run] >= best_score[cpt]);
				score_run_list[run] = best_score[cpt] == 0 ? score_run_list[run] : Score(100)*(score_run_list[run]-best_score[cpt])/best_score[cpt];

				time_run_list[run] = std::chrono::duration_cast<std::chrono::microseconds>(t_end-t_start).count();
			}


			Result result;

			result.average_score = std::accumulate(&score_run_list[0], &score_run_list[RunNumber], (Score)0)/(Score)RunNumber;
			auto minmax_score = std::minmax_element(&score_run_list[0], &score_run_list[RunNumber]);
			result.min_score = *minmax_score.first;
			result.max_score = *minmax_score.second;

			result.average_time = std::accumulate(&time_run_list[0], &time_run_list[RunNumber], (Time)0)/(Time)RunNumber;
			auto minmax_time = std::minmax_element(&time_run_list[0], &time_run_list[RunNumber]);
			result.min_time = *minmax_time.first;
			result.max_time = *minmax_time.second;

			_result[bench_name].push_back(result);
			cpt++;
		}
	}

	void save(const std::string& output_file_pathname) {

		int nb_instance = _result.begin()->second.size();

		for(unsigned int instance = 0; instance < nb_instance; instance++) {
			std::ofstream file(output_file_pathname+"_"+std::to_string(instance), std::ios::out | std::ios::trunc);

			for(auto it = _result.begin(); it != _result.end(); ++it) {
				file << it->first << "\t" << it->second[instance].average_score << "\t" << it->second[instance].min_score << "\t" << it->second[instance].max_score
						<< "\t" << it->second[instance].average_time << "\t" << it->second[instance].min_time << "\t" << it->second[instance].max_time;
			}

			file.close();
		}

		std::ofstream file(output_file_pathname, std::ios::out | std::ios::trunc);

		for(auto it = _result.begin(); it != _result.end(); ++it) {
			Result total = std::accumulate(it->second.begin(), it->second.end(), Result(), [](Result r1, Result r2) {
					Result r;
					r.average_score = r1.average_score+r2.average_score;
					r.min_score = r1.min_score+r2.min_score;
					r.max_score = r1.max_score+r2.max_score;

					r.average_time = r1.average_time+r2.average_time;
					r.min_time = r1.min_time+r2.min_time;
					r.max_time = r1.max_time+r2.max_time;

					return r;
			});


			file << it->first << "\t" << total.average_score/(Score)nb_instance << "\t" << total.min_score/(Score)nb_instance << "\t" << total.max_score/(Score)nb_instance
					<< "\t" << total.average_time/(Time)nb_instance << "\t" << total.min_time/(Time)nb_instance << "\t" << total.max_time/(Time)nb_instance;
		}

		file.close();
	}


private:
	class Result {

	public:
		Result () : min_time(0), max_time(0), average_time(0), min_score(0), max_score(0), average_score(0) {

		}

		Time min_time;
		Time max_time;
		Time average_time;

		Score min_score;
		Score max_score;
		Score average_score;

	};
	std::map<std::string, std::vector<Result>> _result;


};


#endif
