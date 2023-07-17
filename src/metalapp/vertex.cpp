//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include "Foundation/NSString.hpp"
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "vertex.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <simd/simd.h>
#include <vector>

namespace
{
// 座標データ生成管理用
struct PointData
{
    simd::float3 pos{};
    simd::float2 uv{};

    PointData() = default;
    PointData(float x, float y, float z, float u, float v) : pos{x, y, z}, uv{u, v} {}
};

// 座標データ描画用
struct VertexData
{
    simd::float3 position{};
    simd::float3 normal{};
    simd::float2 texcoord{};

    VertexData() = default;
    VertexData(simd::float3 p, simd::float3 n, simd::float2 t) : position(p), normal(n), texcoord(t) {}

    bool operator==(const VertexData& other)
    {
        for (int i = 0; i < 3; i++)
        {
            if (position[i] != other.position[i] || normal[i] != other.normal[i])
            {
                return false;
            }
        }
        return texcoord[0] == other.texcoord[0] && texcoord[1] == other.texcoord[1];
    }
};

} // namespace

struct Vertex::Impl
{
    std::vector<PointData>  pointList_;
    std::vector<VertexData> vertexList_;
    std::vector<uint16_t>   indices_;
    MTL::Buffer*            vertexBuffer_ = nullptr;
    MTL::Buffer*            indexBuffer_  = nullptr;
    std::uintptr_t          nbIndices_    = 0;

    //
    void searchAndPush(VertexData vd)
    {
        for (size_t i = 0; i < vertexList_.size(); i++)
        {
            if (vertexList_[i] == vd)
            {
                indices_.push_back(i);
                return;
            }
        }
        indices_.push_back(vertexList_.size());
        vertexList_.emplace_back(vd);
    }

    // 三角形追加
    void pushTriangle(int p0, int p1, int p2)
    {
        auto& pd0 = pointList_[p0];
        auto& pd1 = pointList_[p1];
        auto& pd2 = pointList_[p2];

        auto dir0 = pd1.pos - pd0.pos;
        auto dir1 = pd2.pos - pd1.pos;
        auto norm = simd::cross(dir0, dir1);
        norm      = simd::normalize(norm);

        searchAndPush({pd0.pos, norm, pd0.uv});
        searchAndPush({pd1.pos, norm, pd1.uv});
        searchAndPush({pd2.pos, norm, pd2.uv});
    }

    // バッファ生成
    void build(MTL::Device* dev)
    {
        auto vsize    = vertexList_.size() * sizeof(VertexData);
        auto isize    = indices_.size() * sizeof(uint16_t);
        vertexBuffer_ = dev->newBuffer(vsize, MTL::ResourceStorageModeManaged);
        indexBuffer_  = dev->newBuffer(isize, MTL::ResourceStorageModeManaged);

        std::memcpy(vertexBuffer_->contents(), vertexList_.data(), vsize);
        std::memcpy(indexBuffer_->contents(), indices_.data(), isize);
        nbIndices_ = indices_.size();

        vertexBuffer_->didModifyRange(NS::Range::Make(0, vertexBuffer_->length()));
        indexBuffer_->didModifyRange(NS::Range::Make(0, indexBuffer_->length()));
    }

    //
    void release()
    {
        if (vertexBuffer_)
        {
            vertexBuffer_->release();
            vertexBuffer_ = nullptr;
        }
        if (indexBuffer_)
        {
            indexBuffer_->release();
            vertexBuffer_ = nullptr;
        }
    }
};

//
Vertex::Vertex() : impl_(std::make_unique<Impl>()) {}

//
Vertex::~Vertex() { release(); }

//
void
Vertex::reserve(size_t num)
{
    impl_->pointList_.reserve(num);
}

//
void
Vertex::release()
{
    impl_->release();
}

//
void
Vertex::build(MTL::Device* dev)
{
    impl_->build(dev);
}

//
int
Vertex::pushPoint(float x, float y, float z, float u, float v)
{
    int idx = impl_->pointList_.size();
    impl_->pointList_.push_back({x, y, z, u, v});
    return idx;
}

//
void
Vertex::pushTriangle(int p0, int p1, int p2)
{
    impl_->pushTriangle(p0, p1, p2);
}

//
MTL::Buffer*
Vertex::getVertexBuffer()
{
    return impl_->vertexBuffer_;
}

//
MTL::Buffer*
Vertex::getIndexBuffer()
{
    return impl_->indexBuffer_;
}

//
std::uintptr_t
Vertex::getIndexCount() const
{
    return impl_->nbIndices_;
}

//
