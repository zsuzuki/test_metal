//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLResource.hpp"
#include "shaderset.h"
#include "simple2d.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <simd/simd.h>
#include <simd/vector_types.h>
#include <vector>

namespace
{
constexpr size_t maxVertex = 20000;

//
struct PrimData2D
{
    simd::float2 position;
    simd::float4 color;
};

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
    ShaderSet               primShader_;
    ScreenData              scrData_;
    std::vector<PrimData2D> primitiveList_;
    simd::float4            drawColor_{1.0f, 1.0f, 1.0f, 1.0f};

    ~Impl() { shader_.release(); }
    void initialize(MTL::Device* dev, float width, float height)
    {
        device_       = dev;
        scrData_.size = {width, height};
        scrBuffer_    = dev->newBuffer(&scrData_, sizeof(scrData_), MTL::ResourceOptionCPUCacheModeDefault);
        shader_.load(dev, "shader/simple2d.metal", "vert2d", "frag2d", true);
        primShader_.load(dev, "shader/prim2d.metal", "vert2d", "frag2d", true);

        auto* dsDesc = MTL::DepthStencilDescriptor::alloc()->init();
        dsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
        dsDesc->setDepthWriteEnabled(true);
        dsState_ = dev->newDepthStencilState(dsDesc);
        dsDesc->release();

        primitiveList_.reserve(maxVertex);
        primitiveList_.resize(0);
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
    void setDrawColor(float red, float green, float blue, float alpha) { drawColor_ = {red, green, blue, alpha}; }
    void setup(MTL::RenderCommandEncoder* enc)
    {
        enc->setRenderPipelineState(shader_.getRenderPipelineState());
        enc->setDepthStencilState(dsState_);

        enc->setCullMode(MTL::CullModeBack);
        enc->setFrontFacingWinding(MTL::Winding::WindingClockwise);
        enc->setVertexBuffer(scrBuffer_, 0, 1);
    }
    void clearDraw() { primitiveList_.resize(0); }
    //
    void render(MTL::RenderCommandEncoder* enc)
    {
        if (!primitiveList_.empty())
        {
            enc->setRenderPipelineState(primShader_.getRenderPipelineState());

            auto* buff = device_->newBuffer(primitiveList_.data(), primitiveList_.size() * sizeof(PrimData2D),
                                            MTL::ResourceOptionCPUCacheModeDefault);
            enc->setVertexBuffer(buff, 0, 0);
            enc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeLine, 0, primitiveList_.size(), 1);
            buff->release();
        }
    }
    //
    void drawLine(float x1, float y1, float x2, float y2)
    {
        PrimData2D p2d_s;
        PrimData2D p2d_e;
        p2d_s.position = {x1, y1};
        p2d_s.color    = drawColor_;
        p2d_e.position = {x2, y2};
        p2d_e.color    = drawColor_;
        primitiveList_.emplace_back(p2d_s);
        primitiveList_.emplace_back(p2d_e);
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
//
//
void
Simple2D::setDrawColor(float red, float green, float blue, float alpha)
{
    impl_->setDrawColor(red, green, blue, alpha);
}

//
//
//
void
Simple2D::clearDraw()
{
    impl_->clearDraw();
}

//
//
//
void
Simple2D::render(MTL::RenderCommandEncoder* enc)
{
    impl_->render(enc);
}

//
//
//
void
Simple2D::drawLine(float x1, float y1, float x2, float y2)
{
    impl_->drawLine(x1, y1, x2, y2);
}

//
//
//
void
Simple2D::drawRect(float x1, float y1, float x2, float y2)
{
    impl_->drawLine(x1, y1, x2, y1);
    impl_->drawLine(x2, y1, x2, y2);
    impl_->drawLine(x1, y1, x1, y2);
    impl_->drawLine(x1, y2, x2, y2);
}

//
