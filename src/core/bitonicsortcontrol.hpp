#include "bitonicsort.hpp"

class BitonicSortControl {
    const std::string m_availableTests = "..\\tests\\tests.txt";
    const std::string m_testDir = "..\\tests";

  public:
    enum class Type { CHAR, INT, FLOAT, DOUBLE };
    struct TypeModule {
        const Type type;
        const char *typeName;
        unsigned testNum = 0;
    };

    BitonicSortControl();
    void GenerateTests() const;

  private:
    template <typename T>
    void GenerateTestValues(std::ofstream &testFile,
                            const unsigned &testNum) const;
};

template <typename T>
inline void
BitonicSortControl::GenerateTestValues(std::ofstream &testFile,
                                       const unsigned &count) const {}
