//
// Created by Blink on 2020/10/6.
//

#ifndef PLOT_OPENCL_THREAD_POOL_H
#define PLOT_OPENCL_THREAD_POOL_H

#include <atomic>
#include <thread>
#include <vector>
#include "MsgQue.hpp"
class thread_pool {
public:
    thread_pool(int consumer_num_, int threads_per_consumer_, uint64_t *B_): consumer_num(consumer_num_),
                threads_per_consumer(threads_per_consumer_), B(B_) {
        //for (int i = 0; i < 64; i++) {
        //    lock[i].clear(std::memory_order_release);
        //}
    }

    void set_terminate() {
        for (int i = 0; i < threads.size(); i++) {
            task_q.push(std::make_pair(nullptr, nullptr));
        }
    }
    void add_task(std::pair<unsigned int *, unsigned int *> task) {
        task_q.push(task);
    }
    void consume_task() {
        std::pair<unsigned int *, unsigned int *> task;
        while (true) {
            task_q.pop(task);
            unsigned int *A = task.first, *B_rev = task.second;
            if (A == nullptr || B_rev == nullptr) {
                break;
            }
            int thread_num = threads_per_consumer;
            std::vector <std::thread *> thread_p;
            auto w_f = [&](unsigned int offset, unsigned int length, unsigned int *A, unsigned int *B_rev, uint64_t *B) {
                unsigned int end = offset + length;
                unsigned int block_size = (1 << 26);
                uint64_t x, xp;
                for (unsigned int i = offset; i < end; i += 2) {
                    x = A[i];
                    xp = A[i + 1];
                    if (x == 0 || xp == 0) {
                        continue;
                    }
                    unsigned int block = B_rev[i] / block_size;
                    while (lock[block].test_and_set(std::memory_order_acquire));
                    B[B_rev[i]] = (x << 32U) | xp;
                    lock[block].clear(std::memory_order_release);
                    block = B_rev[i + 1] / block_size;
                    while (lock[block].test_and_set(std::memory_order_acquire));
                    B[B_rev[i + 1]] = (xp << 32U) | x;
                    lock[block].clear(std::memory_order_release);
                }
            };
            for (int i = 0; i < thread_num; i++) {
                unsigned int length = ((1 << 29) / thread_num);
                unsigned int offset = i * length;
                thread_p.emplace_back(new std::thread(w_f, offset, length, A, B_rev, B));
            }
            for (int i = 0; i < thread_num; i++) {
                thread_p[i]->join();
                delete thread_p[i];
            }
            delete[] A;
            delete[] B_rev;
        }
    }
    void start() {
        for (int i = 0; i < consumer_num; i++) {
            threads.emplace_back(new std::thread(&thread_pool::consume_task, this));
        }
    }
    ~thread_pool() {
        for (auto &&t : threads) {
            t->join();
            delete t;
        }

    }
private:
    int consumer_num;
    int threads_per_consumer;
    std::atomic_flag lock[64] = {ATOMIC_FLAG_INIT};
    std::mutex mtx[64];
    std::vector<std::thread *> threads;
    uint64_t *B;
    MsgQueue<std::pair<unsigned int *, unsigned int *> > task_q;
};


#endif //PLOT_OPENCL_THREAD_POOL_H
