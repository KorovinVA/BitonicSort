#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

#include <CL/sycl.hpp>

template <typename T> class BitonicSort {
    static constexpr auto MAX_GROUP_SIZE = 256;

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
    int numSteps = std::ceil(std::log(m_input.size()) / log(2));
    auto range = sycl::range(m_input.size());

    sycl::buffer<T, 1> dataBuff(m_input.data(), m_input.size());
    sycl::queue q(m_device);
    for (int i = 1; i <= numSteps; ++i) {
        for (int j = i; j > 0; --j) {
            int callDepth = i - j;
            int nodeHeight = j;
            q.submit([&](auto &h) {
                sycl::accessor data(dataBuff, h, sycl::read_write);

                h.parallel_for(range, [=](cl::sycl::id<1> wiID) {
                    int globalID = wiID;
                    int numWarpsInCmp = 1 << (nodeHeight - 1);
                    int numWarpsInSeq = numWarpsInCmp << (callDepth + 1);

                    bool isWarpComparing = (globalID / numWarpsInCmp) % 2 == 0;
                    bool isIncreasingCmp =
                        isWarpComparing &&
                        ((globalID / numWarpsInSeq) % 2 == 0);

                    if (isWarpComparing) {
                        if (isIncreasingCmp) {
                            if (data[globalID + numWarpsInCmp] <
                                data[globalID]) {
                                T temp = data[globalID];
                                data[globalID] = data[globalID + numWarpsInCmp];
                                data[globalID + numWarpsInCmp] = temp;
                            }

                        } else {
                            if (data[globalID] <
                                data[globalID + numWarpsInCmp]) {
                                T temp = data[globalID];
                                data[globalID] = data[globalID + numWarpsInCmp];
                                data[globalID + numWarpsInCmp] = temp;
                            }
                        }
                    }
                });
            });
        }
    }
}