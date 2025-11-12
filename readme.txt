This should be turned into proper readme.md file later:
To compile this project run:

g++ INPUT_FILE -std=c++20 -isystem benchmark/include   -Lbenchmark/build/src -lbenchmark -lpthread -o OUTPUT_NAME

for example:

g++ main.cpp -std=c++20 -isystem benchmark/include   -Lbenchmark/build/src -lbenchmark -lpthread -o main

g++ benchmark.cc -std=c++20 -isystem benchmark/include   -Lbenchmark/build/src -lbenchmark -lpthread -o benchmarks




