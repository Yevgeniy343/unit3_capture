#include "httplib.h"
#include "nlohmann/json.hpp"

#include "src/Device.h"
#include "src/Capture.h"

#include <iostream>

using json = nlohmann::json;

int main() {

    Device device;

    if (!device.open()) {
        std::cerr << "Cannot open Hantek\n";
        return 1;
    }

    Capture capture(device);

    httplib::Server server;

    server.set_compress(true);

    server.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });

    server.Get("/capture",
        [&](const httplib::Request&, httplib::Response& res) {

            CaptureFrame frame;

            if (!capture.read(frame)) {

                res.status = 500;

                res.set_content(
                    R"({"error":"capture failed"})",
                    "application/json"
                );

                return;
            }

            json j;

            j["sampleRate"] = frame.sampleRate;
            j["points"] = frame.ch1.size();
            j["ch1"] = frame.ch1;
            j["ch2"] = frame.ch2;

            res.set_content(
                j.dump(),
                "application/json"
            );
        }
    );

    std::cout << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << " Hantek HTTP Server started" << std::endl;
    std::cout << " http://127.0.0.1:8080" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << std::endl;

    server.listen("0.0.0.0", 8080);

    device.close();

    return 0;
}