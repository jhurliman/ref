#include "webcam/WebcamDriver.h"

#include <algorithm>
#include <core/Assert.h>
#include <core/Log.h>
#include <core/Time.h>

namespace ref {

static void callbackWrapper(uvc_frame_t* frame, void* ptr) {
    static_cast<WebcamDriver*>(ptr)->callback(frame);
}

WebcamDriver::WebcamDriver(const NodeDefinition& def) : NodeBase(def) {
    int32_t vendor_id = def.parameters().readInt(STRING(vendor_id));
    int32_t product_id = def.parameters().readInt(STRING(product_id));
    std::string serial_number = def.parameters().readString(STRING(serial_number));

    LOG_DEBUG(
            "WebcamDriver(vendor_id=%d, product_id=%d, serial_number=\"%s\")",
            vendor_id,
            product_id,
            serial_number);

    // Initialize libuvc
    uvc_error_t res = uvc_init(&context, nullptr);
    ASSERT_ALWAYS(res >= 0, Format("uvc_init failed: %s", uvc_strerror(res)));

    // Find the USB device we're looking for (defaults to first video device)
    const char* serialNumber = serial_number.empty() ? nullptr : serial_number.c_str();
    res = uvc_find_device(context, &device, vendor_id, product_id, serialNumber);
    ASSERT_ALWAYS(res >= 0, Format("uvc_find_device failed: %s", uvc_strerror(res)));

    // Fetch information about the located device
    uvc_device_descriptor_t* desc = nullptr;
    res = uvc_get_device_descriptor(device, &desc);
    ASSERT_ALWAYS(res >= 0, Format("uvc_get_device_descriptor failed: %s", uvc_strerror(res)));
    LOG_INFO(
            "Using camera \"%s\" (vendor_id=%d, product_id=%d, serial_number=%s)",
            desc->product ? desc->product : "(unknown)",
            desc->idVendor,
            desc->idProduct,
            desc->serialNumber ? desc->serialNumber : "(unknown)");
    uvc_free_device_descriptor(desc);

    // Open the device
    res = uvc_open(device, &deviceHandle);
    ASSERT_ALWAYS(res >= 0, Format("uvc_open failed: %s", uvc_strerror(res)));

    // Populate our stream configuration object
    res = uvc_get_stream_ctrl_format_size(
            deviceHandle, &control, UVC_FRAME_FORMAT_MJPEG, WIDTH, HEIGHT, FPS);
    ASSERT_ALWAYS(
            res >= 0, Format("uvc_get_stream_ctrl_format_size failed: %s", uvc_strerror(res)));

    // Start streaming frames of video
    res = uvc_start_streaming(deviceHandle, &control, callbackWrapper, this, 0);
    ASSERT_ALWAYS(res >= 0, Format("uvc_start_streaming failed: %s", uvc_strerror(res)));

    // Enable auto-exposure
    uvc_set_ae_mode(deviceHandle, 1);
}

void WebcamDriver::tick(const NodeMessages& input, WebcamDriverOutput* output) {
    LOG_DEBUG("WebcamDriver::tick(%s)", Time::ToNanoseconds(input.currentTime));

    std::lock_guard<std::mutex> lock(latestImageMutex);
    if (!latestImage) {
        return;
    }

    output->image_compressed = latestImage;
    latestImage = nullptr;
}

void WebcamDriver::callback(uvc_frame_t* frame) {
    ASSERT_ALWAYS(frame != nullptr);

    // NOTE: frame->capture_time is currently not populated by libuvc. See
    // <https://github.com/ktossell/libuvc/issues/17>
    // Time::TimePoint captureTime = Time::FromTimeval(frame->capture_time);
    Time::TimePoint captureTime = Time::Now();
    uint8_t* srcData = static_cast<uint8_t*>(frame->data);

    LOG_DEBUG(
            "WebcamDriver::callback(), len=%d, now=%f",
            frame->data_bytes,
            Time::ToSeconds(captureTime));

    {
        std::lock_guard<std::mutex> lock(latestImageMutex);

        // TODO: Create a memory pool of these to prevent repeated large
        // allocations when copying srcData to latestImage->data
        latestImage = std::make_shared<messages::sensors::ImageT>();
        if (!latestImage->header) {
            latestImage->header = std::make_unique<messages::HeaderT>();
        }
        latestImage->header->sensor_stamp = Time::ToNanoseconds(captureTime);
        latestImage->height = HEIGHT;
        latestImage->width = WIDTH;
        latestImage->encoding = "MJPEG";
        latestImage->is_bigendian = false;
        latestImage->step = 0;
        latestImage->data.resize(frame->data_bytes);
        std::copy(srcData, srcData + frame->data_bytes, latestImage->data.begin());
    }
}

}  // namespace ref
