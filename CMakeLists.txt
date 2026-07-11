cmake_minimum_required(VERSION 3.16)

project(capture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

add_executable(
    capture
    main.cpp
    src/Device.cpp
    src/Capture.cpp
)

target_include_directories(
    capture
    PRIVATE
        .
        ${LIBUSB_INCLUDE_DIRS}
)

target_link_directories(
    capture
    PRIVATE
        ${LIBUSB_LIBRARY_DIRS}
)

target_compile_options(
    capture
    PRIVATE
        ${LIBUSB_CFLAGS_OTHER}
)

target_link_libraries(
    capture
    PRIVATE
        ${LIBUSB_LIBRARIES}
)