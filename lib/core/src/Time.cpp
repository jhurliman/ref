#include "core/Time.h"

#include <mutex>
#include <shared_mutex>

namespace ref {

namespace Time {

static const TimePoint EPOCH;
static bool _HasInitialized = false;
static TimePoint _SimTime;

static std::shared_mutex& GetTimeMutex() {
    static std::shared_mutex timeMutex;
    return timeMutex;
}

void Init(Optional<TimePoint> simTime) {
    std::unique_lock<std::shared_mutex> lock(GetTimeMutex());
    if (simTime) {
        _SimTime = *simTime;
    }
    _HasInitialized = true;
}

bool IsValid() {
    std::shared_lock<std::shared_mutex> lock(GetTimeMutex());
    return _HasInitialized;
}

TimePoint NowWall() {
    return std::chrono::system_clock::now();
}

HiResTimePoint NowHiRes() {
    return std::chrono::high_resolution_clock::now();
}

timespec ToTimespec(uint64_t ns) {
    constexpr uint64_t NSEC_PER_SEC = 1000000000;

    timespec t;

    // Roll nanoseconds over into seconds
    if (ns >= NSEC_PER_SEC) {
        uint64_t seconds = ns / NSEC_PER_SEC;
        ns -= seconds * NSEC_PER_SEC;
        t.tv_sec = long(seconds);
    } else {
        t.tv_sec = 0;
    }

    t.tv_nsec = long(ns);
    return t;
}

timespec NowMonotonicTimespec(int64_t nsecOffset) {
    constexpr long NSEC_PER_SEC = 1000000000;

    timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    t.tv_nsec += nsecOffset;

    // Roll nanoseconds over into seconds
    if (t.tv_nsec >= NSEC_PER_SEC) {
        long seconds = t.tv_nsec / NSEC_PER_SEC;
        t.tv_nsec -= seconds * NSEC_PER_SEC;
        t.tv_sec += seconds;
    }

    return t;
}

TimePoint Now() {
    std::shared_lock<std::shared_mutex> lock(GetTimeMutex());
    return (!_HasInitialized || _SimTime != EPOCH) ? _SimTime : NowWall();
}

void SetNow(const TimePoint currentTime) {
    std::unique_lock<std::shared_mutex> lock(GetTimeMutex());
    _SimTime = currentTime;
}

uint64_t ToNanoseconds(const TimePoint time) {
    auto ns = time.time_since_epoch();
    return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(ns).count());
}

double ToSeconds(const TimePoint time) {
    return double(ToNanoseconds(time)) / 10.0e9;
}

TimePoint FromNanoseconds(const uint64_t nsec) {
    std::chrono::nanoseconds sinceEpoch(nsec);
    return TimePoint(std::chrono::duration_cast<TimePoint::duration>(sinceEpoch));
}

TimePoint FromSeconds(const double sec) {
    std::chrono::duration<double, std::ratio<1>> sinceEpoch(sec);
    return TimePoint(std::chrono::duration_cast<TimePoint::duration>(sinceEpoch));
}

TimePoint FromTimeval(const timeval time) {
    constexpr uint64_t NS_PER_SEC = 10e9;   // nanoseconds per second
    constexpr uint64_t NS_PER_USEC = 1000;  // nanoseconds per microsecond
    return FromNanoseconds(
            uint64_t(time.tv_sec) * NS_PER_SEC + uint64_t(time.tv_usec) * NS_PER_USEC);
}

double DeltaMS(const TimePoint from, const TimePoint to) {
    using ms = std::chrono::duration<double, std::milli>;
    auto delta = std::chrono::duration_cast<ms>(to - from);
    return delta.count();
}

}  // namespace Time

}  // namespace ref
