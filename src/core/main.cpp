#include <algorithm>
#include <iostream>
#include <vector>

#include "bitonicsortcontrol.hpp"

// clang-format off
const static std::vector<std::string> Arguments({
        "-generate",
        "-run-all",
        "-run=",
        "-platforminfo"
    });
// clang-format on

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Please, use one of the following arguments:" << std::endl;
        for (auto it : Arguments) {
            std::cout << " " << it << std::endl;
        }
        exit(1);
    }
    std::string arg(argv[1]);
    for (auto it : Arguments) {
        if (arg.rfind(it, 0) == 0) {
            arg = it;
            break;
        }
    }

    if (arg == "-generate") {
        BitonicSortControl bSortCtrl;
        bSortCtrl.GenerateTests();
    } else if (arg == "-run-all") {
        BitonicSortControl bSortCtrl;
        bSortCtrl.RunTests();
    } else if (arg == "-run=") {
        // Get the part after '='
        std::string testName(argv[1]);
        testName = testName.substr(testName.find("=") + 1);

        BitonicSortControl bSortCtrl;
        bSortCtrl.ProcessTest(testName);
    } else if (arg == "-platforminfo") {
        std::cout << "Available devices and platforms:" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        auto platforms = sycl::platform::get_platforms();
        for (auto &platform : platforms) {
            std::cout << "Platform: "
                      << platform.get_info<sycl::info::platform::name>()
                      << std::endl;
            auto devices = platform.get_devices();
            for (auto &device : devices) {
                std::cout << "  Device: "
                          << device.get_info<sycl::info::device::name>()
                          << std::endl;
            }
        }
        std::cout << "--------------------------------" << std::endl;
    } else {
        std::cout << "Argument \"" << arg << "\" is no valid!" << std::endl;
    }
}
