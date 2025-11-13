#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <climits>
#include <iostream>
#include <shared_mutex>
#include <vector>

template<typename T>
struct Node {
    long long key;
    T* data;
    Node* next = nullptr;
    Node(const long long key, T *data) : key(key), data(data) {}
};

template<typename T>
class LinkedList final {
public:
    ~LinkedList();
    LinkedList();

    bool insert(long long key, T *data);
    T* remove(long long key);
    T* get(long long key);

    // DEBUG
    void printall();
    std::vector<T*> to_array();
    std::vector<long long> keys_to_array();
private:
    Node<T> *head = new Node<T>(0, nullptr);
    Node<T> *tail = new Node<T>(LONG_LONG_MAX, nullptr);
    std::shared_mutex m;
};

template<typename T>
bool LinkedList<T>::insert(const long long key, T *data) {
    // Throw error if same value found or something
    auto newNode = new Node<T>(key, data);
    m.lock();
    auto *cur = head;
    while (cur->next->key < key)
        cur = cur->next;

    newNode->next = cur->next;
    cur->next = newNode;
    m.unlock();
    return true;
}

template<typename T>
T* LinkedList<T>::remove(const long long key) {
    m.lock();
    auto prev = head;
    while (prev->next->key < key)
        prev = prev->next;

    if (prev->next->key == key) {
        auto temp = prev->next;
        prev->next = temp->next;
        m.unlock();
        auto val = temp->data;
        delete temp;
        return val;
    }
    m.unlock();
    return nullptr;
}

template<typename T>
T* LinkedList<T>::get(const long long key) {
    m.lock_shared();
    auto cur = head->next;
    while (cur->key < key)
        cur = cur->next;
    if (cur->key == key) {
        m.unlock_shared();
        return cur->data;
    }
    m.unlock_shared();
    return nullptr;
}

template<typename T>
LinkedList<T>::~LinkedList() {
    Node<T> *cur = head, *next = nullptr;
    while (cur != nullptr) {
        next = cur->next;
        delete cur;
        cur = next;
    }
}

template<typename T>
LinkedList<T>::LinkedList() {
    head->next = tail;
}

template<typename T>
void LinkedList<T>::printall() {
    m.lock_shared();
    auto cur = head;
    while (cur != nullptr) {
        std::cout << cur->key << ' ';
        cur = cur->next;
    }
    m.unlock_shared();
    std::cout << '\n';
}

template<typename T>
std::vector<T*> LinkedList<T>::to_array() {
    std::vector<T*> vec;
    m.lock_shared();
    auto cur = head->next;
    while (cur != nullptr) {
        vec.push_back(cur->data);
        cur = cur->next;
    }
    m.unlock_shared();
    vec.pop_back();
    return vec;
}

template<typename T>
std::vector<long long> LinkedList<T>::keys_to_array() {
    std::vector<long long> vec;
    m.lock_shared();
    auto cur = head->next;
    while (cur != nullptr) {
        vec.push_back(cur->key);
        cur = cur->next;
    }
    m.unlock_shared();
    vec.pop_back();
    return vec;
}

#endif //LINKEDLIST_H
