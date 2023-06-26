
#include "currentthread.h"
#include <sys/syscall.h>
#include <stdio.h>
#include <unistd.h>
namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_formattedTid[32];
    __thread int t_formattedTidLength;

    pid_t gettid() {
        return static_cast<int>(syscall(SYS_gettid));
    }
    void CacheTid(){
        if (t_cachedTid == 0){
            t_cachedTid = gettid();
            t_formattedTidLength = snprintf(t_formattedTid, sizeof(t_formattedTid), "%5d ", t_cachedTid);
        }
    }
}
    


/*
inline int CurrentThread::tid(){
    if(__builtin_expect(t_cachedTid == 0, 0)){
        CacheTid();
    }
    return t_cachedTid;
}
*/



