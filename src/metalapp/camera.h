//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <camera_interface.h>
#include <cinttypes>
#include <memory>
#include <simd/simd.h>

namespace MTL
{
class Device;
class Buffer;
} // namespace MTL

//
//
//
class Camera : public CameraInterface
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Camera();
    ~Camera() override;

    void initialize(MTL::Device* dev, int keepFrame);
    void update(int frameIndex);
    void release();

    void setEyePosition(simd::float3 eye) override;
    void setTargetPosition(simd::float3 tgt) override;
    void setUpVector(simd::float3 up) override;
    void setIdentity() override;

    void setViewport(float fovy, float aspect, float znear, float zfar) override;

    MTL::Buffer* getCameraBuffer();
};

//
