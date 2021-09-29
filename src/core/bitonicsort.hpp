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

    const std::vector<T> &m_input;
    std::vector<T> &m_output;

    const sycl::device m_device;
    const size_t m_inSize;

  public:
    BitonicSort(const std::vector<T> &input, std::vector<T> &output,
                const sycl::device_selector &device = sycl::default_selector());
    BitonicSort(const BitonicSort &) = delete;

    void Run();

  private:
    void RunInParallel();
};

template <typename T>
inline BitonicSort<T>::BitonicSort(const std::vector<T> &input,
                                   std::vector<T> &output,
                                   const sycl::device_selector &device)
    : m_input(input), m_output(output), m_device(sycl::device(device)),
      m_inSize(input.size()) {
    assert(m_inSize != 0);
    assert(m_input.size() == m_output.size());
}

template <typename T> inline void BitonicSort<T>::Run() {
    m_output = m_input;

    // If array size is not 2^n, add values to the array
    // to have 2^n elements.
    size_t dataSize = m_output.size();
    if ((dataSize & (dataSize - 1)) != 0) {
        T maxVal = std::numeric_limits<T>::max();
        size_t newSize = std::pow(2, std::ceil(std::log(dataSize) / log(2)));
        for (size_t i = 0; i < newSize - dataSize; ++i) {
            m_output.push_back(maxVal);
        }
    }

    std::cout << "DEVICE: " << m_device.get_info<sycl::info::device::name>()
              << std::endl;
    RunInParallel();
}

template <typename T> inline void BitonicSort<T>::RunInParallel() {
    ull numSteps = std::ceil(std::log(m_output.size()) / log(2));
    auto range = sycl::range(m_output.size());
    auto ndRange = sycl::nd_range<>(range, range);

    sycl::buffer<T, 1> buff(m_output.data(), m_output.size());
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
