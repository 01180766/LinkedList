//
// Created by debianasus on 5/27/25.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <climits>
#include <iostream>
#include <vector>

template<typename T>
struct Node {
    int key;
    T* data;
    Node* next = nullptr;
    Node(const int key, T* data) : key(key), data(data) {}
};

template<typename T>
class LinkedList {
    // it's sorted
public:
    bool insert(int key, T* data);
    T* remove(int key);
    T* get(int key);
    ~LinkedList();
    LinkedList();
    void printall();
    std::vector<T*> to_array();
    std::vector<int> keys_to_array();
private:
    Node<T>* head = new Node<T>(0, nullptr);
    Node<T>* tail = new Node<T>(INT_MAX, nullptr);
};

template<typename T>
bool LinkedList<T>::insert(const int key, T *data) {
    // Throw error if same value found or something
    auto newNode = new Node<T>(key, data);
    auto *cur = head;
    while (cur->next->key < key) {
        cur = cur->next;
    }
    newNode->next = cur->next;
    cur->next = newNode;
    return true;
}

template<typename T>
T* LinkedList<T>::remove(const int key) {
    auto prev = head;
    while (prev->next->key < key) {
        prev = prev->next;
    }
    if (prev->next->key == key) {
        auto temp = prev->next;
        prev->next = temp->next;
        auto val = temp->data;
        delete temp;
        return val;
    }
    return nullptr;
}

template<typename T>
T* LinkedList<T>::get(const int key) {
    auto cur = head->next;
    while (cur->key < key)
        cur = cur->next;
    if (cur->key == key)
        return cur->data;
    return nullptr;
}

template<typename T>
LinkedList<T>::~LinkedList() {
    Node<T>* cur = head, *next = nullptr;
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
    auto cur = head;
    while (cur != nullptr) {
        std::cout << cur->key << ' ';
        cur = cur->next;
    }
    std::cout << '\n';
}

template<typename T>
std::vector<T*> LinkedList<T>::to_array() {
    std::vector<T*> vec;
    auto cur = head->next;
    while (cur != nullptr) {
        vec.push_back(cur->data);
        cur = cur->next;
    }
    vec.pop_back();
    return vec;
}

template<typename T>
std::vector<int> LinkedList<T>::keys_to_array() {
    std::vector<int> vec;
    auto cur = head->next;
    while (cur != nullptr) {
        vec.push_back(cur->key);
        cur = cur->next;
    }
    vec.pop_back();
    return vec;
}

#endif //LINKEDLIST_H
