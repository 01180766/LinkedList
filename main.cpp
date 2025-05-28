#include <iostream>
#include "NonLockingLinkedList.h"

int main() {
    auto ll = NonLockingLinkedList<int>();

    for (auto i = 0; i < 100; ++i) {
        ll.insert(rand() % 100, new int(4));
        ll.printall();
    }

    for (auto const array = ll.keys_to_array(); auto const val: array) {
        delete ll.remove(val);
        ll.printall();
    }

    return EXIT_SUCCESS;
}
