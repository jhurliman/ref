#include <controller/Controller.h>
#include <core/Application.h>
#include <recorder/Recorder.h>
#include <webcam/WebcamDriver.h>

int main(int, char const* []) {
    using namespace ref::filesystem;

    std::string baseDir = JoinPath(ApplicationDir(), "camera_record.runfiles/ref_ws");
    std::string configPath = JoinPath(baseDir, "camera_record/config/camera_record.jsonc");

    auto jsonRes = ref::ParseJSONFile(configPath);
    if (!jsonRes.isOk()) {
        throw jsonRes.error();
    }

    ref::Graph g(baseDir, jsonRes.value()["nodes"]);

    ref::Time::Init();

    std::unordered_map<std::string, ref::Controller::NodeFactoryFn> nodeFactory = {
        {"WebcamDriver", NODE_FACTORY(ref::WebcamDriver) },
        {"Recorder", NODE_FACTORY(ref::Recorder) }
    };

    ref::Controller controller;
    auto res = controller.initialize(g, nodeFactory);
    if (!res.isOk()) {
        throw res.error();
    }

    while (ref::Running()) {
        controller.tick();
    }

    return 0;
}
