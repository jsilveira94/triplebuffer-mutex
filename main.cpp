#include "TripleBuffer.hpp"
#include <thread>
#include <iostream>
#include <chrono>

#define MAX_ITERATION 100

void producer(std::shared_ptr<TripleBuffer<int>> tbi)
{   
    void * n;
    for(int i=0;i<MAX_ITERATION;i++)
    {
        n = tbi->producerGet();
        std::cout << "Building: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
        *(int *)n = i;
        tbi->producerGive();
    }
}

void consumer(std::shared_ptr<TripleBuffer<int>> tbi)
{
    void* n;
    while(true)
    {
        n = tbi->consumerGet();
        if(n == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
            continue;
        }
        std::cout << "Using... " << *(int *)n << std::endl;
        if (*(int *)n == MAX_ITERATION-1) break;
        tbi->consumerGive();
    }
}



int main(int argc, char const *argv[])
{
    std::shared_ptr<TripleBuffer<int>> tbi = std::make_shared<TripleBuffer<int>>();

    auto t0 = std::chrono::high_resolution_clock::now();
    std::thread th2 (consumer,tbi);
    std::thread th1 (producer,tbi);
    th1.join();
    th2.join();
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> fs = t1-t0;
    std::cout << fs.count() << std::endl;
    return 0;
}



