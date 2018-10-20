#pragma once

#include <libuvc/libuvc.h>
#include <memory>
#include <mutex>
#include <node/NodeBase.h>
#include <node/Parameters.h>
#include <sensors/CameraInfo_generated.h>
#include <sensors/Image_generated.h>

namespace ref {

struct WebcamDriverOutput : NodeMessages {
    using NodeMessages::NodeMessages;

    std::shared_ptr<messages::sensors::ImageT> image_compressed;
    std::shared_ptr<messages::sensors::CameraInfoT> camera_info;

    virtual void record(Recording& recording) const {
        using namespace messages::sensors;
        recordMessage<Image, ImageT>(STRING(image_compressed), image_compressed.get(), recording);
        recordMessage<CameraInfo, CameraInfoT>(STRING(camera_info), camera_info.get(), recording);
    }
};

class WebcamDriver : public NodeBase {
public:
    WebcamDriver(const NodeDefinition& def);

    void tick(const NodeMessages& input, WebcamDriverOutput* output);
    void callback(uvc_frame_t* frame);

private:
    static const int WIDTH = 640;
    static const int HEIGHT = 480;
    static const int FPS = 30;

    uvc_context_t* context = nullptr;
    uvc_device_t* device = nullptr;
    uvc_device_handle_t* deviceHandle = nullptr;
    uvc_stream_ctrl_t control;

    std::shared_ptr<messages::sensors::ImageT> latestImage;
    std::mutex latestImageMutex;
};

}  // namespace ref
