cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0146 OLD)
project(StreamVision-Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(ZeroMQ REQUIRED libzmq)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json REQUIRED)
find_path(CPPZMQ_INCLUDE_DIR zmq.hpp)
if(CPPZMQ_INCLUDE_DIR)
    message(STATUS "Found cppzmq at ${CPPZMQ_INCLUDE_DIR}")
else()
    message(FATAL_ERROR "cppzmq not found!")
endif()

set(SOURCES
    src/main.cpp
    src/CameraCapture.cpp
    src/FrameEncoder.cpp
    src/FrameMetadata.cpp
    src/FrameSender.cpp
	src/JsonParser.cpp
	src/PipelineController.cpp
	src/WebStream.cpp
)

add_executable(streamvision-client ${SOURCES})

target_include_directories(streamvision-client
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
        ${ZeroMQ_INCLUDE_DIRS}
        ${CPPZMQ_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(streamvision-client
    PRIVATE
        ${OpenCV_LIBS}
        ${ZeroMQ_LIBRARIES}
        nlohmann_json::nlohmann_json
)

target_compile_options(streamvision-client PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -Wpedantic -Werror -Wimplicit-fallthrough -Wsign-conversion>
    $<$<CONFIG:Release>:-O3>
)

# Doxygen Documentation
option(BUILD_DOCS "Build Doxygen documentation" OFF)
find_package(Doxygen QUIET)
if(BUILD_DOCS AND DOXYGEN_FOUND)
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
elseif(NOT BUILD_DOCS)
    message(STATUS "BUILD_DOCS is OFF.")
else()
    message(STATUS "Doxygen not found. Documentation target will not be available.")
endif()
