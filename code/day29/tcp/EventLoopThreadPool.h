#pragma once

#include "common.h"

#include <memory>
#include <thread>
#include <vector>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool{

    public:
        DISALLOW_COPY_AND_MOVE(EventLoopThreadPool);
        EventLoopThreadPool(EventLoop *loop);
        ~EventLoopThreadPool();

        void SetThreadNums(int thread_nums);

        void start();

        // 获取线程池中的EventLoop
        EventLoop *nextloop();

    private:
        EventLoop *main_reactor_;
        std::vector<std::unique_ptr<EventLoopThread>> threads_;

        std::vector<EventLoop *> loops_;

        int thread_nums_;

        int next_;
};