#include <execution>
#include <vector>

#include "bitonicsortcontrol.hpp"

constexpr auto FACTOR = 15;
constexpr auto TEST_NUMBER = 7;

BitonicSortControl::BitonicSortControl() {
    // clang-format off
    const std::vector<TypeModule> allowedTypes({
        {Type::CHAR,   TEST_NUMBER},
        {Type::INT,    TEST_NUMBER},
        {Type::FLOAT,  TEST_NUMBER},
        {Type::DOUBLE, TEST_NUMBER}
    });
    // clang-format on

    m_allowedTypes = allowedTypes;
}

void BitonicSortControl::generateTests() const {
    // Flush test file content
    std::ofstream testFile;
    testFile.open(m_availableTests, std::ofstream::out | std::ofstream::trunc);
    testFile.close();

    const auto generator = [this](TypeModule const &config) {
        const auto singleGen = [=](unsigned const &testSize) mutable {
            switch (config.type) {
            case Type::CHAR:
                generateTest<char>("char", testSize);
                break;
            case Type::INT:
                generateTest<int>("int", testSize);
                break;
            case Type::FLOAT:
                generateTest<float>("float", testSize);
                break;
            case Type::DOUBLE:
                generateTest<double>("double", testSize);
                break;
            default:
                std::cerr << "Unhandled Type!" << std::endl;
                exit(1);
            }
        };

        std::vector<unsigned> sizeVec(config.testNum);
        for (size_t i = 0; i < config.testNum; ++i) {
            if (i == 0) {
                sizeVec[i] = 1;
            } else {
                sizeVec[i] = sizeVec[i - 1] * FACTOR;
            }
        }
        std::for_each(std::execution::par, sizeVec.begin(), sizeVec.end(),
                      singleGen);
    };

    std::for_each(std::execution::par, m_allowedTypes.begin(),
                  m_allowedTypes.end(), generator);
};
