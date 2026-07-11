#include "src/Commands.h"
#include "src/Device.h"

#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

namespace {

constexpr int POINTS_PER_CHANNEL = 0x2000;
constexpr int CHANNELS = 2;
constexpr int REQUESTED_BYTES =
    POINTS_PER_CHANNEL * CHANNELS;

} // namespace

int main() {

    Device device;

    if (!device.open()) {
        return 1;
    }

    while (true) {

        // START

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
            std::cerr
                << "Failed to start sampling\n";

            break;
        }

        // READ

        std::vector<std::uint8_t> rawData;

        int transferredLength = 0;

        const bool readSuccess =
            device.bulkRead(
                rawData,
                REQUESTED_BYTES,
                transferredLength,
                3000
            );

        // STOP

        const auto stopData =
            HantekCommands::stopSampling();

        device.controlWrite(
            HantekCommands::START_SAMPLING,
            0,
            0,
            stopData.data(),
            static_cast<std::uint16_t>(
                stopData.size()
            )
        );

        if (!readSuccess) {
            std::cerr
                << "Read failed\n";

            break;
        }

        if (
            transferredLength !=
            REQUESTED_BYTES
        ) {

            std::cerr
                << "Wrong frame size: "
                << transferredLength
                << '\n';

            continue;
        }

        //
        // Пишем размер кадра
        //

        std::uint32_t size =
            static_cast<std::uint32_t>(
                rawData.size()
            );

        std::cout.write(
            reinterpret_cast<char*>(&size),
            sizeof(size)
        );

        //
        // Пишем кадр
        //

        std::cout.write(
            reinterpret_cast<char*>(
                rawData.data()
            ),
            rawData.size()
        );

        std::cout.flush();

        //
        // Чтобы не грузить CPU
        //

        std::this_thread::sleep_for(
            std::chrono::milliseconds(20)
        );
    }

    device.close();

    return 0;
}