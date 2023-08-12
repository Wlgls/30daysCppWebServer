#pragma once
#include "common.h"
#include <thread>
#include <condition_variable>
class Latch
{
private:
    std::mutex mux_;
    std::condition_variable cv_;
    int count_;

public:
    DISALLOW_COPY_AND_MOVE(Latch);

    explicit Latch(int count) : count_(count){}
    void wait(){
        std::unique_lock<std::mutex> lock(mux_);
        while(count_ > 0){
            cv_.wait(lock);
        }
    }

    void notify(){
        std::unique_lock<std::mutex> lock(mux_);
        --count_;
        if(count_ == 0){
            cv_.notify_all();
        }
    }
};