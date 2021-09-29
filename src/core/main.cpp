#include <vector>
#include "bitonicsort.hpp"

#define SIZE 17

int main() {
    std::vector<int> in = {7, 16, 5 , 8, 9, 10, 11, 13, 8, 4, 5, 9, 15, 13, 0, 1, 1};
    std::vector<int> out(SIZE);

    auto sort = BitonicSort<int>(in, out, sycl::gpu_selector());
    sort.Run();

    for (auto it: out) {
        std::cout << it << std::endl;
    }
}
