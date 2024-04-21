#ifndef STELL_BODY
#define STELL_BODY

#include <array>
#include <string>

#include "math.h"

namespace Stellarium 
{

class Body
{    
    public:
        Body(const std::string& name);
        ~Body() {};

        std::string name() const { return _name; };
        void name(const std::string& name) { _name = name; };

        double mass() const { return _mass; };
        void mass(double mass) { _mass = mass; };

        Vector3 cm() const { return _cm; };
        void cm(const Vector3& cm) { _cm = cm; };

        Matrix33 inertia() const { return _inertia; };
        void inertia(const Matrix33& inertia) { _inertia = inertia; };
    
    protected:
        std::string _name;
        double _mass = 1.0;
        Vector3 _cm {0.0, 0.0, 0.0};
        Matrix33 _inertia = {{1.0, 0.0, 0.0} ,{0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
        Vector3 _pos {0.0, 0.0, 0.0};
        Vector3 _vel {0.0, 0.0, 0.0};
        Vector3 _acc {0.0, 0.0, 0.0};
        Quaternion _att {0.0, 0.0, 0.0, 1.0};

    private:
};

} // end namespace Stellarium

#endif // end STELL_BODY