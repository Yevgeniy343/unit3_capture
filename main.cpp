#include "src/Commands.h"
#include "src/Device.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

constexpr int POINTS_PER_CHANNEL = 0x2000;
constexpr int CHANNELS = 2;
constexpr int REQUESTED_BYTES = POINTS_PER_CHANNEL * CHANNELS;

int main() {

    Device device;

    if (!device.open()) {
        return 1;
    }

    sf::RenderWindow window(
        sf::VideoMode(1400, 700),
        "Hantek 6022"
    );

    window.setFramerateLimit(60);

    std::vector<std::uint8_t> rawData;
    std::vector<std::uint8_t> ch1;

    rawData.reserve(REQUESTED_BYTES);
    ch1.reserve(POINTS_PER_CHANNEL);

    sf::VertexArray waveform(sf::LineStrip, POINTS_PER_CHANNEL);

    while (window.isOpen()) {

        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }

        //----------------------------------------
        // START
        //----------------------------------------

        const auto startData =
            HantekCommands::startSampling();

        if (!device.controlWrite(
                HantekCommands::START_SAMPLING,
                0,
                0,
                startData.data(),
                startData.size())) {

            continue;
        }

        //----------------------------------------
        // READ
        //----------------------------------------

        int transferred = 0;

        if (!device.bulkRead(
                rawData,
                REQUESTED_BYTES,
                transferred,
                3000)) {

            continue;
        }

        //----------------------------------------
        // STOP
        //----------------------------------------

        const auto stopData =
            HantekCommands::stopSampling();

        device.controlWrite(
            HantekCommands::START_SAMPLING,
            0,
            0,
            stopData.data(),
            stopData.size()
        );

        //----------------------------------------
        // CH1
        //----------------------------------------

        ch1.clear();

        for (
            std::size_t i = 0;
            i + 1 < rawData.size();
            i += 2
        ) {
            ch1.push_back(rawData[i]);
        }

        //----------------------------------------
        // DRAW
        //----------------------------------------

        window.clear(sf::Color::Black);

        const float width =
            static_cast<float>(window.getSize().x);

        const float height =
            static_cast<float>(window.getSize().y);

        const float dx =
            width / static_cast<float>(POINTS_PER_CHANNEL);

        for (
            std::size_t i = 0;
            i < ch1.size();
            i++
        ) {

            float x =
                static_cast<float>(i) * dx;

            float y =
                height -
                (
                    static_cast<float>(ch1[i]) / 255.0f
                ) * height;

            waveform[i].position = {x, y};

            waveform[i].color = sf::Color::Green;
        }

        window.draw(waveform);
                window.display();
    }

    device.close();

    return 0;
}