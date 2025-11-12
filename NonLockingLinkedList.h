//
// Created by Piotr Czarnecki on 5/28/25.
//
// Na postawie: A Pragmatic Implementation of Non-Blocking Linked Lists
// Autorstwa: Timothy L.Harris
// Źródło: https://www.cl.cam.ac.uk/research/srg/netos/papers/2001-caslists.pdf
// doi/10.5555/645958.676105

#ifndef NONLOCKINGNonLockingLinkedList_H
#define NONLOCKINGNonLockingLinkedList_H
#include <atomic>
#include <vector>
#include <climits>

template<typename T>
struct NonLockingNode {
    int key;
    T* data;
    std::atomic<NonLockingNode*> next;
    std::atomic<bool> marked_for_deletion;
    NonLockingNode(const int key, T* data) : key(key), data(data), next(nullptr), marked_for_deletion(false) {}
};

// TODO: Figure out memory orders

template<typename T>
class NonLockingLinkedList {
    // it's sorted
public:
    bool insert(int key, T *data);
    T* remove(int key);
    T* get(int key);
    ~NonLockingLinkedList();
    NonLockingLinkedList();
    void printall();
    std::vector<T*> to_array(); // DEBUG
    std::vector<int> keys_to_array(); // DEBUG
private:
    NonLockingNode<T> *head = new NonLockingNode<T>(0, nullptr);
    NonLockingNode<T> *tail = new NonLockingNode<T>(INT_MAX, nullptr);
    NonLockingNode<T> *search(int search_key, NonLockingNode<T>** left_node);
    static NonLockingNode<T> *get_marked_reference(NonLockingNode<T> *node);
    static NonLockingNode<T> *get_unmarked_reference(NonLockingNode<T> *node);
};

template<typename T>
bool NonLockingLinkedList<T>::insert(const int key, T *data) {

    NonLockingNode<T> *left_node, *right_node, *newNode = new NonLockingNode<T>(key, data);
    do {
        right_node = search(key, &left_node);

        if (right_node != tail && right_node->key == key) {
            delete newNode;
            return false;
        }

        newNode->next.store(right_node);

    } while (!left_node->next.compare_exchange_weak(right_node, newNode, std::memory_order_relaxed));
    return true;
}

template<typename T>
T* NonLockingLinkedList<T>::remove(const int key) {
    NonLockingNode<T> *left_node, *right_node, *right_node_next;
    do {
        right_node = search(key, &left_node);
        if (right_node == tail || right_node->key != key)
            return nullptr;
        right_node_next = right_node->next.load();
        // is marked reference
        if (!right_node_next->marked_for_deletion.load()) {
            if (right_node->next.compare_exchange_strong(right_node_next, right_node, std::memory_order_relaxed))
                break;
        }

    } while (true);
    if (!left_node->next.compare_exchange_strong(right_node, right_node_next, std::memory_order_relaxed))
        right_node = search(right_node->key, &left_node);
    return right_node->data;
}

template<typename T>
T* NonLockingLinkedList<T>::get(const int key) {
    NonLockingNode<T> *left_node;
    auto *right_node = search(key, &left_node);
    if (right_node == tail || right_node->key != key)
        return nullptr;
    return right_node->data;
}

template<typename T>
NonLockingLinkedList<T>::~NonLockingLinkedList() {
    NonLockingNode<T>* cur = head, *next = nullptr;
    while (cur != nullptr) {
        next = cur->next;
        delete cur;
        cur = next;
    }
}

template<typename T>
NonLockingLinkedList<T>::NonLockingLinkedList() {
    head->next.store(tail);
}

template<typename T>
void NonLockingLinkedList<T>::printall() {
    auto cur = head;
    while (cur != nullptr) {
        std::cout << cur->key << ' ';
        cur = cur->next.load(std::memory_order_relaxed);
    }
    std::cout << '\n';
}

template<typename T>
std::vector<T*> NonLockingLinkedList<T>::to_array() {
    std::vector<T*> vec;
    auto cur = head->next.load();
    while (cur != nullptr) {
        vec.push_back(cur->data);
        cur = cur->next.load();
    }
    vec.pop_back();
    return vec;
}

template<typename T>
std::vector<int> NonLockingLinkedList<T>::keys_to_array() {
    std::vector<int> vec;
    auto cur = head->next.load();
    while (cur != nullptr) {
        vec.push_back(cur->key);
        cur = cur->next.load();
    }
    vec.pop_back();
    return vec;
}

template<typename T>
NonLockingNode<T>* NonLockingLinkedList<T>::search(const int search_key, NonLockingNode<T> **left_node) {
    NonLockingNode<T> *left_node_next, *right_node;

    search_again:
        while (true) {
            NonLockingNode<T> *t = head, *t_next = head->next.load();
            // 1: Find left_node and right_node
            do {
                if (!t_next->marked_for_deletion.load()) {
                    *left_node = t;
                    left_node_next = t_next;
                }
                t = get_unmarked_reference(t_next);
                if (t == tail) break;
                t_next = t->next.load();

            } while (t_next->marked_for_deletion.load() || t->key < search_key);
            right_node = t;

            // 2: Check nodes are adjacent
            if (left_node_next == right_node) {
                if (right_node != tail && right_node->next.load()->marked_for_deletion.load())
                    goto search_again;
                return right_node;
            }

            // 3. Remove one or more marked nodes
            if ((*left_node)->next.compare_exchange_strong(left_node_next, right_node, std::memory_order_relaxed)) {
                if (right_node != tail && right_node->next.load()->marked_for_deletion.load())
                    goto search_again;
                return right_node;
            }
        }
}

template<typename T>
NonLockingNode<T> * NonLockingLinkedList<T>::get_marked_reference(NonLockingNode<T> *node) {
    node->marked_for_deletion.store(true);
    return node;
}

template<typename T>
NonLockingNode<T> * NonLockingLinkedList<T>::get_unmarked_reference(NonLockingNode<T> *node) {
    node->marked_for_deletion.store(false);
    return node;
}


#endif //NONLOCKINGNonLockingLinkedList_H
