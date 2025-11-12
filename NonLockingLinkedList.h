// Based on: A Pragmatic Implementation of Non-Blocking Linked Lists
// By: Timothy L.Harris
// Source: https://www.cl.cam.ac.uk/research/srg/netos/papers/2001-caslists.pdf
// doi/10.5555/645958.676105
#ifndef NONLOCKINGNonLockingLinkedList_H
#define NONLOCKINGNonLockingLinkedList_H
#include <atomic>
#include <cstdint>
#include <climits>
#include <iostream>
#include <vector>
#include <cassert>

// Forward-declare for pointer-tag helpers
template<typename T>
struct NBNode;

template<typename T>
class alignas(std::hardware_destructive_interference_size) NonLockingLinkedList {
public:
    NonLockingLinkedList();
    ~NonLockingLinkedList();

    bool insert(long long key, T *data);
    T* remove(long long key);
    T* get(long long key);

    // DEBUG
    void printall();
    std::vector<long long> keys_to_array(); // DEBUG
private:
    NBNode<T> *head;
    NBNode<T> *tail;
    NBNode<T>* search(long long search_key, NBNode<T>** left);

    // Helpers
    static NBNode<T>* mark_ptr(NBNode<T>* p) {
        return reinterpret_cast<NBNode<T>*>(reinterpret_cast<std::uintptr_t>(p) | 1ULL);
    }
    static NBNode<T>* unmark_ptr(NBNode<T>* p) {
        return reinterpret_cast<NBNode<T>*>(reinterpret_cast<std::uintptr_t>(p) & ~1ULL);
    }
    static bool is_marked(NBNode<T>* p) {
        return (reinterpret_cast<std::uintptr_t>(p) & 1ULL) != 0;
    }
};

// Node structure: key, data pointer, atomic next pointer (next may have low bit set for 'marked')
template<typename T>
struct NBNode {
    long long key;
    T* data;
    std::atomic<NBNode<T>*> next;

    NBNode(const long long k, T* d) : key(k), data(d), next(nullptr) {}
};

template<typename T>
NonLockingLinkedList<T>::NonLockingLinkedList() {
    tail = new NBNode<T>(LLONG_MAX, nullptr);
    head = new NBNode<T>(LLONG_MIN, nullptr);
    head->next.store(tail, std::memory_order_relaxed);
}

template<typename T>
bool NonLockingLinkedList<T>::insert(const long long key, T *data) {
    auto *newNode = new NBNode<T>(key, data);
    while (true) {
        NBNode<T> *left, *right = search(key, &left);

        if (right != tail && right->key == key) {
            delete newNode;
            return false;
        }

        newNode->next.store(right, std::memory_order_relaxed);

        if (left->next.compare_exchange_strong(right, newNode,std::memory_order_acq_rel,std::memory_order_acquire))
            return true;
    }
}


template<typename T>
T* NonLockingLinkedList<T>::remove(const long long key) {
    while (true) {
        NBNode<T> *left, *right = search(key, &left);

        if (right == tail || right->key != key) return nullptr;

        auto *right_next = right->next.load(std::memory_order_acquire);

        if (is_marked(right_next)) {
            left->next.compare_exchange_strong(right,unmark_ptr(right_next),std::memory_order_acq_rel,std::memory_order_acquire);
            continue;
        }

        auto *marked = mark_ptr(right_next);
        if (right->next.compare_exchange_strong(right_next,marked,std::memory_order_acq_rel,std::memory_order_acquire)) {
            left->next.compare_exchange_strong(right,right_next,std::memory_order_acq_rel,std::memory_order_acquire);
            return right->data;
        }
    }
}

template<typename T>
T* NonLockingLinkedList<T>::get(const long long key) {
    NBNode<T> *left, *right = search(key, &left);
    if (right == tail || right->key != key) return nullptr;
    auto *next = right->next.load(std::memory_order_acquire);
    if (is_marked(next)) return nullptr;
    return right->data;
}

template<typename T>
NonLockingLinkedList<T>::~NonLockingLinkedList() {
    NBNode<T> *cur = head, *next = nullptr;
    while (cur != nullptr) {
        next = cur->next;
        delete cur;
        cur = next;
    }
}

template<typename T>
void NonLockingLinkedList<T>::printall() {
    auto *cur = unmark_ptr(head->next.load(std::memory_order_acquire));
    while (cur != tail) {
        std::cout << cur->key << ' ';
        cur = unmark_ptr(cur->next.load(std::memory_order_acquire));
    }
    std::cout << '\n';
}

template<typename T>
std::vector<long long> NonLockingLinkedList<T>::keys_to_array() {
    std::vector<long long> vec;
    auto *cur = unmark_ptr(head->next.load(std::memory_order_acquire));
    while (cur != tail) {
        vec.push_back(cur->key);
        cur = unmark_ptr(cur->next.load(std::memory_order_acquire));
    }
    return vec;
}

template<typename T>
NBNode<T>* NonLockingLinkedList<T>::search(const long long search_key, NBNode<T> **left) {
retry:
    NBNode<T> *left_node = head, * left_next = head->next.load(std::memory_order_acquire), *t = left_next;

    while (true) {
        NBNode<T> *t_next = t->next.load(std::memory_order_acquire);

        while (is_marked(t_next)) {
            NBNode<T>* unmarked_t_next = unmark_ptr(t_next);

            if (!left_node->next.compare_exchange_strong(t,unmarked_t_next,std::memory_order_acq_rel,std::memory_order_acquire))
                goto retry;

            t = unmarked_t_next;
            t_next = t->next.load(std::memory_order_acquire);
        }

        if (t == tail || t->key >= search_key) {
            *left = left_node;
            return t;
        }

        left_node = t;
        t = t_next;
    }
}

#endif //NONLOCKINGNonLockingLinkedList_H