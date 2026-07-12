#pragma once

#include "Shader.h"
#include "Vector3.h"
#include <glad/glad.h>
#include <stb_truetype.h>

#include <memory>
#include <string>
#include <vector>

namespace Stellarium
{

// Bakes a TrueType font into a single-channel glyph atlas (via stb_truetype) and draws
// strings as screen-space quads. One instance per font/size.
class TextRenderer
{
    public:

        TextRenderer(const std::string& fontPath, const std::string& vertexShaderPath,
                     const std::string& fragmentShaderPath, float pixelHeight = 32.0f);
        ~TextRenderer();

        // Draws text with (x, y) as the baseline origin in framebuffer pixels (origin at the
        // top-left of the window, y down -- so ascenders extend to y values *less* than y).
        // fbWidth/fbHeight must be the current framebuffer size so the orthographic
        // projection matches the viewport. scale multiplies the glyph size and spacing
        // around (x, y); 1.0 draws at the atlas's baked pixelHeight. Since the atlas is a
        // fixed-resolution bitmap, scale much above ~1.5-2.0 will look blurry -- for
        // reliably crisp larger text, construct a second TextRenderer with a bigger
        // pixelHeight instead.
        void drawText(const std::string& text, float x, float y, const Vector3& color,
                      int fbWidth, int fbHeight, float scale = 1.0f);

    private:

        static constexpr int ATLAS_SIZE = 512;
        static constexpr int FIRST_CHAR = 32;
        static constexpr int NUM_CHARS = 96; // ASCII 32..127

        std::unique_ptr<Shader> _shader;
        unsigned int _texture = 0;
        unsigned int _VAO = 0, _VBO = 0;
        std::vector<stbtt_bakedchar> _charData;
        float _pixelHeight;
};

}
