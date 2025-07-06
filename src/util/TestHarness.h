#ifndef STELL_TEST
#define STELL_TEST

#include <iostream>
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
        Test(const std::string& name) : _name(name) {
            std::cout << bold("\nRunning test: " + _name) << std::endl;
        };
        ~Test() { summarize(); };
        void assertTrue(const std::string& description, bool condition);
        void summarize();

    private:
        std::string _name {};
        std::vector<std::pair<std::string, bool>> _results {};
        bool _verbose = false;

        std::string bold(const std::string& text);
        std::string red(const std::string& text);
        std::string green(const std::string& text);
        std::string boldGreen(const std::string& text);
        std::string boldRed(const std::string& text);

    protected:
};

} // end namespace Stellarium

#endif // end STELL_TEST