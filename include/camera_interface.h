//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <simd/simd.h>

//
//
//
class CameraInterface
{

  public:
    CameraInterface()          = default;
    virtual ~CameraInterface() = default;

    virtual void setEyePosition(simd::float3 eye)    = 0;
    virtual void setTargetPosition(simd::float3 tgt) = 0;
    virtual void setUpVector(simd::float3 up)        = 0;
    virtual void setIdentity()                       = 0;

    virtual void setViewport(float fovy, float aspect, float znear, float zfar) = 0;
};
