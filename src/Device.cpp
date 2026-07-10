#include "Device.h"

#include <iostream>

constexpr uint16_t VENDOR_ID = 0x04B4;
constexpr uint16_t PRODUCT_ID = 0x6022;

Device::Device() {}

Device::~Device() {
    close();
}

bool Device::open() {

    if (libusb_init(&context) != 0) {
        std::cout << "libusb_init failed\n";
        return false;
    }

    handle = libusb_open_device_with_vid_pid(
        context,
        VENDOR_ID,
        PRODUCT_ID
    );

    if (!handle) {
        std::cout << "Hantek not found\n";
        return false;
    }

    std::cout << "Hantek found\n";

    int result = libusb_claim_interface(handle, 0);

    if (result != 0) {
        std::cout << "claimInterface failed: " << result << "\n";
        return false;
    }

    std::cout << "USB opened\n";

    return true;
}

void Device::close() {

    if (handle) {
        libusb_release_interface(handle, 0);
        libusb_close(handle);
        handle = nullptr;
    }

    if (context) {
        libusb_exit(context);
        context = nullptr;
    }
}