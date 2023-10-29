//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include "camera_interface.h"
#include <cinttypes>
#include <simd/vector_types.h>
#include <string>
#include <type_traits>

//
//
//
class Context
{

  public:
    Context()          = default;
    virtual ~Context() = default;

    // get 3d camera control
    virtual CameraInterface& GetCamera() = 0;
    // set draw color
    virtual void SetDrawColor(float r, float g, float b, float a = 1.0f) = 0;
    // display message on screen
    virtual void Print(float x, float y, std::string msg) = 0;
    //
    virtual void DrawLine2D(simd::float2 from, simd::float2 to) = 0;
    //
    virtual void DrawRect2D(simd::float2 pos, simd::float2 size) = 0;
    //
    virtual void DrawLine3D(simd::float3 from, simd::float3 to) = 0;
    //
    virtual void DrawRect3D(simd::float3 p0, simd::float3 p1, simd::float3 p2, simd::float3 p3) = 0;
    //
    virtual void DrawTriangle3D(simd::float3 v0, simd::float3 v1, simd::float3 v2) = 0;
    //
    virtual void DrawPlane3D(simd::float3 v0, simd::float3 v1, simd::float3 v2, simd::float3 v3) = 0;
};
