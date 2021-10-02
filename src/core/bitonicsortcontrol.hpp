#include "bitonicsort.hpp"
#include <chrono>

class BitonicSortControl {
    const std::string m_availableTests = "..\\tests\\tests.txt";
    const std::string m_testDir = "..\\tests";
    const sycl::device m_cpu;
    const sycl::device m_gpu;
  public:
    enum class Type { INVALID, CHAR, INT, FLOAT, DOUBLE };
    struct TypeModule {
        const Type type;
        const char *typeName;
        unsigned testNum = 0;
    };

    BitonicSortControl();
    void GenerateTests() const;
    void RunTests() const;

  private:
    template <typename T>
    void GenerateTestValues(std::ofstream &testFile,
                            const unsigned &testNum) const;
    template <typename T>
    void CheckAndVerify(const std::vector<T> &data, const sycl::device &device) const;
    template <typename T>
    void RunTestStdTy(std::ifstream &testFile, const unsigned &size) const;
    void ProcessTest(std::ifstream &testFile) const;
};

template <typename T>
inline void
BitonicSortControl::GenerateTestValues(std::ofstream &testFile,
                                       const unsigned &count) const {}

template <typename T>
inline void BitonicSortControl::CheckAndVerify(const std::vector<T> &data,
                                               const sycl::device& device) const {
    std::vector<T> arrayToSort(data);

    auto start = std::chrono::high_resolution_clock::now();
    if (device == m_cpu) {
        std::cout << "DEVICE: "
                  << "std::sort" << std::endl;
        std::sort(arrayToSort.begin(), arrayToSort.end());
    } else {
        std::cout << "DEVICE: " << device.get_info<sycl::info::device::name>()
                  << std::endl;
        BitonicSort(arrayToSort, device);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto mSec =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::chrono::duration<double> Sec = end - start;
    std::cout << "Execution time: " << Sec.count() << " seconds, " << mSec.count() << " milliseconds"
              << std::endl;

    bool isFailed = false;
    for (unsigned i = 1; i < arrayToSort.size(); ++i) {
        if (arrayToSort[i] < arrayToSort[i - 1]) {
            isFailed = true;
            break;
        }
    }

    std::cout << "Status: ";
    if (isFailed) {
        std::cout << "FAIL" << std::endl;
    } else {
        std::cout << "PASS" << std::endl;
    }
}

template <typename T>
inline void BitonicSortControl::RunTestStdTy(std::ifstream &testFile,
                                             const unsigned &size) const {
    std::vector<T> elems;
    elems.reserve(size);

    T tmp;
    while (testFile >> tmp) {
        elems.push_back(tmp);
    }

    CheckAndVerify(elems, m_cpu);
    CheckAndVerify(elems, m_gpu);
}
