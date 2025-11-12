#include <iostream>
#include <thread>

#include "NonLockingLinkedList.h"
#include "LinkedList.h"


auto ll = LinkedList<int>();
auto nll = NonLockingLinkedList<int>();
std::mutex m;

constexpr int amount_to_insert = 10000;


void add_values_to_list(const int id) {
    for (auto i = 0; i < amount_to_insert; i+=2) {
        m.lock();
        ll.insert(i + id, new int(4));
        m.unlock();
    }
}

void add_values_to_list_nonlocking(const int id) {
    for (auto i = 0; i < amount_to_insert; i+=2)
        nll.insert(i + id, new int(4));
}

int main() {

    // for (auto i = 0; i < 100; ++i) {
    //     ll.insert(rand() % 100, new int(4));
    //     ll.printall();
    //
    constexpr size_t threads = 4;


    std::thread t[threads];

    auto begin = std::chrono::steady_clock::now();
    for (auto i = 0; i < threads; ++i)
        t[i] = std::thread(add_values_to_list, i);

    for (auto & i : t)
        i.join();
    auto end = std::chrono::steady_clock::now();
    std::cout << "Mutex time for " << threads << " threads:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << '\n';

    for (auto const array = ll.keys_to_array(); auto const val: array) {
        delete ll.remove(val);
        //ll.printall();
    }

    begin = std::chrono::steady_clock::now();
    for (auto i = 0; i < threads; ++i)
        t[i] = std::thread(add_values_to_list_nonlocking, i);

    for (auto & i : t)
        i.join();
    end = std::chrono::steady_clock::now();

    std::cout << "Nonlocking time for " << threads << " threads:" << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << '\n';

    for (auto const array = nll.keys_to_array(); auto const val: array) {
        delete nll.remove(val);
        //nll.printall();
    }


    return EXIT_SUCCESS;
}
