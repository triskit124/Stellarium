#ifndef STELL_SHADER
#define STELL_SHADER

#include "Matrix44.h"
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Stellarium
{

class Shader
{
    public:
        Shader(const std::string& vertexPath, const std::string& fragmentPath);
        void use();
        void remove();
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setMat4(const std::string &name, const Matrix44& value) const;

    private:
        unsigned int ID;
};

}

#endif // STELL_SHADER