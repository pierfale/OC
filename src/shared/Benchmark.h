#ifndef _BENCHMARK_H
#define _BENCHMARK_H

#include <cassert>
#include <chrono>
#include <map>
#include <vector>

#include "shared/BenchLogger.h"
#include "shared/PassiveLogger.h"

template<unsigned int RunNumber>
class Benchmark {

	typedef unsigned int Time;
	typedef float Score;

public:

	Benchmark() {

	}


	template<typename SolverType, template<unsigned int, typename> class ProgramOptionType>
	int execute(int argc, const char** argv, const char* best_file_pathname, const std::string& bench_name) {

		int problem_size;

		if(argc < 2)
			throw std::runtime_error("usage : "+ProgramOptionType<40, PassiveLogger>::usage());

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

	template<typename SolverType, template<unsigned int, typename> class ProgramOptionType, unsigned int Size, template<unsigned int, typename> class ParserImpl>
	void _execute(int argc, const char** argv, const char* best_file_pathname, const std::string& bench_name) {
		DataInput<Size> input;
		DataOutput<Size> output;

		ProgramOptionType<Size, BenchLogger> program_options;
		program_options.parse(argc, argv);

		ParserImpl<Size, BenchLogger> parser(program_options);

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
			bool optimal_found_list[RunNumber];
			unsigned int nb_cost_call_list[RunNumber];

			for(unsigned int run=0; run<RunNumber; run++) {
				BenchLogger::reset();

				output.reset();
				output.compute_score(input);


				auto t_start = std::chrono::high_resolution_clock::now();
				SolverType::process(program_options, input, output);
				auto t_end = std::chrono::high_resolution_clock::now();

				score_run_list[run] = Verifier::process<Size, PassiveLogger>(input, output);
				//std::cout << "Instance " << cpt << " : " << score_run_list[run] << (best_score[cpt] == score_run_list[run] ? " Optimal !" : "") << std::endl;
				optimal_found_list[run] = best_score[cpt] == score_run_list[run];

				assert(score_run_list[run] >= best_score[cpt]);
				score_run_list[run] = best_score[cpt] == 0 ? score_run_list[run] : Score(100)*(score_run_list[run]-best_score[cpt])/best_score[cpt];

				time_run_list[run] = std::chrono::duration_cast<std::chrono::microseconds>(t_end-t_start).count();
				nb_cost_call_list[run] =  BenchLogger::cost_call_number();
				//cpt++;

			}
			std::cout << std::endl;



			Result result;

			result.average_score = std::accumulate(&score_run_list[0], &score_run_list[RunNumber], (Score)0)/(Score)RunNumber;
			auto minmax_score = std::minmax_element(&score_run_list[0], &score_run_list[RunNumber]);
			result.min_score = *minmax_score.first;
			result.max_score = *minmax_score.second;

			result.average_time = std::accumulate(&time_run_list[0], &time_run_list[RunNumber], (Time)0)/(Time)RunNumber;
			auto minmax_time = std::minmax_element(&time_run_list[0], &time_run_list[RunNumber]);
			result.min_time = *minmax_time.first;
			result.max_time = *minmax_time.second;

			result.optimal_once = std::accumulate(&optimal_found_list[0], &optimal_found_list[RunNumber], false, std::logical_or<bool>()) ? 1 : 0;
			result.optimal_all = std::accumulate(&optimal_found_list[0], &optimal_found_list[RunNumber], true, std::logical_and<bool>()) ? 1 : 0;

			result.nb_cost_call = std::accumulate(&nb_cost_call_list[0], &nb_cost_call_list[RunNumber], 0)/RunNumber;

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
					 << "\t" << it->second[instance].average_time << "\t" << it->second[instance].min_time << "\t" << it->second[instance].max_time
					 << "\t" << it->second[instance].optimal_once << "\t" << it->second[instance].optimal_all << "\t" << it->second[instance].nb_cost_call << std::endl;
			}

			file.close();
		}

		for(auto it = _result.begin(); it != _result.end(); ++it) {
			std::ofstream file(output_file_pathname+"_"+it->first, std::ios::out | std::ios::trunc);
			for(unsigned int instance = 0; instance < nb_instance; instance++) {
				file << (instance+1)<< "\t" << it->second[instance].average_score << "\t" << it->second[instance].min_score << "\t" << it->second[instance].max_score
					 << "\t" << it->second[instance].average_time << "\t" << it->second[instance].min_time << "\t" << it->second[instance].max_time
					 << "\t" << it->second[instance].optimal_once << "\t" << it->second[instance].optimal_all << "\t" << it->second[instance].nb_cost_call << std::endl;
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

					r.optimal_once = r1.optimal_once+r2.optimal_once;
					r.optimal_all = r1.optimal_all+r2.optimal_all;

					r.nb_cost_call = r1.nb_cost_call+r2.nb_cost_call;

					return r;
			});



			file << it->first << "\t" << total.average_score/(Score)nb_instance << "\t" << total.min_score/(Score)nb_instance << "\t" << total.max_score/(Score)nb_instance
				 << "\t" << total.average_time/(Time)nb_instance << "\t" << total.min_time/(Time)nb_instance << "\t" << total.max_time/(Time)nb_instance
				 << "\t" << total.optimal_once << "\t" << total.optimal_all << "\t" << total.nb_cost_call/nb_instance << std::endl;
		}

		file.close();


	}


private:
	class Result {

	public:
		Result () : min_time(0), max_time(0), average_time(0), min_score(0), max_score(0), average_score(0), optimal_once(false), optimal_all(false), nb_cost_call(0) {

		}

		Time min_time;
		Time max_time;
		Time average_time;

		Score min_score;
		Score max_score;
		Score average_score;

		unsigned int optimal_once;
		unsigned int optimal_all;

		unsigned int nb_cost_call;

	};
	std::map<std::string, std::vector<Result>> _result;


};


#endif
