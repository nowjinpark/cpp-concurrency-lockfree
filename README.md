# C++ Concurrency and Lock-Free Data Structures

Course projects by **Jin Park / Jinwon Park**.

This repository contains selected C++ concurrency work from CS355, including pthread synchronization, semaphore implementation, parallel quicksort, a lock-free sorted vector exercise, and a lock-free linked list final-project contribution.

## 한국어 소개

이 저장소는 CS355에서 진행한 C++ concurrency 및 lock-free data structure 관련 작업을 정리한 포트폴리오용 공개본입니다. pthread 동기화, custom semaphore, parallel quicksort, atomic CAS 기반 자료구조 구현을 포함합니다.

### 제가 구현한 주요 기능

- pthread worker thread를 사용한 Parallel Game of Life 구현.
- mutex와 condition variable을 이용한 custom barrier 구현.
- `std::mutex`, `std::condition_variable` 기반 counting semaphore 구현.
- atomic compare-and-swap을 활용한 lock-free sorted vector update 흐름 작성.
- worker thread, thread-safe task stack, atomic task tracking을 사용한 parallel quicksort 구현.
- marked pointer, logical deletion, physical deletion, CAS를 사용하는 lock-free linked list 구현.

### 기술적으로 보여주는 부분

- thread synchronization primitive를 직접 구현하고 사용하는 경험.
- `std::atomic`과 CAS 기반 lock-free programming 이해.
- 병렬 작업 분할, task tracking, worker coordination 설계.
- 팀 final project에서 lock-free linked list 관련 핵심 자료구조 구현에 기여한 내용.

### 공개 범위

final project는 팀 프로젝트였기 때문에, 이 저장소는 제 기여와 관련된 lock-free linked list 및 concurrency source 중심으로 정리했습니다.

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
