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
#include <simd/matrix_types.h>
#include <simd/quaternion.h>
#include <simd/simd.h>
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
}

//
void
Camera::update(int frameIndex)
{
    auto* buffer = impl_->buffers_[frameIndex];

    auto* camera                 = reinterpret_cast<CameraData*>(buffer->contents());
    camera->perspectiveTransform = impl_->perspective_;
    camera->worldTransform       = math::makeIdentity();
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
}

//
void
Camera::setTargetPosition(simd::float3 tgt)
{
}

//
void
Camera::setUpVector(simd::float3 up)
{
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
