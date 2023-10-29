//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <AppKit/AppKit.hpp>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "Metal/MTLDevice.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLResource.hpp"
#include "shaderset.h"
#include "simple3d.h"
#include <memory>
#include <simd/simd.h>
#include <simd/vector_types.h>
#include <vector>

namespace
{
constexpr size_t maxVertex   = 20000;
constexpr size_t maxTriangle = 10000;

//
struct PrimData3D
{
    simd::float3 position;
    simd::float4 color;
};

} // namespace

struct Simple3D::Impl
{
    MTL::Device*            device_ = nullptr;
    ShaderSet               shader_;
    std::vector<PrimData3D> primitiveList_;
    std::vector<PrimData3D> triangleList_;
    simd::float4            drawColor_{1.0f, 1.0f, 1.0f, 1.0f};

    ~Impl() { shader_.release(); }

    //
    void initialize(MTL::Device* dev)
    {
        device_ = dev;
        shader_.load(dev, "shader/prim3d.metal", "primVert3d", "primFrag3d", true);

        primitiveList_.reserve(maxVertex);
        primitiveList_.resize(0);
        triangleList_.reserve(maxTriangle * 3);
        triangleList_.resize(0);
    }
    //
    void finalize() { device_ = nullptr; }
    //
    void setDrawColor(float red, float green, float blue, float alpha) { drawColor_ = simd_make_float4(red, green, blue, alpha); }
    //
    void clear()
    {
        primitiveList_.resize(0);
        triangleList_.resize(0);
    }
    //
    void render(MTL::RenderCommandEncoder* enc)
    {
        bool setupShader = false;
        if (!primitiveList_.empty())
        {
            enc->setRenderPipelineState(shader_.getRenderPipelineState());
            setupShader = true;

            auto* buff = device_->newBuffer(primitiveList_.data(), primitiveList_.size() * sizeof(PrimData3D),
                                            MTL::ResourceOptionCPUCacheModeDefault);
            enc->setVertexBuffer(buff, 0, 0);
            enc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeLine, 0, primitiveList_.size(), 1);
            buff->release();
        }
        if (!triangleList_.empty())
        {
            if (!setupShader)
            {
                enc->setRenderPipelineState(shader_.getRenderPipelineState());
            }

            auto* buff = device_->newBuffer(triangleList_.data(), triangleList_.size() * sizeof(PrimData3D),
                                            MTL::ResourceOptionCPUCacheModeDefault);
            enc->setVertexBuffer(buff, 0, 0);
            enc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 0, triangleList_.size(), 1);
            buff->release();
        }
    }
    //
    void drawLine(simd::float3 from, simd::float3 to)
    {
        PrimData3D p3d_f;
        PrimData3D p3d_t;
        p3d_f.position = from;
        p3d_f.color    = drawColor_;
        p3d_t.position = to;
        p3d_t.color    = drawColor_;
        primitiveList_.emplace_back(p3d_f);
        primitiveList_.emplace_back(p3d_t);
    }
    //
    void drawTriangle(simd::float3 v0, simd::float3 v1, simd::float3 v2)
    {
        PrimData3D p3d0;
        PrimData3D p3d1;
        PrimData3D p3d2;
        p3d0.position = v0;
        p3d0.color    = drawColor_;
        p3d1.position = v1;
        p3d1.color    = drawColor_;
        p3d2.position = v2;
        p3d2.color    = drawColor_;
        triangleList_.emplace_back(p3d0);
        triangleList_.emplace_back(p3d1);
        triangleList_.emplace_back(p3d2);
    }
};

//
//
//
Simple3D::Simple3D() : impl_(std::make_unique<Impl>()) {}

//
//
//
Simple3D::~Simple3D() { finalize(); }

//
void
Simple3D::initialize(MTL::Device* dev)
{
    impl_->initialize(dev);
}

//
void
Simple3D::finalize()
{
    impl_->finalize();
}

//
void
Simple3D::setDrawColor(float red, float green, float blue, float alpha)
{
    impl_->setDrawColor(red, green, blue, alpha);
}

//
void
Simple3D::clearDraw()
{
    impl_->clear();
}

//
void
Simple3D::render(MTL::RenderCommandEncoder* enc)
{
    impl_->render(enc);
}

//
void
Simple3D::drawLine(simd::float3 from, simd::float3 to)
{
    impl_->drawLine(from, to);
}

//
void
Simple3D::drawRect(simd::float3 p0, simd::float3 p1, simd::float3 p2, simd::float3 p3)
{
    impl_->drawLine(p0, p1);
    impl_->drawLine(p1, p2);
    impl_->drawLine(p2, p3);
    impl_->drawLine(p3, p0);
}

//
void
Simple3D::drawTriangle(simd::float3 v0, simd::float3 v1, simd::float3 v2)
{
    impl_->drawTriangle(v0, v1, v2);
}

//
void
Simple3D::drawPlane(simd::float3 v0, simd::float3 v1, simd::float3 v2, simd::float3 v3)
{
    impl_->drawTriangle(v0, v1, v2);
    impl_->drawTriangle(v0, v2, v3);
}

//
