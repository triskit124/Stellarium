#include "TextRenderer.h"
#include "Matrix44.h"
#include "Vector4.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#include <fstream>
#include <iostream>

namespace Stellarium
{

namespace
{
    // Maps framebuffer pixel space (origin top-left, y down -- matching the coordinates
    // stb_truetype's baked quads are generated in) directly to clip space.
    Matrix44 screenProjection(float width, float height)
    {
        Vector4 x { 2.0 / width, 0.0, 0.0, -1.0 };
        Vector4 y { 0.0, -2.0 / height, 0.0, 1.0 };
        Vector4 z { 0.0, 0.0, -1.0, 0.0 };
        Vector4 w { 0.0, 0.0, 0.0, 1.0 };
        return Matrix44(x, y, z, w);
    }
}

TextRenderer::TextRenderer(const std::string& fontPath, const std::string& vertexShaderPath,
                            const std::string& fragmentShaderPath, float pixelHeight)
    : _pixelHeight(pixelHeight)
{
    _shader = std::make_unique<Shader>(vertexShaderPath, fragmentShaderPath);

    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file)
    {
        std::cout << "Failed to open font at path: " << fontPath << std::endl;
        return;
    }
    std::vector<unsigned char> fontBuffer(static_cast<size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(fontBuffer.data()), fontBuffer.size());

    std::vector<unsigned char> atlasBitmap(ATLAS_SIZE * ATLAS_SIZE);
    _charData.resize(NUM_CHARS);
    int result = stbtt_BakeFontBitmap(fontBuffer.data(), 0, _pixelHeight, atlasBitmap.data(),
                                       ATLAS_SIZE, ATLAS_SIZE, FIRST_CHAR, NUM_CHARS, _charData.data());
    if (result <= 0)
    {
        std::cout << "Glyph atlas too small to bake font: " << fontPath << std::endl;
    }

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    // stb_truetype's bitmap is tightly packed 1 byte/pixel; GL defaults to 4-byte row
    // alignment and will misread rows whose width isn't a multiple of 4.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, ATLAS_SIZE, ATLAS_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, atlasBitmap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer()
{
    glDeleteTextures(1, &_texture);
    glDeleteBuffers(1, &_VBO);
    glDeleteVertexArrays(1, &_VAO);
}

void TextRenderer::drawText(const std::string& text, float x, float y, const Vector3& color,
                             int fbWidth, int fbHeight, float scale)
{
    struct GlyphVertex { float x, y, u, v; };
    std::vector<GlyphVertex> verts;
    verts.reserve(text.size() * 6);

    // Lay the string out at the baked pixel height with the pen starting at the origin,
    // then scale the whole layout (glyph size *and* inter-glyph advances) around (x, y)
    // when emitting vertices below -- scaling post-hoc per glyph would keep the small
    // baked advances while blowing up glyph size, causing overlap.
    float cursorX = 0.0f, cursorY = 0.0f;
    for (unsigned char c : text)
    {
        if (c < FIRST_CHAR || c >= FIRST_CHAR + NUM_CHARS)
            continue;

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(_charData.data(), ATLAS_SIZE, ATLAS_SIZE, c - FIRST_CHAR, &cursorX, &cursorY, &q, 1);

        auto emit = [&](float qx, float qy, float qu, float qv) {
            verts.push_back({x + qx * scale, y + qy * scale, qu, qv});
        };
        emit(q.x0, q.y0, q.s0, q.t0);
        emit(q.x1, q.y0, q.s1, q.t0);
        emit(q.x1, q.y1, q.s1, q.t1);
        emit(q.x0, q.y0, q.s0, q.t0);
        emit(q.x1, q.y1, q.s1, q.t1);
        emit(q.x0, q.y1, q.s0, q.t1);
    }

    if (verts.empty())
        return;

    _shader->use();
    _shader->setMat4("projection", screenProjection(static_cast<float>(fbWidth), static_cast<float>(fbHeight)));
    _shader->setVec3("textColor", color);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    _shader->setInt("glyphAtlas", 0);

    // Alpha-blend the glyph coverage over whatever's already drawn, and skip depth
    // testing entirely -- text is always an on-top screen-space overlay here.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glBindVertexArray(_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(verts.size() * sizeof(GlyphVertex)), verts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size()));
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

}
