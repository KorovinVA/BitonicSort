#include <execution>
#include <limits>
#include <random>

#include "bitonicsortcontrol.hpp"

constexpr auto FACTOR = 15;
constexpr auto TEST_NUMBER = 6;

// clang-format off
const static std::vector<BitonicSortControl::TypeModule>
    allowedTypes({
        {BitonicSortControl::Type::CHAR,    "char",   TEST_NUMBER},
        {BitonicSortControl::Type::INT,     "int",    TEST_NUMBER},
        {BitonicSortControl::Type::FLOAT,   "float",  TEST_NUMBER},
        {BitonicSortControl::Type::DOUBLE,  "double", TEST_NUMBER}
    });
// clang-format on

template <>
void BitonicSortControl::GenerateTestValues<char>(std::ofstream &testFile,
                                                  const unsigned &count) const {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> distr(
        std::numeric_limits<char>::lowest(), std::numeric_limits<char>::max());
    for (unsigned i = 0; i < count; ++i) {
        testFile << distr(gen) << " ";
    }
}

template <>
void BitonicSortControl::GenerateTestValues<int>(std::ofstream &testFile,
                                                 const unsigned &count) const {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::uniform_int_distribution<int> distr(
        std::numeric_limits<int>::lowest() / 2,
        std::numeric_limits<int>::max() / 2);
    for (unsigned i = 0; i < count; ++i) {
        testFile << distr(gen) << " ";
    }
}

template <>
void BitonicSortControl::GenerateTestValues<float>(
    std::ofstream &testFile, const unsigned &count) const {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::normal_distribution<float> distr(0.0f);
    for (unsigned i = 0; i < count; ++i) {
        testFile << distr(gen) << " ";
    }
}

template <>
void BitonicSortControl::GenerateTestValues<double>(
    std::ofstream &testFile, const unsigned &count) const {
    std::random_device rd;
    std::default_random_engine gen(rd());
    std::normal_distribution<double> distr(0.0f);
    for (unsigned i = 0; i < count; ++i) {
        testFile << distr(gen) << " ";
    }
}

BitonicSortControl::BitonicSortControl()
    : m_cpu((sycl::cpu_selector())), m_gpu(((sycl::gpu_selector()))) {}

void BitonicSortControl::GenerateTests() const {
    // Flush test file content
    std::ofstream config;
    config.open(m_availableTests, std::ofstream::out | std::ofstream::trunc);
    config.close();

    const auto generator = [this](TypeModule const &config) {
        const auto singleGen = [=](unsigned const &testSize) mutable {
            std::string testName =
                std::string(config.typeName) + "__" + std::to_string(testSize);
            std::string fileName = m_testDir + "\\" + testName;
            std::ofstream test;

            test.open(fileName);
            test << config.typeName << " " << testSize << "\n";
            switch (config.type) {
            case Type::CHAR:
                GenerateTestValues<char>(test, testSize);
                break;
            case Type::INT:
                GenerateTestValues<int>(test, testSize);
                break;
            case Type::FLOAT:
                GenerateTestValues<float>(test, testSize);
                break;
            case Type::DOUBLE:
                GenerateTestValues<double>(test, testSize);
                break;
            default:
                std::cerr << "Unhandled Type!" << std::endl;
                test.close();
                exit(1);
            }
            test << "\n";
            test.close();

            std::ofstream config;
            config.open(m_availableTests, std::ios_base::app);
            config << testName + "\n";
            config.close();
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

    std::for_each(std::execution::par, allowedTypes.begin(), allowedTypes.end(),
                  generator);
}

void BitonicSortControl::RunTests() const {
    std::ifstream config(m_availableTests);
    if (config.is_open()) {
        std::vector<std::string> tests;
        std::string line;
        while (std::getline(config, line)) {
            tests.push_back(line);
        }

        // Run tests for the same type sequentially
        std::sort(tests.begin(), tests.end());

        for (auto it : tests) {
            if (it.empty()) {
                continue;
            }

            auto testName = m_testDir + "\\" + it;
            std::ifstream test(testName);
            if (test.is_open()) {
                std::cout << "Running " << it << std::endl;
                std::cout << "--------------------------------------------------------" << std::endl;
                ProcessTest(test);
                std::cout << "--------------------------------------------------------\n" << std::endl;
            } else {
                std::cerr << "Cannot open " << testName << std::endl;
            }
        }
        config.close();
    } else {
        std::cerr << "Cannot open " << m_availableTests << std::endl;
        exit(1);
    }
}

void BitonicSortControl::ProcessTest(std::ifstream &testFile) const {
    std::string typeName, sizeString;
    unsigned valueCount = 0;
    Type type = Type::INVALID;

    testFile >> typeName >> sizeString;
    valueCount = std::stoul(sizeString);
    for (auto it : allowedTypes) {
        if (typeName == it.typeName) {
            type = it.type;
            break;
        }
    }

    switch (type) {
    case Type::CHAR:
        RunTestStdTy<char>(testFile, valueCount);
        break;
    case Type::INT:
        RunTestStdTy<int>(testFile, valueCount);
        break;
    case Type::FLOAT:
        RunTestStdTy<float>(testFile, valueCount);
        break;
    case Type::DOUBLE:
        RunTestStdTy<double>(testFile, valueCount);
        break;
    default:
        std::cerr << "Type is not supported: " << typeName << std::endl;
        exit(1);
    }
};
