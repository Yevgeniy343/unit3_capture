#include "Device.h"

#include <iostream>

namespace {

constexpr std::uint16_t PRODUCT_ID = 0x6022;

constexpr std::uint16_t VENDOR_ID_FIRMWARE = 0x04B5;
constexpr std::uint16_t VENDOR_ID_NO_FIRMWARE = 0x04B4;

constexpr int INTERFACE_NUMBER = 0;

constexpr std::uint8_t REQUEST_TYPE_VENDOR_OUT = 0x40;
constexpr unsigned char BULK_ENDPOINT_IN = 0x86;

} // namespace


Device::Device() = default;


Device::~Device() {
    close();
}


bool Device::open() {
    if (isOpen()) {
        return true;
    }

    const int initResult = libusb_init(&context);

    if (initResult != LIBUSB_SUCCESS) {
        std::cerr
            << "libusb_init failed: "
            << libusb_error_name(initResult)
            << '\n';

        context = nullptr;
        return false;
    }

    handle = libusb_open_device_with_vid_pid(
        context,
        VENDOR_ID_FIRMWARE,
        PRODUCT_ID
    );

    if (handle == nullptr) {
        handle = libusb_open_device_with_vid_pid(
            context,
            VENDOR_ID_NO_FIRMWARE,
            PRODUCT_ID
        );
    }

    if (handle == nullptr) {
        std::cerr
            << "Hantek DSO-6022BE not found "
            << "(expected 04b5:6022 or 04b4:6022)"
            << '\n';

        libusb_exit(context);
        context = nullptr;

        return false;
    }

    const int kernelDriverActive =
        libusb_kernel_driver_active(handle, INTERFACE_NUMBER);

    if (kernelDriverActive == 1) {
        const int detachResult =
            libusb_detach_kernel_driver(
                handle,
                INTERFACE_NUMBER
            );

        if (
            detachResult != LIBUSB_SUCCESS &&
            detachResult != LIBUSB_ERROR_NOT_FOUND
        ) {
            std::cerr
                << "Failed to detach kernel driver: "
                << libusb_error_name(detachResult)
                << '\n';

            close();
            return false;
        }
    }

    const int claimResult =
        libusb_claim_interface(
            handle,
            INTERFACE_NUMBER
        );

    if (claimResult != LIBUSB_SUCCESS) {
        std::cerr
            << "Failed to claim USB interface: "
            << libusb_error_name(claimResult)
            << '\n';

        close();
        return false;
    }

    interfaceClaimed = true;

    std::cerr << "Hantek connected\n";

    return true;
}


void Device::close() {
    if (handle != nullptr) {
        if (interfaceClaimed) {
            const int releaseResult =
                libusb_release_interface(
                    handle,
                    INTERFACE_NUMBER
                );

            if (
                releaseResult != LIBUSB_SUCCESS &&
                releaseResult != LIBUSB_ERROR_NO_DEVICE
            ) {
                std::cerr
                    << "Failed to release USB interface: "
                    << libusb_error_name(releaseResult)
                    << '\n';
            }

            interfaceClaimed = false;
        }

        libusb_close(handle);
        handle = nullptr;
    }

    if (context != nullptr) {
        libusb_exit(context);
        context = nullptr;
    }
}


bool Device::isOpen() const {
    return handle != nullptr && interfaceClaimed;
}


bool Device::controlWrite(
    std::uint8_t request,
    std::uint16_t value,
    std::uint16_t index,
    const std::uint8_t* data,
    std::uint16_t length,
    unsigned int timeoutMs
) {
    if (!isOpen()) {
        std::cerr
            << "controlWrite failed: device is not open\n";

        return false;
    }

    auto* mutableData =
        const_cast<unsigned char*>(data);

    const int transferred = libusb_control_transfer(
        handle,
        REQUEST_TYPE_VENDOR_OUT,
        request,
        value,
        index,
        mutableData,
        length,
        timeoutMs
    );

    if (transferred < 0) {
        std::cerr
            << "Control transfer failed, request=0x"
            << std::hex
            << static_cast<int>(request)
            << std::dec
            << ": "
            << libusb_error_name(transferred)
            << '\n';

        return false;
    }

    if (transferred != static_cast<int>(length)) {
        std::cerr
            << "Control transfer incomplete: expected "
            << length
            << ", transferred "
            << transferred
            << '\n';

        return false;
    }

    return true;
}


bool Device::bulkRead(
    std::vector<std::uint8_t>& buffer,
    int requestedLength,
    int& transferredLength,
    unsigned int timeoutMs
) {
    transferredLength = 0;

    if (!isOpen()) {
        std::cerr << "bulkRead failed: device is not open\n";
        return false;
    }

    if (requestedLength <= 0) {
        std::cerr << "bulkRead failed: invalid requested length\n";
        return false;
    }

    buffer.assign(
        static_cast<std::size_t>(requestedLength),
        0
    );

    const int result = libusb_bulk_transfer(
        handle,
        BULK_ENDPOINT_IN,
        buffer.data(),
        requestedLength,
        &transferredLength,
        timeoutMs
    );

    if (result != LIBUSB_SUCCESS) {
        std::cerr
            << "Bulk read failed: "
            << libusb_error_name(result)
            << ", transferred="
            << transferredLength
            << '\n';

        buffer.clear();
        return false;
    }

    buffer.resize(
        static_cast<std::size_t>(transferredLength)
    );

    return true;
}