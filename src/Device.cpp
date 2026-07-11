#include "Device.h"

#include <iostream>

namespace {

constexpr std::uint16_t VENDOR_ID = 0x04B4;
constexpr std::uint16_t PRODUCT_ID = 0x6022;

constexpr int INTERFACE_NUMBER = 0;

// Host -> Device, Vendor, Device recipient
constexpr std::uint8_t REQUEST_TYPE_VENDOR_OUT = 0x40;

} // namespace


Device::Device() = default;


Device::~Device() {
    close();
}


bool Device::open() {
    // Не открываем устройство второй раз.
    if (handle != nullptr) {
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
        VENDOR_ID,
        PRODUCT_ID
    );

    if (handle == nullptr) {
        std::cerr
            << "Hantek DSO-6022BE not found "
            << "(VID=0x04B4, PID=0x6022)"
            << '\n';

        libusb_exit(context);
        context = nullptr;

        return false;
    }

    std::cout << "Hantek found\n";

    // Если устройство неожиданно занято драйвером ядра,
    // пробуем его временно отсоединить.
    const int kernelDriverActive =
        libusb_kernel_driver_active(handle, INTERFACE_NUMBER);

    if (kernelDriverActive == 1) {
        std::cout << "Detaching kernel driver...\n";

        const int detachResult =
            libusb_detach_kernel_driver(handle, INTERFACE_NUMBER);

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
        libusb_claim_interface(handle, INTERFACE_NUMBER);

    if (claimResult != LIBUSB_SUCCESS) {
        std::cerr
            << "Failed to claim USB interface "
            << INTERFACE_NUMBER
            << ": "
            << libusb_error_name(claimResult)
            << '\n';

        close();
        return false;
    }

    interfaceClaimed = true;

    std::cout << "USB opened\n";

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
            << "controlWrite failed: USB device is not open"
            << '\n';

        return false;
    }

    // libusb_control_transfer принимает неконстантный указатель,
    // хотя для OUT-запроса библиотека данные не изменяет.
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
            << "Control transfer failed"
            << " request=0x"
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
            << " bytes, transferred "
            << transferred
            << '\n';

        return false;
    }

    std::cout
        << "Control transfer OK"
        << " request=0x"
        << std::hex
        << static_cast<int>(request)
        << std::dec
        << ", bytes="
        << transferred
        << '\n';

    return true;
}