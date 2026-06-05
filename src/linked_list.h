#ifndef LOCK_FREE_LINKED_LIST_H
#define LOCK_FREE_LINKED_LIST_H

#include <atomic>
#include <limits>
#include <cstdint>
#include <iostream>

template<typename T>
class LockFreeLinkedList {
private:
    struct Node {
        T value;
        std::atomic<Node*> next;
        Node(T val) : value(val), next(nullptr) {}
    };

    Node* head;

    static Node* getMarked(Node* ptr) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(ptr) | 0x1);
    }
    static Node* getUnmarked(Node* ptr) {
        return reinterpret_cast<Node*>(reinterpret_cast<uintptr_t>(ptr) & ~uintptr_t(0x1));
    }
    static bool isMarked(Node* ptr) {
        return (reinterpret_cast<uintptr_t>(ptr) & 0x1) != 0;
    }

public:
    LockFreeLinkedList() {
        head = new Node(std::numeric_limits<T>::min());
        Node* tail = new Node(std::numeric_limits<T>::max());
        head->next.store(tail);
    }

    ~LockFreeLinkedList() {
        Node* curr = head;
        while (curr != nullptr) {
            Node* next = getUnmarked(curr->next.load());
            delete curr;
            curr = next;
        }
    }

    bool contains(const T& value) {
        Node* curr = head;
        while (curr->value < value) {
            Node* next = getUnmarked(curr->next.load());
            curr = next;
        }
        return (curr->value == value && !isMarked(curr->next.load()));
    }

    bool insert(const T& value) {
        while (true) {
            Node *pred, *curr;
            if (find(value, pred, curr)) {
                return false;
            }
            Node* newNode = new Node(value);
            newNode->next.store(curr);
            if (pred->next.compare_exchange_strong(curr, newNode)) {
                return true;
            }
            delete newNode;
        }
    }

    bool remove(const T& value) {
        Node *pred, *curr;
        while (true) {
            if (!find(value, pred, curr)) {
                return false;
            }
            Node* succ = curr->next.load();
            if (!curr->next.compare_exchange_strong(succ, getMarked(succ))) {
                continue;
            }
            if (pred->next.compare_exchange_strong(curr, succ)) {
                delete curr;
            }
            return true;
        }
    }

    int size() {
        int count = 0;
        Node* curr = getUnmarked(head->next.load());
        while (curr && curr->value != std::numeric_limits<T>::max()) {
            if (!isMarked(curr->next.load())) {
                count++;
            }
            curr = getUnmarked(curr->next.load());
        }
        return count;
    }

    bool validate() {
        Node* curr = head;
        T prevVal = curr->value;
        curr = getUnmarked(curr->next.load());
        while (curr && curr->value != std::numeric_limits<T>::max()) {
            if (isMarked(curr->next.load())) {
                return false;
            }
            if (curr->value <= prevVal) {
                return false;
            }
            prevVal = curr->value;
            curr = getUnmarked(curr->next.load());
        }
        return true;
    }

private:
    bool find(const T& value, Node*& pred, Node*& curr) {
        while (true) {
            pred = head;
            curr = getUnmarked(pred->next.load());
            while (true) {
                Node* succ = curr->next.load();
                while (isMarked(succ)) {
                    if (!pred->next.compare_exchange_strong(curr, getUnmarked(succ))) {
                        goto retry;
                    }
                    curr = getUnmarked(pred->next.load());
                    succ = curr->next.load();
                }
                if (curr->value >= value) {
                    return (curr->value == value);
                }
                pred = curr;
                curr = getUnmarked(curr->next.load());
            }
        retry:;
        }
        return false;
    }
};

#endif
