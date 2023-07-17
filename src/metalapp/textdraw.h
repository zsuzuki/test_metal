//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <memory>
#include <string>

namespace MTL
{
class Device;
class RenderCommandEncoder;
} // namespace MTL

class TextDraw
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    TextDraw();
    virtual ~TextDraw();

    void initialize(MTL::Device* dev);

    void setFontName(std::string fname);
    void setSize(float size);
    void setColor(float red, float green, float blue, float alpha);

    void render(MTL::RenderCommandEncoder* enc);
    void clear();

    void print(float x, float y, const char* msg);

    template <class... Args>
    void printf(float x, float y, const char* fmt, Args&&... args)
    {
        std::array<char, 256> msg;
        snprintf(msg.data(), msg.size(), fmt, std::forward<Args>(args)...);
        print(x, y, msg.data());
    }
};

//
