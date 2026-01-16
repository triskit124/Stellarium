#pragma once

#include "Vector2.h"
#include "Vector3.h"
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
        unsigned int getId() const { return ID; }
        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;
        void setVec2(const std::string &name, const Vector2& value) const;
        void setVec3(const std::string &name, const Vector3& value) const;
        void setMat4(const std::string &name, const Matrix44& value) const;

    private:
        unsigned int ID;
};

}
