//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "textdraw.h"
#include "texture.h"
#include <array>
#include <iostream>
#include <list>
#include <simd/simd.h>

namespace
{

//
struct DrawBuffer
{
    Texture      tex;
    float        x;
    float        y;
    simd::float4 col;
};

} // namespace

struct TextDraw::Impl
{
    MTL::Device*          device_ = nullptr;
    Texture::StringDesc   strdesc_{};
    simd::float4          color_{1.0f, 1.0f, 1.0f, 1.0f};
    std::list<DrawBuffer> drawList_{};

    //
    void print(float x, float y, const char* msg)
    {
        drawList_.push_back({});
        auto& tdb        = drawList_.back();
        strdesc_.message = msg;
        tdb.tex.buildByString(device_, strdesc_);
        tdb.x = x;
        tdb.y = y;
    }
};

//
//
//
TextDraw::TextDraw() : impl_(std::make_unique<Impl>()) {}

//
//
//
TextDraw::~TextDraw() {}

//
//
//
void
TextDraw::initialize(MTL::Device* dev)
{
    impl_->device_ = dev;
}

//
//
//
void
TextDraw::setFontName(std::string fname)
{
    impl_->strdesc_.fontName = fname;
}

//
//
//
void
TextDraw::setSize(float size)
{
    impl_->strdesc_.size = size;
}

//
//
//
void
TextDraw::setColor(float red, float green, float blue, float alpha)
{
    impl_->color_ = {red, green, blue, alpha};
}

//
//
//
void
TextDraw::render(MTL::RenderCommandEncoder* enc)
{
    struct V2D
    {
        simd::float2 p;
        simd::float4 c;
        simd::float2 t;
    };

    for (auto& tdb : impl_->drawList_)
    {
        float lx = tdb.x;
        float ty = tdb.y;
        float rx = lx + tdb.tex.getWidth();
        float by = ty + tdb.tex.getHeight();

        std::array<V2D, 4> varray;
        varray[0].c = varray[1].c = varray[2].c = varray[3].c = impl_->color_;

        varray[0].p[0] = lx;
        varray[0].p[1] = ty;
        varray[0].t    = {0.0, 0.0};
        varray[1].p[0] = rx;
        varray[1].p[1] = ty;
        varray[1].t    = {1.0, 0.0};
        varray[2].p[0] = lx;
        varray[2].p[1] = by;
        varray[2].t    = {0.0, 1.0};
        varray[3].p[0] = rx;
        varray[3].p[1] = by;
        varray[3].t    = {1.0, 1.0};
        auto* buff     = impl_->device_->newBuffer(varray.data(), sizeof(varray), MTL::ResourceOptionCPUCacheModeDefault);

        enc->setVertexBuffer(buff, 0, 0);
        enc->setFragmentTexture(tdb.tex.get(), 0);
        enc->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangleStrip, 0, 4, 1);

        buff->release();
    }
}

//
//
//
void
TextDraw::clear()
{
    impl_->drawList_.clear();
}

//
//
//
void
TextDraw::print(float x, float y, const char* msg)
{
    impl_->print(x, y, msg);
}

//
