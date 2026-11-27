#pragma once

#include <cstdlib>
#include <exception>
#include <functional>
#include <string>
#include <vector>

namespace Stellarium
{

/**
 * @brief A simple unit test class.
 */
class Test
{
    public:

        Test(const std::string& name);
        ~Test() { summarize(); };

        void assertTrue(const std::string& description, bool condition);
        void assertEquals(const std::string& description, double actual, double expected, double abs_tol = 0.0, double rel_tol = 0.0);

        template <typename exception_type>
        void assertRaises(const std::string& description, std::function<void()> callable) {
            bool raises_correctly = false;
            try {
                callable();
            } catch (const exception_type&) {
                raises_correctly = true;
            }
            recordResult(description, raises_correctly);
        }

        void summarize();

        unsigned int getNumFails() { return _num_fails; };
        unsigned int getNumPasses() { return _num_passes; };

        bool getVerbose() { return _verbose; };
        void setVerbose(bool verbose) { _verbose = verbose; };

    private:
        std::string _name;
        unsigned int _num_fails { 0 };
        unsigned int _num_passes { 0 };
        std::vector<std::pair<std::string, bool>> _results { };
        bool _verbose { true };
        size_t _description_width { 0 };

        void recordResult(const std::string& description, bool pass);

        std::string bold(const std::string& text);
        std::string red(const std::string& text);
        std::string green(const std::string& text);
        std::string boldGreen(const std::string& text);
        std::string boldRed(const std::string& text);

    protected:
};

} // end namespace Stellarium
