#include <vector>
#include <algorithm>

#include "bitonicsortcontrol.hpp"

int main(int argc, char *argv[]) {
    BitonicSortControl bitonic;
    bitonic.GenerateTests();
    //if (argc == 2) {
    //    std::ifstream infile(argv[1]);
    //    if (!infile.is_open()) {
    //        std::cerr << "Cannot open: " << argv[1] << std::endl;
    //        exit(1);
    //    }

    //    std::vector<int> in = {7, 16, 5, 8,  9,  10, 11, 13, 8,
    //                           4, 5,  9, 15, 13, 0,  1,  1};
    //    const sycl::device Cpu((sycl::cpu_selector()));
    //    const sycl::device Gpu((sycl::gpu_selector()));
    //    auto sort = BitonicSort<int>(in, Cpu);

    //    for (auto it : in) {
    //        std::cout << it << std::endl;
    //    }
    //}
    //std::cerr << "Cannot open: " << argv[1] << std::endl;
    //exit(1);
}
