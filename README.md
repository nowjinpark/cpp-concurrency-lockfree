# C++ Concurrency and Lock-Free Data Structures

Course projects by **Jin Park / Jinwon Park**.

This repository contains selected C++ concurrency work from CS355, including pthread synchronization, semaphore implementation, parallel quicksort, a lock-free sorted vector exercise, and a lock-free linked list final-project contribution.

## What I Built

- Parallel Game of Life using pthread worker threads.
- Custom barrier using mutex and condition variable.
- Counting semaphore using `std::mutex` and `std::condition_variable`.
- Lock-free sorted vector update flow using atomic compare-and-swap.
- Parallel quicksort with worker threads, a thread-safe task stack, and atomic task tracking.
- Lock-free linked list using CAS, marked pointers, logical deletion, and physical deletion.

## Highlighted Systems

### Lock-Free Linked List

- Maintains sorted order with sentinel head/tail nodes.
- Uses marked pointers to represent logically deleted nodes.
- Uses compare-and-swap to insert and remove without traditional locks.
- Includes `contains`, `insert`, `remove`, `size`, and `validate` behavior.

### Parallel Quicksort

- Uses multiple worker threads.
- Stores partition tasks in a synchronized stack.
- Tracks active tasks with an atomic counter.
- Uses small-array sorting helpers for base cases.

## Tech Stack

- C++
- pthread
- `std::thread`
- `std::atomic`
- `std::mutex`
- `std::condition_variable`
- Lock-free programming

## Notes

The final project was a team project; this repository focuses on the lock-free linked list and related concurrency source relevant to my contribution.

