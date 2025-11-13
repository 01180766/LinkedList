#include <barrier>
#include <thread>
#include <benchmark/benchmark.h>

#include "NonLockingLinkedList.h"
#include "LinkedList.h"

constexpr int CORE_COUNT = 16;

auto ll = LinkedList<int>();
auto nll = NonLockingLinkedList<int>();

static void BM_INSERTS_MUTEX(benchmark::State &state) {

    const int counter = state.thread_index() * state.range(0);
    for (auto _: state) {
        for (auto i = 0; i < state.range(0); ++i)
            ll.insert(counter + i, nullptr);
    }
}

static void BM_INSERTS_NONLOCKING(benchmark::State &state) {
    const int counter = state.thread_index() * state.range(0);
    for (auto _: state) {
        for (auto i = 0; i < state.range(0); ++i)
            nll.insert(counter + i, nullptr);
    }
}

BENCHMARK(BM_INSERTS_MUTEX)->Threads(CORE_COUNT)->Arg(10'000)->UseRealTime();

BENCHMARK(BM_INSERTS_NONLOCKING)->Threads(CORE_COUNT)->Arg(10'000)->UseRealTime();

BENCHMARK_MAIN();