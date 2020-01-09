#include "opencv_video/OpenCVVideoDriver.h"

#include <algorithm>
#include <core/Assert.h>
#include <core/Format.h>
#include <core/Log.h>
#include <core/Time.h>

namespace ref {

OpenCVVideoDriver::OpenCVVideoDriver(const NodeDefinition& def, const Graph& graph)
        : NodeBase(def, graph), _input(def.inputs()), _output(def.outputs()) {
    // Initialize OpenCV VideoCapture interface
    _capture.open(0);
    REF_ASSERT(_capture.isOpened(), "Failed to open video capture via OpenCV");

    LOG_DEBUG("OpenCVVideoDriver(backend=\"%s\")", _capture.getBackendName());
}

void OpenCVVideoDriver::tick() {
    LOG_DEBUG("OpenCVVideoDriver::tick(%s)", Time::ToNanoseconds(_input.currentTime()));

    // Capture a new image
    cv::Mat image;
    _capture.read(image);

    Time::TimePoint captureTime = Time::Now();
    cv::Size size = image.size();
    size_t totalBytes = image.step[0] * image.rows;

    // Build the image output message
    auto imgMsg = std::make_unique<messages::sensors::ImageT>();
    imgMsg->header = std::make_unique<messages::HeaderT>();
    imgMsg->header->sensor_stamp = Time::ToNanoseconds(captureTime);
    imgMsg->height = size.height;
    imgMsg->width = size.width;
    imgMsg->encoding = "MJPEG";
    imgMsg->is_bigendian = false;
    imgMsg->step = 0;
    imgMsg->data.resize(totalBytes);
    std::copy(image.data, image.data + totalBytes, imgMsg->data.begin());

    // Move imgMsg -> _output
    _output.image_compressed.message = std::move(imgMsg);
}

}  // namespace ref
