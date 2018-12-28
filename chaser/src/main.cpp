#include <controller/Controller.h>
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

static std::unique_ptr<ref::NodeBase> CreateNode(const ref::NodeDefinition& def, const ref::Graph g) {
    if (def.nodeType() == "WebcamDriver") {
        return std::make_unique<ref::WebcamDriver>(def, g);
    } else if (def.nodeType() == "Recorder") {
        return std::make_unique<ref::Recorder>(def, g);
    } else {
        ABORT_ALWAYS(ref::Format("Unrecognized node type '%s'", def.nodeType()));
    }
}

static void Loop() {
    using namespace ref::filesystem;
    using namespace std::chrono_literals;

    std::string baseDir = JoinPath(ApplicationDir(), "chaser.runfiles/ref_ws");
    std::string configPath = JoinPath(baseDir, "chaser/config/chaser.jsonc");

    Json::Value config;
    std::ifstream configFile(configPath, std::ifstream::binary);
    if (!configFile.is_open()) {
        ABORT_ALWAYS(ref::Format("Failed to open %s", configPath));
    }
    configFile >> config;

    ref::Graph g(baseDir, config["nodes"]);

    g.writeDot(std::cout);

    ref::WebcamDriver webcam{g.nodes()[0], g};
    ref::Recorder recorder{g.nodes()[1], g};

    ref::Controller controller{g, CreateNode};

    auto start = ref::Time::Now();
    auto lastTick = start;
    std::vector<ref::NodeBase*> ready;
    ref::Time::TimePoint tickDeadline;

    do {
        auto now = ref::Time::Now();

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
    } while (lastTick - start < 3s);
}

int main(int, char const* []) {
    ref::Time::Init();
    Loop();
    return 0;
}
