#pragma once

#include <cstdint>
#include <vector>

#include <libusb-1.0/libusb.h>

class Device {
public:
    Device();
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    bool open();
    void close();

    bool isOpen() const;

    bool controlWrite(
        std::uint8_t request,
        std::uint16_t value,
        std::uint16_t index,
        const std::uint8_t* data,
        std::uint16_t length,
        unsigned int timeoutMs = 1000
    );

    bool bulkRead(
        std::vector<std::uint8_t>& buffer,
        int requestedLength,
        int& transferredLength,
        unsigned int timeoutMs = 3000
    );

private:
    libusb_context* context = nullptr;
    libusb_device_handle* handle = nullptr;
    bool interfaceClaimed = false;
};