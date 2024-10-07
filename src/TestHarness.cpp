#include "TestHarness.h"

#include <iomanip>
#include <iostream>
// #include <iomanip>
#include <string>
#include <utility>


namespace Stellarium {


void Test::assertTrue(const std::string& description, bool condition)
{
    _results.push_back(std::make_pair(description, condition));
}

void Test::summarize()
{
    unsigned int passed = 0;
    unsigned int failed = 0;
    const int width = 50;

    std::cout << bold("\nRunning test: " + _name + "\n");
    
    for (auto& result : _results)
    {
        if (result.second)
        {
            passed++;

            if (_verbose)
            {
                std::cout 
                    << std::left
                    << std::setw(width)
                    << green("\t" + result.first + ":")
                    << std::right
                    << boldGreen("PASS\n");
            }
        }
        else
        {
            failed++;

            std::cout
                << std::left
                << std::setw(width)
                << red("\t" + result.first + ":")
                << std::right
                << boldRed("FAIL\n");
        }
    }
    int total = passed + failed;
    if (failed == 0)
    {
        std::cout << boldGreen("SUMMARY: PASSED (" + std::to_string(passed) + " of " + std::to_string(total) + ")\n");
    }
    else
    {
        std::cout << boldRed("SUMMARY: FAILED (" + std::to_string(failed) + " of " + std::to_string(passed+failed) + ") tests failed\n");
    }
}

std::string Test::bold(const std::string& text)
{
    return "\033[1m" + text + "\033[0m";
}

std::string Test::red(const std::string& text)
{
    return "\033[31m" + text + "\033[0m";
}

std::string Test::green(const std::string& text)
{
    return  "\033[32m" + text + "\033[0m";
}

std::string Test::boldGreen(const std::string& text)
{
    return "\033[01;32m" + text + "\033[0m";
}

std::string Test::boldRed(const std::string& text)
{
    return "\033[01;31m" + text + "\033[0m";
}

}