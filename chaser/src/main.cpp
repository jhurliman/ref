#include <core/Assert.h>
#include <core/Log.h>
#include <messages/Recording.h>
#include <node/Graph.h>
#include <node/NodeDefinition.h>
#include <recorder/Recorder.h>
#include <webcam/WebcamDriver.h>

#include <iostream>
#include <thread>
#include <time.h>
#include <unistd.h>

static void loop() {
    using namespace std::chrono_literals;

    Json::Value config;
    std::ifstream configFile("chaser/config/chaser.jsonc", std::ifstream::binary);
    if (!configFile.is_open()) {
        ABORT_ALWAYS("Failed to open chaser/config/chaser.jsonc");
    }
    configFile >> config;

    ref::Graph g("deploy/bin/chaser/chaser.runfiles/ref_ws", config["nodes"]);

    g.writeDot(std::cout);

    ref::WebcamDriver webcam{g.nodes()[0], g};
    ref::Recorder recorder{g.nodes()[1], g};

    // FIXME: Replace all of this with a Controller
    auto start = ref::Time::NowHiRes();
    auto lastTick = start;
    do {
        auto now = ref::Time::NowHiRes();
        auto elapsed = now - lastTick;
        if (elapsed >= 10ms) {
            lastTick = now;
            elapsed = elapsed % 10ms;

            webcam.inputs()->setCurrentTime(ref::Time::Now());
            webcam.tick();

            recorder.inputs()->setCurrentTime(ref::Time::Now());
            recorder.tick();
        } else {
            ref::Time::Nanosleep(10ms - elapsed);
        }
    } while (lastTick - start < 3s);
}

int main(int, char const* []) {
    // ref::Time::Init(Time::FromSeconds(1));
    ref::Time::Init();

    // eigen_test();
    // usb_test();
    // jpeg_test();
    // libuvc_test();

    loop();

    return 0;
}
