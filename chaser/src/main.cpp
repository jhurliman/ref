#include <core/Assert.h>
#include <core/FileUtils.h>
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

    std::string baseDir = ref::JoinPath(ref::ApplicationDir(), "chaser.runfiles/ref_ws");
    std::string configPath = ref::JoinPath(baseDir, "chaser/config/chaser.jsonc");

    Json::Value config;
    std::ifstream configFile(configPath, std::ifstream::binary);
    if (!configFile.is_open()) {
        ABORT_ALWAYS(ref::Format("Failed to open %s", configPath).c_str());
    }
    configFile >> config;

    ref::Graph g(baseDir, config["nodes"]);

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
    ref::Time::Init();
    loop();
    return 0;
}
