#include "PipelineController.hpp"

#include <iostream>

int main() {

    PipelineController pipeline;
    pipeline.start();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    pipeline.stop();  // clean shutdown
    // CameraCapture cam(1);
    // FrameSender sender;

    // cam.start();
    // sender.start();

    // while (true) {
    //     auto frame = cam.getLatestFrame();
    //     if (frame && !frame->empty()) {
    //         auto encoded = FrameEncoder::encodeJPEG(*frame);
    //         if (!encoded.empty()) {
    //             sender.pushEncodedFrame(std::move(encoded));
    //         } 
    //     }
    // }

    // sender.stop();
    // cam.stop();

    return 0;
}