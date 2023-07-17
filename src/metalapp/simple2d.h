//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <memory>

namespace MTL
{
class Device;
class RenderCommandEncoder;
} // namespace MTL

//
//
//
class Simple2D
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Simple2D();
    virtual ~Simple2D();

    void initialize(MTL::Device* dev, float width, float height);
    void finalize();

    void setupRender(MTL::RenderCommandEncoder* enc);
    void render(MTL::RenderCommandEncoder* enc);
    void clearDraw();
    void setDrawColor(float red, float green, float blue, float alpha);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawRect(float x1, float y1, float x2, float y2);
};
