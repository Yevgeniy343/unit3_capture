#include "src/Commands.h"
#include "src/Device.h"

#include <cstdint>
#include <iostream>
#include <vector>

int main() {
    Device device;

    if (!device.open()) {
        return 1;
    }

    constexpr int POINTS_PER_CHANNEL = 0x2000;
    constexpr int CHANNELS = 2;
    constexpr int REQUESTED_BYTES = POINTS_PER_CHANNEL * CHANNELS;

    for (int frame = 1; frame <= 1000; frame++) {

        // START
        const auto startData = HantekCommands::startSampling();

        if (!device.controlWrite(
                HantekCommands::START_SAMPLING,
                0,
                0,
                startData.data(),
                static_cast<std::uint16_t>(startData.size()))) {

            std::cout << "START failed on frame " << frame << std::endl;
            break;
        }

        // READ
        std::vector<std::uint8_t> rawData;
        int transferred = 0;

        if (!device.bulkRead(
                rawData,
                REQUESTED_BYTES,
                transferred,
                3000)) {

            std::cout << "READ failed on frame " << frame << std::endl;
            break;
        }

        // STOP
        const auto stopData = HantekCommands::stopSampling();

        if (!device.controlWrite(
                HantekCommands::START_SAMPLING,
                0,
                0,
                stopData.data(),
                static_cast<std::uint16_t>(stopData.size()))) {

            std::cout << "STOP failed on frame " << frame << std::endl;
            break;
        }

        std::cout
            << "Frame "
            << frame
            << "   bytes="
            << transferred
            << std::endl;
    }

    return 0;
}