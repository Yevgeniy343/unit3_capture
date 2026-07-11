#include "src/Commands.h"
#include "src/Device.h"

#include <iostream>

int main() {
    Device device;

    if (!device.open()) {
        return 1;
    }

    const auto startData =
        HantekCommands::startSampling();

    const bool started = device.controlWrite(
        HantekCommands::START_SAMPLING,
        0,
        0,
        startData.data(),
        static_cast<std::uint16_t>(startData.size())
    );

    if (!started) {
        std::cerr << "Failed to start sampling\n";
        return 1;
    }

    std::cout << "Sampling started\n";

    const auto stopData =
        HantekCommands::stopSampling();

    const bool stopped = device.controlWrite(
        HantekCommands::START_SAMPLING,
        0,
        0,
        stopData.data(),
        static_cast<std::uint16_t>(stopData.size())
    );

    if (!stopped) {
        std::cerr << "Failed to stop sampling\n";
        return 1;
    }

    std::cout << "Sampling stopped\n";

    return 0;
}