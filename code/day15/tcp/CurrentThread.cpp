
#include "CurrentThread.h"
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_formattedTid[32];
    __thread int t_formattedTidLength;

    pid_t gettid() {
        // 不使用std::this_thread::get_id()，它好像并不是唯一的
        return static_cast<int>(syscall(SYS_gettid));
    }
    void CacheTid(){
        if (t_cachedTid == 0){
            t_cachedTid = gettid();
            t_formattedTidLength = snprintf(t_formattedTid, sizeof(t_formattedTid), "%5d ", t_cachedTid);
        }
    }
}
    
