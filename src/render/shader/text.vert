#version 330 core
layout (location = 0) in vec4 aVertex; // xy = position in framebuffer pixels, zw = atlas uv

out vec2 TexCoord;

uniform mat4 projection;

void main()
{
    TexCoord = aVertex.zw;
    gl_Position = projection * vec4(aVertex.xy, 0.0, 1.0);
}
