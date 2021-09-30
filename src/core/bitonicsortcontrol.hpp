#include "bitonicsort.hpp"

#include <limits>
#include <map>
#include <random>

class BitonicSortControl {
    enum class Type { CHAR, INT, FLOAT, DOUBLE };
    struct TypeModule {
        Type type = Type::CHAR;
        unsigned testNum = 0;
    };

    std::vector<TypeModule> m_allowedTypes;
    const std::string m_availableTests = "..\\tests\\tests.txt";
    const std::string m_testDir = "..\\tests";

  public:
    BitonicSortControl();
    void generateTests() const;

  private:
    template <typename T>
    void generateTest(const std::string &typeName,
                      const unsigned &testNum) const;
};

template <typename T>
inline void BitonicSortControl::generateTest(const std::string &typeName,
                                             const unsigned &count) const {
    std::string testName = typeName + "__" + std::to_string(count);
    std::string fileName = m_testDir + "\\" + testName;
    std::ofstream file;
    file.open(fileName);
    file << typeName << " " << count << "\n";

    std::random_device rd;
    std::default_random_engine gen(rd());
    if constexpr (std::is_same<char, T>::value) {
        std::uniform_int_distribution<int> distr(
            std::numeric_limits<T>::lowest(),
            std::numeric_limits<T>::max());
        for (unsigned i = 0; i < count; ++i) {
            file << distr(gen) << " ";
        }
    }
    else if constexpr (std::is_floating_point<T>::value) {
        std::uniform_real_distribution<T> distr(
            std::numeric_limits<T>::lowest() / 2,
            std::numeric_limits<T>::max() / 2);
        for (unsigned i = 0; i < count; ++i) {
            file << distr(gen) << " ";
        }
    } else {
        std::uniform_int_distribution<T> distr(
            std::numeric_limits<T>::lowest() / 2,
            std::numeric_limits<T>::max() / 2);
        for (unsigned i = 0; i < count; ++i) {
            file << distr(gen) << " ";
        }
    }

    file << "\n";
    file.close();

    std::ofstream testFile;
    testFile.open(m_availableTests, std::ios_base::app);
    testFile << testName << "\n";
    testFile.close();
}