#pragma once

#include "Optional.h"

#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <time.h>
#include <unistd.h>

namespace ref {

namespace Time {

using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;
using HiResTimePoint = std::chrono::high_resolution_clock::time_point;

void Init(Optional<TimePoint> simTime = Nullopt);

bool IsValid();

TimePoint NowWall();

HiResTimePoint NowHiRes();

timespec NowMonotonicTimespec(uint64_t nsecOffset);

TimePoint Now();

void SetNow(const TimePoint currentTime);

uint64_t ToNanoseconds(const TimePoint time);

double ToSeconds(const TimePoint time);

TimePoint FromNanoseconds(const uint64_t nsec);

TimePoint FromSeconds(const double sec);

TimePoint FromTimeval(const timeval time);

double DeltaMS(const TimePoint from, const TimePoint to);

#ifdef __MACH__

inline void timespec_monodiff_rml(struct timespec* ts_out, const struct timespec* ts_in) {
    /* out = in - out,
       where in > out
     */

    constexpr long TIMING_GIGA = 1000000000;

    ts_out->tv_sec = ts_in->tv_sec - ts_out->tv_sec;
    ts_out->tv_nsec = ts_in->tv_nsec - ts_out->tv_nsec;
    if (ts_out->tv_sec < 0) {
        ts_out->tv_sec = 0;
        ts_out->tv_nsec = 0;
    } else if (ts_out->tv_nsec < 0) {
        if (ts_out->tv_sec == 0) {
            ts_out->tv_sec = 0;
            ts_out->tv_nsec = 0;
        } else {
            ts_out->tv_sec = ts_out->tv_sec - 1;
            ts_out->tv_nsec = ts_out->tv_nsec + TIMING_GIGA;
        }
    } else {
    }
}

inline int NanosleepAbstime(const struct timespec* req) {
    struct timespec ts_delta;
    int retval = clock_gettime(CLOCK_MONOTONIC, &ts_delta);
    if (retval == 0) {
        timespec_monodiff_rml(&ts_delta, req);
        retval = nanosleep(&ts_delta, NULL);
    }
    return retval;
}

#else

inline int NanosleepAbstime(const struct timespec* req) {
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, req, nullptr);
}

#endif

}  // namespace Time

}  // namespace ref
