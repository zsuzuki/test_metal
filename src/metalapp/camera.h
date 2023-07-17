//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <memory>
#include <simd/simd.h>
#include <simd/vector_types.h>

namespace MTL
{
class Device;
class Buffer;
} // namespace MTL

//
//
//
class Camera
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Camera();
    virtual ~Camera();

    void initialize(MTL::Device* dev, int keepFrame);
    void update(int frameIndex);
    void release();

    void setEyePosition(simd::float3 eye);
    void setTargetPosition(simd::float3 tgt);
    void setUpVector(simd::float3 up);
    void setIdentity();

    void setViewport(float fovy, float aspect, float znear, float zfar);

    MTL::Buffer* getCameraBuffer();
};

//
