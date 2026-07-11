#pragma once

#include "Device.h"

#include <cstdint>
#include <vector>

struct CaptureFrame {
    int sampleRate = 4000000;

    std::vector<std::uint8_t> ch1;
    std::vector<std::uint8_t> ch2;
};

class Capture {
public:
    explicit Capture(Device& device);

    bool read(CaptureFrame& frame);

private:
    Device& device;
};