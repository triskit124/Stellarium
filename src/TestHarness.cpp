#include "TestHarness.h"

#include <iostream>
#include <string>


namespace Stellarium {


void Test::assertTrue(const std::string& description, bool condition)
{
    _results[description] = condition;

}

void Test::summarize()
{
    unsigned int passed = 0;
    unsigned int failed = 0;

    bold("Running test: " + _name + "\n");
    for (auto& result : _results)
    {
        if (result.second)
        {
            passed++;
            green("\t" + result.first + ":\t");
            boldGreen("PASS\n");
        }
        else
        {
            failed++;
            red("\t" + result.first + ":\t");
            boldRed("FAIL\n");
        }
    }
    if (failed == 0)
    {
        boldGreen("SUMMARY: PASSED\n");
    }
    else
    {
        boldRed("SUMMARY: FAILED (" + std::to_string(failed) + " of " + std::to_string(passed+failed) + ") tests failed\n");
    }
}

void Test::bold(const std::string& text)
{
    std::cout << "\033[1m" << text << "\033[0m";
}

void Test::red(const std::string& text)
{
    std::cout << "\033[31m" << text << "\033[0m";
}

void Test::green(const std::string& text)
{
    std::cout << "\033[32m" << text << "\033[0m";
}

void Test::boldGreen(const std::string& text)
{
    std::cout << "\033[01;32m" << text << "\033[0m";
}

void Test::boldRed(const std::string& text)
{
    std::cout << "\033[01;31m" << text << "\033[0m";
}

}