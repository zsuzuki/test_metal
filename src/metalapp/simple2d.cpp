//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "Metal/MTLDepthStencil.hpp"
#include "Metal/MTLDevice.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLResource.hpp"
#include "shaderset.h"
#include "simple2d.h"
#include <cstddef>
#include <iostream>
#include <list>
#include <memory>
#include <simd/simd.h>

namespace
{

//
struct ScreenData
{
    simd::float2 size;
};

} // namespace

//
//
//
struct Simple2D::Impl
{
    MTL::Device*            device_    = nullptr;
    MTL::Buffer*            scrBuffer_ = nullptr;
    MTL::DepthStencilState* dsState_   = nullptr;
    ShaderSet               shader_;
    ScreenData              scrData_;

    ~Impl() { shader_.release(); }
    void initialize(MTL::Device* dev, float width, float height)
    {
        device_       = dev;
        scrData_.size = {width, height};
        scrBuffer_    = dev->newBuffer(&scrData_, sizeof(scrData_), MTL::ResourceOptionCPUCacheModeDefault);
        shader_.load(dev, "shader/simple2d.metal", "vert2d", "frag2d", true);

        auto* dsDesc = MTL::DepthStencilDescriptor::alloc()->init();
        dsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
        dsDesc->setDepthWriteEnabled(true);
        dsState_ = dev->newDepthStencilState(dsDesc);
        dsDesc->release();
    }
    void finalize()
    {
        if (scrBuffer_)
        {
            scrBuffer_->release();
            scrBuffer_ = nullptr;
        }
        if (dsState_)
        {
            dsState_->release();
            dsState_ = nullptr;
        }
        device_ = nullptr;
    }
    void setup(MTL::RenderCommandEncoder* enc)
    {
        enc->setRenderPipelineState(shader_.getRenderPipelineState());
        enc->setDepthStencilState(dsState_);

        enc->setCullMode(MTL::CullModeBack);
        enc->setFrontFacingWinding(MTL::Winding::WindingClockwise);
        enc->setVertexBuffer(scrBuffer_, 0, 1);
    }
};

//
//
//
Simple2D::Simple2D() : impl_(std::make_unique<Impl>()) {}

//
//
//
Simple2D::~Simple2D() { finalize(); }

//
//
//
void
Simple2D::initialize(MTL::Device* dev, float width, float height)
{
    impl_->initialize(dev, width, height);
}

//
//
//
void
Simple2D::finalize()
{
    impl_->finalize();
}

//
//
//
void
Simple2D::setupRender(MTL::RenderCommandEncoder* enc)
{
    impl_->setup(enc);
}

//
