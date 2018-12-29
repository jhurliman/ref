#include <controller/Controller.h>
#include <core/Assert.h>
#include <core/FileUtils.h>
#include <core/Json.h>
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

static void Loop() {
    using namespace ref::filesystem;
    using namespace std::chrono_literals;

    std::string baseDir = JoinPath(ApplicationDir(), "chaser.runfiles/ref_ws");
    std::string configPath = JoinPath(baseDir, "chaser/config/chaser.jsonc");

    auto jsonRes = ref::ParseJSONFile(configPath);
    if (!jsonRes.isOk()) {
        throw jsonRes.error();
    }

    ref::Graph g(baseDir, jsonRes.value()["nodes"]);
    g.writeDot(std::cout);

    std::unordered_map<std::string, ref::Controller::NodeFactoryFn> nodeFactory = {
        {"WebcamDriver", NODE_FACTORY(ref::WebcamDriver) },
        {"Recorder", NODE_FACTORY(ref::Recorder) }
    };

    ref::Controller controller;
    auto res = controller.initialize(g, nodeFactory);
    if (!res.isOk()) {
        throw res.error();
    }

    ref::Time::TimePoint start = ref::Time::Now();
    ref::Time::TimePoint now;
    std::vector<ref::NodeBase*> ready;
    ref::Time::TimePoint tickDeadline;

    do {
        now = ref::Time::Now();

        controller.readyNodes(now, &ready, &tickDeadline);

        if (ready.size()) {
            controller.tickNodes(now, ready);
        } else {
            // Find the delay in milliseconds between now and the next deadline
            double delayMS = ref::Time::DeltaMS(now, tickDeadline);
            // Clamp the delay to [0-1] milliseconds
            delayMS = std::max(0.0, std::min(1.0, delayMS));
            // Sleep for this amount of time
            ref::Time::Nanosleep(uint64_t(delayMS * 1e6));
        }
    } while (now - start < 3s);
}

int main(int, char const* []) {
    ref::Time::Init();
    Loop();
    return 0;
}
