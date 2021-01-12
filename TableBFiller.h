//
// Created by Blink on 2020/10/6.
//

#ifndef PLOT_OPENCL_TABLEBFILLER_H
#define PLOT_OPENCL_TABLEBFILLER_H

#include <atomic>
#include <thread>
#include <vector>
#include "MsgQue.hpp"
class TableBFiller {
public:
    explicit TableBFiller(int consumer_num_, int threads_per_consumer_, uint64_t *B_);

    void set_terminate();

    void add_task(std::pair<unsigned int *, unsigned int *> task);

    void consume_task();

    void start();

    ~TableBFiller();

private:
    int consumer_num;
    int threads_per_consumer;
    std::atomic_flag lock[4096] = {ATOMIC_FLAG_INIT};
    std::vector<std::thread *> threads;
    uint64_t *B;
    MsgQueue<std::pair<unsigned int *, unsigned int *> > task_q;
};


#endif //PLOT_OPENCL_TABLEBFILLER_H
