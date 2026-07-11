#include "Capture.h"
#include "Commands.h"

constexpr int POINTS_PER_CHANNEL = 0x2000;
constexpr int CHANNELS = 2;
constexpr int REQUESTED_BYTES =
    POINTS_PER_CHANNEL * CHANNELS;

Capture::Capture(Device& d)
    : device(d)
{
}

bool Capture::read(CaptureFrame& frame) {

    const auto startData =
        HantekCommands::startSampling();

    if (!device.controlWrite(
            HantekCommands::START_SAMPLING,
            0,
            0,
            startData.data(),
            startData.size()))
        return false;

    std::vector<std::uint8_t> rawData;

    int transferred = 0;

    if (!device.bulkRead(
            rawData,
            REQUESTED_BYTES,
            transferred,
            3000))
        return false;

    const auto stopData =
        HantekCommands::stopSampling();

    device.controlWrite(
        HantekCommands::START_SAMPLING,
        0,
        0,
        stopData.data(),
        stopData.size());

    frame.ch1.clear();
    frame.ch2.clear();

    frame.ch1.reserve(rawData.size() / 2);
    frame.ch2.reserve(rawData.size() / 2);

    for (size_t i = 0; i + 1 < rawData.size(); i += 2) {

        frame.ch1.push_back(rawData[i]);
        frame.ch2.push_back(rawData[i + 1]);
    }

    return true;
}