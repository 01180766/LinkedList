#include <barrier>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <benchmark/benchmark.h>

#include "NonLockingLinkedList.h"
#include "LinkedList.h"

constexpr int CORE_COUNT = 16;

LinkedList<int> ll = LinkedList<int>();
NonLockingLinkedList<int> nll = NonLockingLinkedList<int>();
std::shared_mutex m;

static void BM_INSERTS_MUTEX(benchmark::State &state) {

    const int counter = state.thread_index() * state.range(0);
    for (auto _: state) {
        for (auto i = 0; i < state.range(0); ++i) {
            std::scoped_lock lock(m);
            ll.insert(counter + i, nullptr);
        }
    }
}

static void BM_INSERTS_NONLOCKING(benchmark::State &state) {
    const int counter = state.thread_index() * state.range(0);
    for (auto _: state) {
        for (auto i = 0; i < state.range(0); ++i)
            nll.insert(counter + i, nullptr);
    }

    // if (state.thread_index() == 0)
    //     free(nll);
}

BENCHMARK(BM_INSERTS_MUTEX)->Threads(CORE_COUNT)->Arg(10'000)->UseRealTime();

BENCHMARK(BM_INSERTS_NONLOCKING)->Threads(CORE_COUNT)->Arg(10'000)->UseRealTime();

BENCHMARK_MAIN();