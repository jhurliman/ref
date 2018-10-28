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

class WebcamDriverOutput : public NodeOutputs {
    PublishedMessage<ImageT> image_compressed;
    PublishedMessage<CameraInfoT> camera_info;

    WebcamDriverOutput(NodeDefinition::IDToTopicMap& idToTopicMap)
            : NodeOutputs(idToTopicMap)
            , image_compressed(idToTopicMap[STR(image_compressed)])
            , camera_info(idToTopicMap[STR(camera_info)]) {}

    virtual void serialize() {
        serializeMessage<Image, ImageT>(image_compressed);
        serializeMessage<CameraInfo, CameraInfoT>(camera_info);
    }

    // FIXME: Move the recording code into the Recorder or Recording classes.
    // Expose a std::unordered_map<TopicName, {publish_stamp, type?,
    // flatbuffers::FlatBufferBuilder*}>& This can be used for both deserialization when populating
    // NodeInputs classes and for recording

    // virtual void record(Recording& recording) const {
    //     using namespace messages::sensors;
    //     recordMessage<Image, ImageT>(
    //             idToTopicMap[image_compressed_key].name,
    //             image_compressed_data,
    //             recording,
    //             image_compressed.get());
    //     recordMessage<CameraInfo, CameraInfoT>(
    //             idToTopicMap[camera_info_key].name, camera_info_data, recording,
    //             camera_info.get());
    // }

    // template<class Message, class MessageT>
    // void recordMessage(const std::string& topic, const flatbuffers::FlatBufferBuilder& builder,
    // Recording& recording, MessageT* msg) {
    //     if (msg) {
    //         uint64_t timestamp = msg->header.publish_stamp;
    //         const char* data = reinterpret_cast<const char*>(builder.GetBufferPointer());
    //         recording.write(timestamp, topic, data, builder.GetSize());
    //     }
    // }
};

class WebcamDriver : public NodeBase {
public:
    WebcamDriver(const NodeDefinition& def, const Graph& graph);

    void tick(const NodeInputs& input, WebcamDriverOutput* output);
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
