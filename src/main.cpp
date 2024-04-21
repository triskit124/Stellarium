#include <iostream>

#include "body.h"
#include "math.h"

int main()
{
    Stellarium::Body vehicle = Stellarium::Body("vehicle");

    Stellarium::Vector3 v1 {1, 0, 0};
    Stellarium::Vector3 v2 {0, 1, 0};
    Stellarium::Vector3 v3 =  v1 * 5;

    std::cout << v3[0] << " " << v3[1] << " " << v3[2] << std::endl;
    return 0;
}