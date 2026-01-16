#include "TestHarness.h"

#include <iostream>
#include <algorithm>
#include <iomanip>

namespace Stellarium {

Test::Test(const std::string& name)
    : _name(name) 
{
    std::cerr << bold("\nRunning test: " + _name) << std::endl;
};

void Test::assertTrue(const std::string& description, bool condition)
{
    _results.push_back({ description, condition } );
    _description_width = std::max(_description_width, description.length());

    if (condition)
    {
        _num_passes++;
    }
    else
    {
        _num_fails++;
    }

}

void Test::assertEquals(const std::string& description, double a, double b, double epsilon)
{
    assertTrue(description, abs(a - b) <= epsilon);
}

void Test::summarize()
{    
    for (auto& result : _results)
    {
        if (!result.second || _verbose)
        {
            std::string description = result.second ? green(result.first + ":") : red(result.first + ":");
            std::string report = result.second ? boldGreen("PASS\n") : boldRed("FAIL\n");

            std::cerr
                << std::setw(_description_width + 15)
                << std::left
                << description
                << std::setw(15)
                << std::right
                << report;
        }
    }
    
    if (_num_fails == 0)
    {
        std::cerr << boldGreen("SUMMARY: PASSED (" + std::to_string(_num_passes) + " of " + std::to_string(_num_passes + _num_fails) + ") tests passed \u2705 \n");
    }
    else
    {
        std::cerr << boldRed("SUMMARY: FAILED (" + std::to_string(_num_fails) + " of " + std::to_string(_num_passes + _num_fails) + ") tests failed \u274C \n");
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

} // namespace Stellarium