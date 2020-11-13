#include "TripleBuffer.hpp"
#include <thread>
#include <iostream>
#include <chrono>

#define MAX_ITERATION 100

void producer(std::shared_ptr<TripleBuffer<int>> tbi)
{   
    int* n;
    for(int i=0;i<MAX_ITERATION;i++)
    {
        n = tbi->producerGet();
        std::cout << "Building: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // time to build
        *n = i;
        tbi->producerGive(n);
    }
}

void consumer(std::shared_ptr<TripleBuffer<int>> tbi, int consumer_id, bool* run)
{
    int* n;
    while(*run)
    {
        n = tbi->consumerGet();
        if(n == nullptr)
        {
            std::cout << "Consumer " << consumer_id << " no elements left" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(5)); // idle waiting
            continue;
        }
        std::cout << "Consumer " << consumer_id << " using... " << *n << std::endl; // time to consume
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
        if (*n == MAX_ITERATION-1) *run = false;
        tbi->consumerGive(n);
    }
}



int main(int argc, char const *argv[])
{
    std::shared_ptr<TripleBuffer<int>> tbi = std::make_shared<TripleBuffer<int>>();
    bool consumers_run = true;

    auto t0 = std::chrono::high_resolution_clock::now();
    std::thread th1 (producer,tbi);
    std::thread th2 (consumer,tbi,1, &consumers_run);
    // std::thread th3 (consumer,tbi,2, &consumers_run);
    th1.join();
    th2.join();
    // th3.join();
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fs = t1-t0;
    std::cout << fs.count() << std::endl;
    return 0;
}



