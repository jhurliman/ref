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

extern TimePoint DISTANT_FUTURE;

void Init(Optional<TimePoint> simTime = Nullopt);

bool IsValid();

TimePoint NowWall();

timespec ToTimespec(uint64_t ns);

timespec NowMonotonicTimespec(int64_t nsecOffset);

TimePoint Now();

void SetNow(const TimePoint currentTime);

uint64_t ToNanoseconds(const TimePoint time);

double ToSeconds(const TimePoint time);

double ToSeconds(const std::chrono::nanoseconds ns);

std::string ToString(const TimePoint time);

TimePoint FromNanoseconds(const uint64_t nsec);

TimePoint FromSeconds(const double sec);

TimePoint FromTimeval(const timeval time);

TimePoint AddSeconds(const TimePoint time, const double sec);

TimePoint Min(const TimePoint a, const TimePoint b);

TimePoint Max(const TimePoint a, const TimePoint b);

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
    }
}

inline int Nanosleep(uint64_t ns) {
    timespec ts = ToTimespec(ns);
    return nanosleep(&ts, nullptr);
}

inline int NanosleepAbstime(const struct timespec* ts) {
    timespec tsDelta;
    int retval = clock_gettime(CLOCK_MONOTONIC, &tsDelta);
    if (retval == 0) {
        timespec_monodiff_rml(&tsDelta, ts);
        retval = nanosleep(&tsDelta, NULL);
    }
    return retval;
}

#else

inline int Nanosleep(uint64_t ns) {
    timespec ts = ToTimespec(ns);
    return clock_nanosleep(CLOCK_MONOTONIC, 0, ts, nullptr);
}

inline int NanosleepAbstime(const struct timespec* req) {
    return clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, req, nullptr);
}

#endif

inline int Nanosleep(std::chrono::nanoseconds ns) {
    return Nanosleep(ns.count());
}

}  // namespace Time

}  // namespace ref
