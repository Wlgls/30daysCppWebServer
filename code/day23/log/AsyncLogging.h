#pragma once

#include <vector>
#include <memory>

#include <thread>
#include "Latch.h"
#include "Logging.h"

static const double BufferWriteTimeout = 3.0; // 等待写入的时间
static const int64_t FileMaximumSize = 1024 * 1024 * 1024; // 单个文件最大的容量
class AsyncLogging{
    public:
        typedef FixedBuffer<FixedLargeBuffferSize> Buffer;

        AsyncLogging(const char *filepath = nullptr);
        ~AsyncLogging();

        void Stop();
        void Start();

        void Append(const char* data, int len);
        void Flush();
        void ThreadFunc();
    
    private:
        
        bool running_;
        const char *filepath_;
        std::mutex mutex_;
        std::condition_variable cv_;
        Latch latch_;
        std::thread thread_;

        std::unique_ptr<Buffer> current_; // 当前的缓存
        std::unique_ptr<Buffer> next_; // 空闲的缓冲
        std::vector<std::unique_ptr<Buffer>> buffers_;// 已满的缓冲区
};