#include "src/Commands.h"
#include "src/Device.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace {

constexpr int POINTS_PER_CHANNEL = 0x2000; // 8192
constexpr int CHANNELS = 2;

constexpr int REQUESTED_BYTES =
    POINTS_PER_CHANNEL * CHANNELS;

} // namespace


int main() {
    Device device;

    if (!device.open()) {
        return 1;
    }

    const auto startData =
        HantekCommands::startSampling();

    if (
        !device.controlWrite(
            HantekCommands::START_SAMPLING,
            0,
            0,
            startData.data(),
            static_cast<std::uint16_t>(
                startData.size()
            )
        )
    ) {
        std::cerr << "Failed to start sampling\n";
        return 1;
    }

    std::vector<std::uint8_t> rawData;
    int transferredLength = 0;

    const bool readSuccess = device.bulkRead(
        rawData,
        REQUESTED_BYTES,
        transferredLength,
        3000
    );

    const auto stopData =
        HantekCommands::stopSampling();

    const bool stopSuccess = device.controlWrite(
        HantekCommands::START_SAMPLING,
        0,
        0,
        stopData.data(),
        static_cast<std::uint16_t>(
            stopData.size()
        )
    );

    if (!stopSuccess) {
        std::cerr << "Failed to stop sampling\n";
    }

    if (!readSuccess) {
        return 1;
    }

    if (transferredLength != REQUESTED_BYTES) {
        std::cerr
            << "Unexpected data size: "
            << transferredLength
            << ", expected "
            << REQUESTED_BYTES
            << '\n';

        return 1;
    }

    std::cout.write(
        reinterpret_cast<const char*>(rawData.data()),
        static_cast<std::streamsize>(rawData.size())
    );

    std::cout.flush();

    if (!std::cout) {
        std::cerr << "Failed to write capture data to stdout\n";
        return 1;
    }

    return 0;
}