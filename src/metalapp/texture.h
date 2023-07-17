//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <string>

namespace MTL
{
class Device;
class Texture;
} // namespace MTL

//
//
//
class Texture
{
    MTL::Texture* tex_    = nullptr;
    uint16_t      width_  = 0;
    uint16_t      height_ = 0;

  public:
    Texture() = default;
    virtual ~Texture();

    void release();

    bool loadFromMemory(MTL::Device* dev, uint8_t* buffer, uint32_t width, uint32_t height);
    bool loadFromJPG(MTL::Device* dev, std::string path, uint32_t toW, uint32_t toH);
    bool loadFromPNG(MTL::Device* dev, std::string path, uint32_t toW, uint32_t toH);

    struct StringDesc
    {
        std::string fontName{"ヒラギノ角ゴシック"};
        std::string message;
        float       size  = 20.0f;
        float       red   = 1.0f;
        float       green = 1.0f;
        float       blue  = 1.0f;
        float       alpha = 1.0f;
    };
    bool buildByString(MTL::Device* dev, const StringDesc& strdesc);

    [[nodiscard]] uint16_t      getWidth() const { return width_; }
    [[nodiscard]] uint16_t      getHeight() const { return height_; }
    [[nodiscard]] MTL::Texture* get() { return tex_; }
};

//
