//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include "Foundation/NSString.hpp"
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "Metal/MTLBuffer.hpp"
#include "camera.h"
#include <algorithm>
#include <iostream>
#include <matrix.h>
#include <simd/geometry.h>
#include <simd/matrix.h>
#include <simd/matrix_types.h>
#include <simd/quaternion.h>
#include <simd/simd.h>
#include <simd/vector_make.h>
#include <simd/vector_types.h>
#include <vector>

namespace
{
//
struct CameraData
{
    simd::float4x4 perspectiveTransform;
    simd::float4x4 worldTransform;
    simd::float3x3 worldNormalTransform;
};

} // namespace

//
//
//
struct Camera::Impl
{
    std::vector<MTL::Buffer*> buffers_;
    simd::float4x4            perspective_;
    simd::float4x4            view_;
    MTL::Buffer*              readBuffer_;

    simd::float3 eyePosition_;
    simd::float3 targetPosition_;
    simd::float3 upVector_;
};

//
//
//
Camera::Camera() : impl_(std::make_unique<Impl>()) {}

//
Camera::~Camera() { release(); }

//
void
Camera::initialize(MTL::Device* dev, int keepFrame)
{
    impl_->buffers_.resize(keepFrame);
    for (auto& buff : impl_->buffers_)
    {
        buff = dev->newBuffer(sizeof(CameraData), MTL::ResourceStorageModeManaged);
    }
    impl_->view_           = math::makeIdentity();
    impl_->eyePosition_    = simd_make_float3(0.0f, 0.0f, 10.0f);
    impl_->targetPosition_ = simd_make_float3(0.0f, 0.0f, 0.0f);
    impl_->upVector_       = simd_make_float3(0.0f, 1.0f, 0.0f);
}

//
void
Camera::update(int frameIndex)
{
    auto& eyePos    = impl_->eyePosition_;
    auto  targetVec = impl_->targetPosition_ - eyePos;
    auto  frontVec  = simd_normalize(targetVec);
    auto  upVec     = simd_normalize(impl_->upVector_);
    auto  sideVec   = simd_normalize(simd_cross(upVec, frontVec));
    upVec           = simd_normalize(simd_cross(frontVec, sideVec));

    simd::float4x4 viewMtx;
    viewMtx.columns[0] = simd_make_float4(sideVec[0], sideVec[1], sideVec[2], 0.0f);
    viewMtx.columns[1] = simd_make_float4(upVec[0], upVec[1], upVec[2], 0.0f);
    viewMtx.columns[2] = simd_make_float4(frontVec[0], frontVec[1], frontVec[2], 0.0f);
    viewMtx.columns[3] = simd_make_float4(eyePos[0], eyePos[1], eyePos[2], 1.0f);
    viewMtx.columns[3] = simd_mul(viewMtx, viewMtx.columns[3]);

    auto* buffer = impl_->buffers_[frameIndex];

    auto* camera                 = reinterpret_cast<CameraData*>(buffer->contents());
    camera->perspectiveTransform = impl_->perspective_;
    camera->worldTransform       = viewMtx;
    camera->worldNormalTransform = math::discardTranslation(camera->worldTransform);

    buffer->didModifyRange(NS::Range::Make(0, sizeof(CameraData)));

    impl_->readBuffer_ = buffer;
}

//
void
Camera::release()
{
    for (auto& buff : impl_->buffers_)
    {
        if (buff)
        {
            buff->release();
            buff = nullptr;
        }
    }
}

//
void
Camera::setEyePosition(simd::float3 eye)
{
    impl_->eyePosition_ = eye;
}

//
void
Camera::setTargetPosition(simd::float3 tgt)
{
    impl_->targetPosition_ = tgt;
}

//
void
Camera::setUpVector(simd::float3 up)
{
    impl_->upVector_ = up;
}

//
void
Camera::setIdentity()
{
}

//
void
Camera::setViewport(float fovy, float aspect, float znear, float zfar)
{
    impl_->perspective_ = math::makePerspective(fovy, aspect, znear, zfar);
}

//
MTL::Buffer*
Camera::getCameraBuffer()
{
    return impl_->readBuffer_;
}

//
