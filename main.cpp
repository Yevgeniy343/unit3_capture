#include "src/Commands.h"
#include "src/Device.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#include "httplib.h"

namespace {

constexpr int POINTS_PER_CHANNEL = 0x2000; // 8192
constexpr int CHANNELS = 2;

// В потоке данные чередуются:
//
// CH1, CH2, CH1, CH2...
constexpr int REQUESTED_BYTES =
    POINTS_PER_CHANNEL * CHANNELS;

void printFirstValues(
    const std::vector<std::uint8_t>& values,
    std::size_t count
) {
    const std::size_t valuesToPrint =
        std::min(count, values.size());

    for (std::size_t i = 0; i < valuesToPrint; ++i) {
        if (i > 0) {
            std::cout << ", ";
        }

        std::cout
            << static_cast<unsigned int>(values[i]);
    }

    std::cout << '\n';
}

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

    std::cout << "Sampling started\n";

    std::vector<std::uint8_t> rawData;
    int transferredLength = 0;

    const bool readSuccess = device.bulkRead(
        rawData,
        REQUESTED_BYTES,
        transferredLength,
        3000
    );

    // Останавливаем выборку независимо от результата чтения.
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

    if (stopSuccess) {
        std::cout << "Sampling stopped\n";
    } else {
        std::cerr << "Failed to stop sampling\n";
    }

    if (!readSuccess) {
        return 1;
    }

    std::vector<std::uint8_t> channel1;
    std::vector<std::uint8_t> channel2;

    channel1.reserve(
        rawData.size() / 2
    );

    channel2.reserve(
        rawData.size() / 2
    );

    for (
        std::size_t index = 0;
        index + 1 < rawData.size();
        index += 2
    ) {
        channel1.push_back(rawData[index]);
        channel2.push_back(rawData[index + 1]);
    }

    std::cout
        << "Raw bytes: "
        << rawData.size()
        << '\n';

    std::cout
        << "CH1 points: "
        << channel1.size()
        << '\n';

    std::cout
        << "CH2 points: "
        << channel2.size()
        << '\n';

    std::cout << "CH1 first 20: ";
    printFirstValues(channel1, 20);

    std::cout << "CH2 first 20: ";
    printFirstValues(channel2, 20);

    if (!channel1.empty()) {
        const auto [minimum, maximum] =
            std::minmax_element(
                channel1.begin(),
                channel1.end()
            );

        std::cout
            << "CH1 min: "
            << static_cast<unsigned int>(*minimum)
            << '\n';

        std::cout
            << "CH1 max: "
            << static_cast<unsigned int>(*maximum)
            << '\n';
    }

    return 0;
}