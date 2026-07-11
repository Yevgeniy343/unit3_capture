#pragma once

#include <array>
#include <cstdint>

namespace HantekCommands {

constexpr std::uint8_t SET_GAIN_CH1 = 0xE0;
constexpr std::uint8_t SET_GAIN_CH2 = 0xE1;
constexpr std::uint8_t SET_SAMPLE_RATE = 0xE2;
constexpr std::uint8_t START_SAMPLING = 0xE3;
constexpr std::uint8_t SET_NUM_CHANNELS = 0xE4;
constexpr std::uint8_t SET_COUPLING = 0xE5;
constexpr std::uint8_t SET_CAL_FREQUENCY = 0xE6;

inline std::array<std::uint8_t, 1> startSampling() {
    return {0x01};
}

inline std::array<std::uint8_t, 1> stopSampling() {
    return {0x00};
}

inline std::array<std::uint8_t, 1> setNumChannels(
    std::uint8_t channels
) {
    return {channels};
}

inline std::array<std::uint8_t, 2> setSampleRate(
    std::uint8_t id,
    std::uint8_t index
) {
    return {id, index};
}

inline std::array<std::uint8_t, 2> setGain(
    std::uint8_t gain,
    std::uint8_t index
) {
    return {gain, index};
}

} // namespace HantekCommands