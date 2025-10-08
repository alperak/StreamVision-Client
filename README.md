![C++](https://img.shields.io/badge/C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![OpenCV](https://img.shields.io/badge/OpenCV-4.12.0-5C3EE8?style=for-the-badge&logo=opencv&logoColor=white)
![ZeroMQ](https://img.shields.io/badge/ZeroMQ-4.3.5-DF0000?style=for-the-badge&logoColor=white)
![cppzmq](https://img.shields.io/badge/cppzmq-4.11.0-DF0000?style=for-the-badge&logoColor=white)
![nlohmann-json](https://img.shields.io/badge/nlohmann--json-3.12.0-00599C?style=for-the-badge&logoColor=white)
![cpp-httplib](https://img.shields.io/badge/cpp--httplib-0.26.0-00599C?style=for-the-badge&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-3.10+-064F8C?style=for-the-badge&logo=cmake&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-Ready-2496ED?style=for-the-badge&logo=docker&logoColor=white)
![Doxygen](https://img.shields.io/badge/Docs-Doxygen-2C4AA8?style=for-the-badge&logo=doxygen&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-Ubuntu_24.04-E95420?style=for-the-badge&logo=ubuntu&logoColor=white)

## Table of Contents

- [Project Overview](#overview)
- [Architecture](#architecture)
- [Dependencies](#dependencies)
- [Installation](#installation)
  - [Docker (Recommended)](#docker-recommended)
  - [Manual Build](#manual-build)
- [Usage](#usage)

---

## Overview

StreamVision is a distributed object detection system consisting of two components:

- **StreamVision-Client** (this repository): C++ application that captures camera frames, sends them to a detection server, receives results, draws bounding boxes on frames, and displays live video stream in web browsers.
- **[StreamVision-Server](https://github.com/alperak/StreamVision-Server)** (separate repository): C++ application that receives encoded frames, decodes them, runs inference with YOLOv11/v12, and returns detection results to client.

---

## Architecture

```
┌─────────────────┐
│ CameraCapture   │ → Captures frames from camera
└────────┬────────┘
         ↓
┌─────────────────┐
│ FrameEncoder    │ → Encodes frames as JPEG
└────────┬────────┘
         ↓
┌─────────────────┐
│ FrameHandler    │ → Sends encoded frames/receives detections via ZeroMQ (REQ-REP pattern)
└────────┬────────┘
         ↓
┌─────────────────┐
│ JsonParser      │ → Parses detection results
└────────┬────────┘
         ↓
┌─────────────────┐
│ Drawer          │ → Draws bounding boxes and labels
└────────┬────────┘
         ↓
┌─────────────────┐
│ WebStream       │ → Streams MJPEG to web browser
└─────────────────┘
```

---

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| OpenCV | 4.12.0 | Frame capture and image processing |
| ZeroMQ (libzmq) | 4.3.5 | Network communication |
| cppzmq | 4.11.0 | C++ bindings for ZeroMQ |
| nlohmann-json | 3.12.0 | JSON parsing |
| cpp-httplib | 0.26.0 | HTTP server for streaming |
| Doxygen | Latest | Documentation generation (optional) |

---

## Installation

### Docker (Recommended)

The easiest way to build and run StreamVision-Client:

```bash
# Clone the repository
git clone https://github.com/yourusername/StreamVision-Client.git
cd StreamVision-Client

# Build Docker image
docker build -t streamvision-client .

# Run container
docker run -it --rm \
  --device=/dev/video0:/dev/video0 \
  --network host \
  streamvision-client
```

**Note**: `--device=/dev/video0` mounts your camera. Adjust if using a different camera index.

### Manual Build

You can install dependencies by following the steps in the `Dockerfile`.

---

## Usage

### Quick Start

1. **Start the detection server** (see [StreamVision-Server](https://github.com/alperak/StreamVision-Server) repository)

2. **Run the client application**:

```bash
./streamvision-client
```

3. **View the live stream** in your browser:

```
http://localhost:8080/stream
```

4. **View the doxygen documentation** in your browser:

```
http://localhost:8001
```
