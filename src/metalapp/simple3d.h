//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <memory>
#include <simd/vector_types.h>

namespace MTL
{
class Device;
class RenderCommandEncoder;
} // namespace MTL

//
//
//
class Simple3D
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Simple3D();
    virtual ~Simple3D();

    void initialize(MTL::Device* dev);
    void finalize();

    void render(MTL::RenderCommandEncoder* enc);
    void clearDraw();
    void setDrawColor(float red, float green, float blue, float alpha);
    void drawLine(simd::float3 from, simd::float3 to);
    void drawRect(simd::float3 p0, simd::float3 p1, simd::float3 p2, simd::float3 p3);
    void drawTriangle(simd::float3 v0, simd::float3 v1, simd::float3 v2);
    void drawPlane(simd::float3 v0, simd::float3 v1, simd::float3 v2, simd::float3 v3);
};
