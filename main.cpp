#include <thread>
#include "LinkedList.h"
#include "NonLockingLinkedList.h"

constexpr unsigned int TEST_SIZE = 5'000;
long long placeholder = 1;

void add_values_to_list(LinkedList<long long> &ll, const long long id, const unsigned int test_size) {
    for (auto i = 0; i < test_size; ++i) {
        ll.insert(i + id * test_size, &placeholder);
        std::cout << "Added " << i + id * test_size << " element\n";
    }
}

void add_values_to_list_nonlocking(NonLockingLinkedList<long long> &nll, const long long id, const unsigned int test_size) {
    for (auto i = 0; i < test_size; ++i) {
        nll.insert(i + id * test_size, &placeholder);
        std::cout << "Added " << i + id * test_size << " element\n";
    }
}


int main() {
    // Correctness tests
    std::cout << "Declared core count: " << std::thread::hardware_concurrency() << '\n';
    const auto thread_count = std::thread::hardware_concurrency() - 1;

    std::thread threads[thread_count];
    auto ll = LinkedList<long long>();

    for (auto i = 0; i < thread_count; ++i)
        threads[i] = std::thread(add_values_to_list, std::ref(ll), i, TEST_SIZE);

    for (auto &thread : threads)
        thread.join();

    // Try to find all values
    for (auto i = 0; i < thread_count * TEST_SIZE; ++i) {
        if (auto res = ll.get(i)) {
            if (res != &placeholder) {
                std::cerr << "Incorrect value held under the key " << i << " : " << res << std::endl;
                return EXIT_FAILURE;
            }
        }
        else {
            std::cerr << "Incorrect value held under the key" << i << " : " << res << std::endl;
            return EXIT_FAILURE;
        }
    }
    std::cerr << "Tests passed for locked implementation" << '\n';

    auto nll = NonLockingLinkedList<long long>();
    for (auto i = 0; i < thread_count; ++i)
        threads[i] = std::thread(add_values_to_list_nonlocking, std::ref(nll), i, TEST_SIZE);

    for (auto &thread : threads)
        thread.join();

    // Try to find all values
    for (auto i = 0; i < thread_count * TEST_SIZE; ++i) {
        if (auto res = nll.get(i)) {
            if (res != &placeholder) {
                std::cerr << "Incorrect value held under the key " << i << " : " << res << std::endl;
                return EXIT_FAILURE;
            }
        }
        else {
            std::cerr << "Incorrect value held under the key" << i << " : " << res << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Found element " << i << std::endl;
    }
    std::cerr << "Tests passed for non-locking implementation" << '\n';


    return EXIT_SUCCESS;
}