//
// Created by debianasus on 5/28/25.
//

#ifndef NONLOCKINGNonLockingLinkedList_H
#define NONLOCKINGNonLockingLinkedList_H
#include <atomic>
#include <vector>
#include <climits>


template<typename T>
struct Node {
    int key;
    T* data;
    Node* next;
    Node(const int key, T* data) : key(key), data(data), next(nullptr) {}
};

template<typename T>
class NonLockingLinkedList {
    // it's sorted
public:
    bool insert(int key, T* data);
    T* remove(int key);
    T* get(int key);
    ~NonLockingLinkedList();
    NonLockingLinkedList();
    void printall();
    std::vector<T*> to_array(); // DEBUG
    std::vector<int> keys_to_array(); // DEBUG
private:
    Node<T>* head = new Node<T>(0, nullptr);
    Node<T>* tail = new Node<T>(INT_MAX, nullptr);
};

template<typename T>
bool NonLockingLinkedList<T>::insert(const int key, T *data) {
    Node<T>* newNode = new Node<T>(key, data); // Create the new node
    Node<T>* pred, *curr;
    do {
        pred = head;
        // Atomically load the current node using std::atomic_ref.
        // std::memory_order_acquire ensures preceding memory operations are visible.
        curr = std::atomic_ref<Node<T>*>(pred->next).load(std::memory_order_acquire);

        // Traverse the list to find the correct insertion point.
        // Each access to 'next' must be done via std::atomic_ref for atomicity.
        while (curr != tail && curr->key < key) {
            pred = curr;
            // Load curr->next atomically before assigning to curr
            curr = std::atomic_ref<Node<T>*>(curr->next).load(std::memory_order_acquire);
        }

        // Check for duplicate key: if a node with the same key exists, insertion fails.
        if (curr != tail && curr->key == key) {
            delete newNode; // Clean up the newly created node if not inserted
            return false;
        }

        // Set new node's raw 'next' pointer to 'curr'. This is a non-atomic assignment.
        // It's safe here because 'newNode' is not yet published to other threads.
        newNode->next = curr;

        // Attempt to link 'pred->next' to 'newNode' using CAS.
        // 'curr' holds the value that 'pred->next' should currently be.
        // If CAS fails, 'curr' will be updated by compare_exchange_strong to the actual current value,
        // allowing the loop to retry with the correct state.

    } while (!std::atomic_ref<Node<T>*>(pred->next).compare_exchange_strong(curr, newNode));
}

template<typename T>
T* NonLockingLinkedList<T>::remove(const int key) {
    Node<T> *right_node, *right_node_next, *left_node;
    do {
        right_node = head->next;
        while (right_node->key < key) {
            left_node = right_node;
            right_node = right_node->next;
        }
        if (right_node == tail || right_node->key != key)
            return nullptr;
        right_node_next = right_node->next;
        
    }
}

// public boolean List::delete (KeyType search_key) {
// Node *right_node, *right_node_next, *left_node;
// do {
// right_node = search (search_key, &left_node);
// if ((right_node == tail) || (right_node.key != search_key)) /*T1*/
// return false;
// right_node_next = right_node.next;
// if (!is_marked_reference(right_node_next))
// if (CAS (&(right_node.next), /*C3*/
// right_node_next, get_marked_reference (right_node_next)))
// break;
// } while (true); /*B4*/
// if (!CAS (&(left_node.next), right_node, right_node_next)) /*C4*/
// right_node = search (right_node.key, &left_node);
// return true;
// }

template<typename T>
T* NonLockingLinkedList<T>::get(const int key) {
    auto cur = head->next;
    while (cur->key < key)
        cur = cur->next;
    if (cur->key == key)
        return cur->data;
    return nullptr;
}

template<typename T>
NonLockingLinkedList<T>::~NonLockingLinkedList() {
    Node<T>* cur = head, *next = nullptr;
    while (cur != nullptr) {
        next = cur->next;
        delete cur;
        cur = next;
    }
}

template<typename T>
NonLockingLinkedList<T>::NonLockingLinkedList() {
    head->next = tail;
}

template<typename T>
void NonLockingLinkedList<T>::printall() {
    auto cur = head;
    while (cur != nullptr) {
        std::cout << cur->key << ' ';
        cur = cur->next;
    }
    std::cout << '\n';
}

template<typename T>
std::vector<T*> NonLockingLinkedList<T>::to_array() {
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
std::vector<int> NonLockingLinkedList<T>::keys_to_array() {
    std::vector<int> vec;
    auto cur = head->next;
    while (cur != nullptr) {
        vec.push_back(cur->key);
        cur = cur->next;
    }
    vec.pop_back();
    return vec;
}

#endif //NONLOCKINGNonLockingLinkedList_H
