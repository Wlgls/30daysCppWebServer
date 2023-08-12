#include "LogFile.h"
#include "TimeStamp.h"
#include <string>
#include <iostream>
LogFile::LogFile(const char* filepath)
    : fp_(::fopen(filepath, "a+")),
      written_bytes_(0),
      lastwrite_(0),
      lastflush_(0) {
    if (!fp_) {
        std::string DefaultPath = std::move("../LogFiles/LogFile_" +
                              TimeStamp::Now().TimeStamp::ToFormattedDefaultLogString() +
                              ".log");
        
        fp_ = ::fopen(DefaultPath.data(), "a+");
        
    }
}

LogFile::~LogFile() {
    Flush();
    if(!fp_){
        fclose(fp_);
    }
}

void LogFile::Write(const char* data, int len) {
    
    int pos = 0;
    while (pos != len) {
        // 使用无锁版本加快写入速度，一般一个系统只有一个后端日志系统。
        pos += static_cast<int>(fwrite_unlocked(data + pos, 
    										sizeof(char), len - pos, fp_));
    }
    time_t now = ::time(nullptr);
    // 更新当前状态
    if (len != 0) {
        lastwrite_ = now;
        written_bytes_ += len;
    }
    // 判断是否需要Flush
    if (lastwrite_ - lastflush_ > FlushInterval) {
        Flush();
        lastflush_ = now;
    }
}

int64_t LogFile::writtenbytes() const { return written_bytes_; }

void LogFile::Flush() { fflush(fp_); }