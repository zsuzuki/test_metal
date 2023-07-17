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
};
