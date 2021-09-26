#pragma once
#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

#include <CL/sycl.hpp>

template <typename T> class BitonicSort {
    const std::vector<T> &m_input;
    const std::vector<T> &m_gold;
    std::vector<T> &m_output;

    const sycl::device m_device;
    const size_t m_inSize;

  public:
    BitonicSort(const std::vector<T> &input, std::vector<T> &output,
                const sycl::device_selector &device = sycl::default_selector(),
                const std::vector<T> &gold = std::vector<T>());
    BitonicSort(const BitonicSort &) = delete;

    void Run();

  private:
    void RunInParallel();
};

template <typename T>
inline BitonicSort<T>::BitonicSort(const std::vector<T> &input,
                                   std::vector<T> &output,
                                   const sycl::device_selector &device,
                                   const std::vector<T> &gold)
    : m_input(input), m_output(output), m_device(sycl::device(device)),
      m_gold(gold), m_inSize(input.size()) {
    assert(m_inSize != 0);
    assert(m_input.size() == m_output.size());
    if (!m_gold.empty()) {
        assert(m_input.size() == m_gold.size());
    }
}

template <typename T> inline void BitonicSort<T>::Run() {
    m_output = m_input;

    // If array size is not 2^n, add values to the array
    // to have 2^n elements.
    if ((m_inSize & (m_inSize - 1)) != 0) {
        T maxVal = std::numeric_limits<T>::max();
        size_t newSize = std::pow(2, std::ceil(std::log(m_inSize) / log(2)));
        for (size_t i = 0; i < newSize - m_inSize; ++i) {
            m_output.push_back(maxVal);
        }
    }
    RunInParallel();
}

template <typename T> inline void BitonicSort<T>::RunInParallel() {
    std::cout << "DEVICE: " << m_device.get_info<sycl::info::device::name>()
              << std::endl;

    auto range = sycl::range(m_output.size());
    sycl::buffer buf(m_output.data(), range);
    sycl::queue q(m_device);

    q.submit([&](auto &h) {
        sycl::accessor arr(buf, h, sycl::read_write);
        h.parallel_for(range, [=](cl::sycl::id<1> wiID) {;
        });
    });
}
