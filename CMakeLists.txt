cmake_minimum_required (VERSION 2.8)

project (OC)

macro(copy_files GLOBPAT DESTINATION)
  file(GLOB COPY_FILES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLOBPAT})
  add_custom_target(copy ALL
    COMMENT "Copying files: ${GLOBPAT}")

  foreach(FILENAME ${COPY_FILES})
    set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}")
    set(DST "${DESTINATION}/${FILENAME}")

    add_custom_command(
      TARGET copy
      COMMAND ${CMAKE_COMMAND} -E copy ${SRC} ${DST}
      )
  endforeach(FILENAME)
endmacro(copy_files)

add_definitions(-O3)
add_definitions(-msse -msse2 -msse3 -mfpmath=sse -march=native)
#add_definitions(-g)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories(src)

file (
    GLOB_RECURSE
    shared_files
    src/shared/*
)

file (
    GLOB_RECURSE
    solver_files
    src/solver/*
)

copy_files(data/* .)

# Solvers
add_executable(RandomSolver ${shared_files} ${solver_files} src/executable/Random.cpp)
add_executable(EDDSolver ${shared_files} ${solver_files} src/executable/EDD.cpp)
add_executable(MDDSolver ${shared_files} ${solver_files} src/executable/MDD.cpp)
add_executable(HillClimbingSolver ${shared_files} ${solver_files} src/executable/HillClimbing.cpp)
add_executable(VNDSolver ${shared_files} ${solver_files} src/executable/VND.cpp)
add_executable(VNDPipedSolver ${shared_files} ${solver_files} src/executable/VNDPiped.cpp)
add_executable(TabuSearchSolver ${shared_files} ${solver_files} src/executable/TabuSearch.cpp)
add_executable(ILSSolver ${shared_files} ${solver_files} src/executable/ILS.cpp)
add_executable(VNSSolver ${shared_files} ${solver_files} src/executable/VNS.cpp)
add_executable(GeneticAlgorithm ${shared_files} ${solver_files} src/executable/GeneticAlgorithm.cpp)
add_executable(MemeticAlgorithm ${shared_files} ${solver_files} src/executable/MemeticAlgorithm.cpp)

# Benchmark
add_executable(HillClimbingBenchmark ${shared_files} ${solver_files} src/executable/BenchHillClimbing.cpp)
add_executable(VNDBenchmark ${shared_files} ${solver_files} src/executable/BenchVND.cpp)
add_executable(TabuSearchBenchmark ${shared_files} ${solver_files} src/executable/BenchTabuSearch.cpp)
add_executable(ILSBenchmark ${shared_files} ${solver_files} src/executable/BenchILS.cpp)
add_executable(VNSBenchmark ${shared_files} ${solver_files} src/executable/VNSBenchmark.cpp)
add_executable(GeneticAlgorithmBenchmark ${shared_files} ${solver_files} src/executable/BenchGeneticAlgorithm.cpp)
add_executable(MemeticAlgorithmBenchmark ${shared_files} ${solver_files} src/executable/BenchMemeticAlgorithm.cpp)
