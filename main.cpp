#include "src/Device.h"

#include <iostream>

int main() {

    Device device;

    if (!device.open()) {
        return 1;
    }

    std::cout << "Everything OK\n";

    return 0;
}