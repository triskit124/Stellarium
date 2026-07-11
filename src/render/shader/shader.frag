#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform bool hasDiffuseTexture;
uniform vec3 diffuseColor;

void main()
{
    if (hasDiffuseTexture)
    {
        FragColor = texture(texture_diffuse1, TexCoords);
    }
    else
    {
        FragColor = vec4(diffuseColor, 1.0);
    }
}