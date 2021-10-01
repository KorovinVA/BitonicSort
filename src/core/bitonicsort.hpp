#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

#include <CL/sycl.hpp>

template <typename T> class BitonicSort {
    typedef unsigned long long ull;

    std::vector<T> &m_input;
    const size_t m_inSize;
    const sycl::device m_device;

  public:
    BitonicSort(std::vector<T> &input,
                const sycl::device device = sycl::device());
    BitonicSort(const BitonicSort &) = delete;

  private:
    void Run();
    void RunInParallel();
};

template <typename T>
inline BitonicSort<T>::BitonicSort(std::vector<T> &input,
                                   const sycl::device device)
    : m_input(input), m_inSize(input.size()), m_device(device) {
    if (m_inSize != 0)
        Run();
}

template <typename T> inline void BitonicSort<T>::Run() {
    // If input size is not 2^n, add values to have 2^n elements
    if ((m_inSize & (m_inSize - 1)) != 0) {
        T maxVal = std::numeric_limits<T>::max();
        size_t newSize = std::pow(2, std::ceil(std::log(m_inSize) / log(2)));
        for (size_t i = 0; i < newSize - m_inSize; ++i) {
            m_input.push_back(maxVal);
        }
    }

    RunInParallel();

    // Return old size after sorting.
    m_input.resize(m_inSize);
}

template <typename T> inline void BitonicSort<T>::RunInParallel() {
    ull numSteps = std::ceil(std::log(m_input.size()) / log(2));
    auto range = sycl::range(m_input.size());
    auto ndRange = sycl::nd_range<>(range, range);

    sycl::buffer<T, 1> buff(m_input.data(), m_input.size());
    sycl::buffer<ull, 1> sNum(&numSteps, 1);

    sycl::queue q(m_device);
    q.submit([&](auto &h) {
        sycl::accessor data(buff, h, sycl::read_write);
        sycl::accessor steps(sNum, h, sycl::read_only);

        h.parallel_for(ndRange, [=](cl::sycl::nd_item<1> wiID) {
            const auto BitonicStep = [&](ull callDepth, ull nodeHeight) {
                ull globalID = wiID.get_global_id();
                ull numWarpsInCmp = 1 << (nodeHeight - 1);
                ull numWarpsInSeq = numWarpsInCmp << (callDepth + 1);

                bool isWarpComparing = (globalID / numWarpsInCmp) % 2 == 0;
                bool isIncreasingCmp =
                    isWarpComparing && ((globalID / numWarpsInSeq) % 2 == 0);

                if (isWarpComparing) {
                    if (isIncreasingCmp) {
                        if (data[globalID + numWarpsInCmp] < data[globalID])
                            std::swap(data[globalID],
                                      data[globalID + numWarpsInCmp]);

                    } else {
                        if (data[globalID] < data[globalID + numWarpsInCmp])
                            std::swap(data[globalID],
                                      data[globalID + numWarpsInCmp]);
                    }
                }
            };

            for (ull i = 1; i <= steps[0]; ++i) {
                for (ull j = i; j > 0; --j) {
                    BitonicStep(i - j, j);
                    wiID.barrier();
                }
            }
        });
    });
}
