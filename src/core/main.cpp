#include <vector>
#include "bitonicsort.hpp"

#define SIZE 16

int main() {
    std::vector<int> in(SIZE);
    std::vector<int> out(SIZE);

    auto sort = BitonicSort<int>(in, out, sycl::cpu_selector());
    sort.Run();

    for (auto it: out) {
        std::cout << it << std::endl;
    }
}
