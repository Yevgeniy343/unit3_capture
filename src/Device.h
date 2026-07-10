#pragma once

#include <libusb-1.0/libusb.h>

class Device {
public:
    Device();
    ~Device();

    bool open();
    void close();

private:
    libusb_context* context = nullptr;
    libusb_device_handle* handle = nullptr;
};