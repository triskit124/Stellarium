#ifndef STELL_TEST
#define STELL_TEST

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
        void assertEquals(const std::string& description, double a, double b, double epsilon = 0.0);
        
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
        bool _verbose { false };
        size_t _description_width { 0 };

        std::string bold(const std::string& text);
        std::string red(const std::string& text);
        std::string green(const std::string& text);
        std::string boldGreen(const std::string& text);
        std::string boldRed(const std::string& text);

    protected:
};

} // end namespace Stellarium

#endif // end STELL_TEST