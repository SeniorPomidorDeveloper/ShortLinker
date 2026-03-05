#include <cstdint>
#include <string>

#include <benchmark/benchmark.h>

void GenerateIdBenchmark(benchmark::State& state) {
    for (auto _ : state) {
        // Simulate the kind of string operations done during ID generation.
        std::string id;
        id.reserve(8);
        for (int i = 0; i < 8; ++i) {
            id += static_cast<char>('a' + (i % 26));
        }
        benchmark::DoNotOptimize(id);
    }
}

BENCHMARK(GenerateIdBenchmark);
