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

    ref::Time::HiResTimePoint start = ref::Time::NowHiRes();
    ref::Time::HiResTimePoint lastTick = start;
    do {
        auto nowHiRes = ref::Time::NowHiRes();
        auto now = ref::Time::Now();
        auto elapsed = nowHiRes - lastTick;
        if (elapsed >= 10ms) {
            lastTick = nowHiRes;
            elapsed = elapsed % 10ms;

            ref::NodeMessages nullIn{now, {}};
            ref::WebcamDriverOutput camOutput{now, webcam.definition().outputs()};
            webcam.tick(nullIn, &camOutput);

            ref::NodeMessages nullOut{now, recorder.definition().outputs()};
            recorder.tick(camOutput, &nullOut);
        }

        auto deadline = ref::Time::NowMonotonicTimespec((10ms - elapsed).count());
        ref::Time::NanosleepAbstime(&deadline);
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
