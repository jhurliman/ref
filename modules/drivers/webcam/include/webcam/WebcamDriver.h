#pragma once

#include <core/Format.h>
#include <libuvc/libuvc.h>
#include <memory>
#include <mutex>
#include <node/NodeBase.h>
#include <node/NodeInputs.h>
#include <node/NodeOutputs.h>
#include <node/Parameters.h>
#include <sensors/CameraInfo_generated.h>
#include <sensors/Image_generated.h>

namespace ref {

using namespace messages::sensors;

class WebcamDriverOutputs : public NodeOutputs {
public:
    PublishedMessage<ImageT> image_compressed;
    PublishedMessage<CameraInfoT> camera_info;

    WebcamDriverOutputs(const NodeDefinition::IDToTopicMap& idToTopicMap)
            : NodeOutputs(idToTopicMap)
            , image_compressed(NodeDefinition::LookupTopic(idToTopicMap, STR(image_compressed)))
            , camera_info(NodeDefinition::LookupTopic(idToTopicMap, STR(camera_info))) {}

    virtual void serialize() {
        serializeMessage<Image, ImageT>(image_compressed);
        serializeMessage<CameraInfo, CameraInfoT>(camera_info);
    }
};

class WebcamDriver : public NodeBase {
public:
    WebcamDriver(const NodeDefinition& def, const Graph& graph);

    NodeInputs* inputs() override;
    NodeOutputs* outputs() override;
    void tick() override;

    void callback(uvc_frame_t* frame);

private:
    static const int WIDTH = 640;
    static const int HEIGHT = 480;
    static const int FPS = 30;

    uvc_context_t* context = nullptr;
    uvc_device_t* device = nullptr;
    uvc_device_handle_t* deviceHandle = nullptr;
    uvc_stream_ctrl_t control;

    std::unique_ptr<messages::sensors::ImageT> latestImage;
    std::mutex latestImageMutex;

    NodeInputs _input;
    WebcamDriverOutputs _output;
};

}  // namespace ref
