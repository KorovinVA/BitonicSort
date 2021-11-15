#include "bitonicsort.hpp"
#include <chrono>

class BitonicSortControl {
    std::string m_availableTests{};
    std::string m_testDir{};
    sycl::device m_gpu{};

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
    void ProcessTest(const std::string &testName) const;

  private:
    template <typename T>
    void GenerateTestValues(std::ofstream &testFile,
                            const unsigned &testNum) const;
    template <typename T>
    void CheckAndVerify(const std::vector<T> &data, bool isStdSort) const;
    template <typename T>
    void RunTestStdTy(std::ifstream &testFile, const unsigned &size) const;
};

template <typename T>
inline void
BitonicSortControl::GenerateTestValues(std::ofstream &testFile,
                                       const unsigned &count) const {}

template <typename T>
inline void BitonicSortControl::CheckAndVerify(const std::vector<T> &data,
                                               bool isStdSort) const {
    std::vector<T> arrayToSort(data);

    auto start = std::chrono::high_resolution_clock::now();
    auto sizeBefore = arrayToSort.size();
    if (isStdSort) {
        std::cout << "DEVICE: "
                  << "std::sort" << std::endl;
        std::sort(arrayToSort.begin(), arrayToSort.end());
    } else {
        std::cout << "DEVICE: " << m_gpu.get_info<sycl::info::device::name>()
                  << std::endl;
        BitonicSort(arrayToSort, m_gpu);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto mSec =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::chrono::duration<double> Sec = end - start;
    std::cout << "Execution time: " << Sec.count() << " seconds ("
              << mSec.count() << " milliseconds)" << std::endl;

    bool isFailed = false;
    for (unsigned i = 1; i < arrayToSort.size(); ++i) {
        if (arrayToSort[i] < arrayToSort[i - 1]) {
            isFailed = true;
            break;
        }
    }
    if (arrayToSort.size() != sizeBefore) {
        isFailed = true;
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

    CheckAndVerify(elems, true);
    CheckAndVerify(elems, false);
}
