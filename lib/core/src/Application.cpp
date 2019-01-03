#include "core/Application.h"

#include <mutex>
#include <signal.h>

namespace ref {

static bool gShutdown = false;

static void SigintHandler(int sig) {
    assert(sig == SIGINT);
    Shutdown();
}

bool Running() {
    static std::once_flag flag;
    std::call_once(flag, []() { signal(SIGINT, SigintHandler); });

    return !gShutdown;
}

void Shutdown() {
    gShutdown = true;
}

}  // namespace ref
