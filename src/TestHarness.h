#ifndef STELL_TEST
#define STELL_TEST

#include <string>
#include <map>

namespace Stellarium
{

/**
 * @brief A simple unit test class.
 */
class Test
{
    public:
        Test(const std::string& name) : _name(name) {};
        ~Test() { summarize(); };
        void assertTrue(const std::string& description, bool condition);
        void summarize();

    private:
        std::string _name {};
        std::map<std::string, bool> _results {};

        void bold(const std::string& text);
        void red(const std::string& text);
        void green(const std::string& text);
        void boldGreen(const std::string& text);
        void boldRed(const std::string& text);

    protected:
};

} // end namespace Stellarium

#endif // end STELL_TEST