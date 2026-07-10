cmake_minimum_required(VERSION 3.16)

project(capture)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

include_directories(${LIBUSB_INCLUDE_DIRS})

add_executable(
    capture

    main.cpp
    src/Device.cpp
)

target_link_libraries(
    capture

    ${LIBUSB_LIBRARIES}
)