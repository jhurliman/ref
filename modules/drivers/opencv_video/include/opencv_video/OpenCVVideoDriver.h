#pragma once

#include <node/NodeBase.h>
#include <node/NodeInputs.h>
#include <node/NodeOutputs.h>
#include <node/Parameters.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <sensors/CameraInfo_generated.h>
#include <sensors/Image_generated.h>

namespace ref {

using namespace messages::sensors;

class OpenCVVideoDriverOutputs : public NodeOutputs {
public:
    PublishedMessage<ImageT> image_compressed;
    PublishedMessage<CameraInfoT> camera_info;

    OpenCVVideoDriverOutputs(const NodeDefinition::IDToTopicMap& idToTopicMap)
            : NodeOutputs(idToTopicMap)
            , image_compressed(NodeDefinition::LookupTopic(idToTopicMap, STR(image_compressed)))
            , camera_info(NodeDefinition::LookupTopic(idToTopicMap, STR(camera_info))) {}

    virtual void serialize() {
        serializeMessage<Image, ImageT>(image_compressed);
        serializeMessage<CameraInfo, CameraInfoT>(camera_info);
    }
};

class OpenCVVideoDriver : public NodeBase {
public:
    OpenCVVideoDriver(const NodeDefinition& def, const Graph& graph);

    NodeInputs* OpenCVVideoDriver::inputs() {
        return &_input;
    }

    NodeOutputs* OpenCVVideoDriver::outputs() {
        return &_output;
    }

    void tick() override;

private:
    static const int WIDTH = 640;
    static const int HEIGHT = 480;
    static const int FPS = 30;

    cv::VideoCapture _capture;
    NodeInputs _input;
    OpenCVVideoDriverOutputs _output;
};

}  // namespace ref
