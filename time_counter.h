/// 计时接口封装
/// @file
/// @date 2013-07-30 15:26:35
/// @version 1.0.0
/// @author baozhou(周龄), brk@syscall.cn
/// @namespace tree

#ifndef __TIME_COUNTER_H__
#define __TIME_COUNTER_H__ \
        "$Id$"

#include <sys/time.h>
#include <time.h>

namespace tree {

class TimeCounter
{
public:
    TimeCounter() { Reset(); }
    ~TimeCounter() { }

public:
    /// @return 逝去的时间，单位毫秒
    long Cost()
    {
        gettimeofday(&_time_end, NULL);
        return  (_time_end.tv_sec - _time_begin.tv_sec) * 1000
            + (_time_end.tv_usec - _time_begin.tv_usec) / 1000;
    }

    /// @brief 重置开始时间为当前时间
    void Reset()
    {
        gettimeofday(&_time_begin, NULL);
    }

private:
    struct timeval _time_begin;
    struct timeval _time_end;
};

}

#endif
