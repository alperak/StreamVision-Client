# StreamVision-Client

---

## Table of Contents

- [Project Overview](#overview)
- [Demo](#demo)
- [Architecture](#architecture)
- [Dependencies](#dependencies)
- [Installation](#installation)
  - [Docker (Recommended)](#docker-recommended)
  - [Manual Build](#manual-build)
- [Usage](#usage)

---

## Overview

StreamVision is a distributed real time object detection system consisting of two components:

- **StreamVision-Client** (this repository): Multi threaded C++ video streaming client built on ZeroMQ DEALER-ROUTER pattern. Captures frames from camera, encodes them as JPEG, sends to detection server via asynchronous I/O thread, receives JSON detection results, draws bounding boxes with labels on a drawing thread and streams annotated video as MJPEG to web browsers.
- **[StreamVision-Server](https://github.com/alperak/StreamVision-Server)** (separate repository): Multi client C++ detection server that receives JPEG encoded frames, runs YOLOv11/v12 inference with GPU acceleration and returns JSON detection results back to each client.

---

## Demo

A demo captured using **YOLOV11s**:

![streamvision-demo](https://github.com/user-attachments/assets/22f422fa-1839-45f1-98fc-eaa7f44b82bb)

---

## Architecture

> **Note:** Application settings (Camera Device Index, Resolution, FPS, and Server Address, Port) are managed by `ConfigXML` singleton class, loaded once at startup.

```
Camera Device
  1. CameraCapture reads frame (captureThread_)
        |
        v
PipelineController (pipelineThread_)
  2. FrameEncoder::encodeJPEG()
        |
        v
FrameHandler
  3. setEncodedFrame() -> holder (mutex protected)
        |
        |  [ioThread_]
        v
DEALER Socket
  4. Send: [empty delimiter][JPEG data]
        |
        v
  (to StreamVision-Server)
        |
        v
  5. Recv: [empty delimiter][JSON detections]
        |
        v
PipelineController
  6. JsonParser::parse()
        |
        v
Drawer
  7. setFrameAndDetections() -> condition_variable wakes drawerThread_
        |
        |  [drawerThread_]
        v
  8. Clone frame, draw boxes with cv::rectangle/putText
        |
        v
WebStream
  9. setFrame() -> holder (mutex protected)
        |
        |  [webServerThread_]
        v
  10. HTTP MJPEG stream to browser
```

---

### Thread Model

| Thread | Owner | Responsibility |
|--------|-------|----------------|
| main | main() | Signal handling, application lifecycle |
| captureThread_ | CameraCapture | Frame capture from camera |
| pipelineThread_ | PipelineController | Encode, parse, orchestrate pipeline |
| ioThread_ | FrameHandler | DEALER socket send/recv |
| drawerThread_ | Drawer | Draw detection boxes on frames |
| webServerThread_ | WebStream | HTTP MJPEG streaming |

---

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| [OpenCV](https://opencv.org/) | 4.12.0 | Frame capture and image processing |
| [ZeroMQ (libzmq)](https://zeromq.org/) | 4.3.5 | High-performance asynchronous messaging library |
| [cppzmq](https://github.com/zeromq/cppzmq) | 4.11.0 | Header-only C++ bindings for ZeroMQ |
| [nlohmann/json](https://github.com/nlohmann/json) | 3.12.0 | Modern C++ JSON parsing |
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | 0.26.0 | Header-only HTTP server for MJPEG streaming |
| [spdlog](https://github.com/gabime/spdlog) | v1.17.0 | Fast C++ logging library |
| [TinyXML2](https://github.com/leethomason/tinyxml2) | 11.0.0 | XML configuration parsing |
| [Doxygen](https://www.doxygen.nl/) | Latest | API documentation generator (optional) |

---

## Installation

### Docker (Recommended)

The easiest way to build and run StreamVision-Client:

```bash
# Clone the repository
git clone https://github.com/alperak/StreamVision-Client.git
cd StreamVision-Client

# Build Docker image
sudo docker build -t streamvision-client .

# Run container
sudo docker run -it --rm \
  --device=/dev/video0:/dev/video0 \
  --network host \
  streamvision-client
```

> **Note:** `--device=/dev/video0` mounts your camera. Adjust if using a different camera index. You can check available cameras with `v4l2-ctl --list-devices` command.

### Manual Build

For manual installation, follow the dependency build steps in the [`Dockerfile`](Dockerfile).

---

## Usage

### Quick Start

1. **Start the detection server** (see [StreamVision-Server](https://github.com/alperak/StreamVision-Server) repository)

2. **Configure the application** by editing [config/config.xml](config/config.xml):

```xml
<Config>
    <Camera>
        <Index>0</Index>           <!-- Camera device index -->
        <Width>640</Width>         <!-- Camera resolution width -->
        <Height>480</Height>       <!-- Camera resolution height -->
        <FPS>30</FPS>              <!-- Camera frames per second -->
    </Camera>
    <Server>
        <IP>0.0.0.0</IP>           <!-- Detection server IP address -->
        <Port>5555</Port>          <!-- Detection server port -->
    </Server>
</Config>
```

> **Note:**
> - If you don't configure, it will start with the current default values.
> - Configure camera settings based on your device capabilities.

3. **Run the client application**:

```bash
./streamvision-client
```

4. **View the live stream** in your browser:

```
http://localhost:8080/stream
```

5. **View the doxygen documentation** in your browser:

```
http://localhost:8001
```
