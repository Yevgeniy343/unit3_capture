#pragma once

#include <array>
#include <cstdint>

namespace HantekCommands {

// Команды соответствуют ControlCode из OpenHantek:
//
// 0xE0 — усиление CH1
// 0xE1 — усиление CH2
// 0xE2 — частота дискретизации
// 0xE3 — запуск/остановка выборки
// 0xE4 — количество каналов
// 0xE5 — AC/DC coupling
// 0xE6 — частота внутреннего калибровочного генератора

constexpr std::uint8_t SET_GAIN_CH1 = 0xE0;
constexpr std::uint8_t SET_GAIN_CH2 = 0xE1;
constexpr std::uint8_t SET_SAMPLE_RATE = 0xE2;
constexpr std::uint8_t START_SAMPLING = 0xE3;
constexpr std::uint8_t SET_NUM_CHANNELS = 0xE4;
constexpr std::uint8_t SET_COUPLING = 0xE5;
constexpr std::uint8_t SET_CAL_FREQUENCY = 0xE6;

// Тип USB-запроса:
// Host -> Device
// Vendor
// Device recipient
constexpr std::uint8_t REQUEST_TYPE_VENDOR_OUT = 0x40;

// Таймаут control transfer в миллисекундах.
constexpr unsigned int CONTROL_TIMEOUT_MS = 1000;

// ---------------------------------------------------------
// Формирование данных команд
// ---------------------------------------------------------

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

// OpenHantek передаёт для samplerate два байта:
//
// data[0] = id
// data[1] = индекс частоты в таблице модели
//
// Конкретные значения id/index мы добавим после того,
// как найдём таблицу fixedSampleRates для DSO-6022BE.
inline std::array<std::uint8_t, 2> setSampleRate(
    std::uint8_t id,
    std::uint8_t index
) {
    return {id, index};
}

// Команда усиления также содержит два байта:
//
// data[0] = аппаратное значение усиления
// data[1] = индекс диапазона
inline std::array<std::uint8_t, 2> setGain(
    std::uint8_t gain,
    std::uint8_t index
) {
    return {gain, index};
}

// Для coupling используется битовая маска:
//
// CH1 DC = 0x01
// CH2 DC = 0x10
//
// Оба канала DC: 0x11
// Оба канала AC: 0x00
inline std::array<std::uint8_t, 1> setCoupling(
    bool ch1Dc,
    bool ch2Dc
) {
    std::uint8_t value = 0x00;

    if (ch1Dc) {
        value |= 0x01;
    }

    if (ch2Dc) {
        value |= 0x10;
    }

    return {value};
}

// 1 соответствует внутреннему генератору 1 кГц.
inline std::array<std::uint8_t, 1> setCalibrationFrequency(
    std::uint8_t value
) {
    return {value};
}

} // namespace HantekCommands