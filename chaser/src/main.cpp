#include <core/Assert.h>
#include <core/Log.h>
#include <messages/Recording.h>
#include <node/NodeDefinition.h>
#include <recorder/Recorder.h>
#include <webcam/WebcamDriver.h>

#include <iostream>
#include <thread>
#include <time.h>
#include <unistd.h>

void loop() {
    using namespace std::chrono_literals;

    Json::Value config;
    std::ifstream configFile("chaser/config/chaser.jsonc", std::ifstream::binary);
    if (!configFile.is_open()) {
        ABORT_ALWAYS("Failed to open chaser/config/chaser.jsonc");
    }
    configFile >> config;

    auto nodeDef1 = ref::NodeDefinition::Create(config["nodes"][0]);
    ASSERT_ALWAYS(nodeDef1, "Failed to parse WebcamDriver node definition");
    ref::WebcamDriver webcam{*nodeDef1};

    auto nodeDef2 = ref::NodeDefinition::Create(config["nodes"][1]);
    ASSERT_ALWAYS(nodeDef2, "Failed to parse Recorder node definition");
    ref::Recorder recorder{*nodeDef2};

    ref::Time::HiResTimePoint start = ref::Time::NowHiRes();
    ref::Time::HiResTimePoint lastTick = start;
    ref::Time::HiResTimePoint now;
    do {
        now = ref::Time::NowHiRes();
        auto elapsed = now - lastTick;
        if (elapsed >= 10ms) {
            lastTick = now;
            elapsed = elapsed % 10ms;

            auto now = ref::Time::Now();

            ref::NodeMessages input{now, {}};
            ref::WebcamDriverOutput camOutput{now, webcam.definition().outputs()};
            webcam.tick(input, &camOutput);

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
