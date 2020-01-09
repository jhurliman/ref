#include <controller/Controller.h>
#include <core/Application.h>
#include <opencv_video/OpenCVVideoDriver.h>
#include <recorder/Recorder.h>
#include <webcam/WebcamDriver.h>

int main(int, char const*[]) {
    // Initialize the graph definition
    ref::Graph graph;
    if (auto res = graph.initializeFromAppConfig("camera_record"); !res.isOk()) {
        throw res.error();
    }

    // Initialize time
    ref::Time::Init();

    // Define the mapping from node types to methods that instantiate a node
    std::unordered_map<std::string, ref::Controller::NodeFactoryFn> nodeFactory = {
            {"OpenCVVideoDriver", NODE_FACTORY(ref::OpenCVVideoDriver)},
            {"Recorder", NODE_FACTORY(ref::Recorder)},
            {"WebcamDriver", NODE_FACTORY(ref::WebcamDriver)}};

    // Initialize the execution controller
    ref::Controller controller;
    if (auto res = controller.initialize(graph, nodeFactory); !res.isOk()) {
        throw res.error();
    }

    // Start the execution loop
    while (ref::Running()) {
        controller.tick();
    }
}
