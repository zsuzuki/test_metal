//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <memory>

namespace MTL
{
class Device;
class Buffer;
} // namespace MTL

//
//
//
class Vertex
{
    struct Impl;
    std::unique_ptr<Impl> impl_;

  public:
    Vertex();
    virtual ~Vertex();

    // 予定超点数確保
    void reserve(size_t num);

    // 頂点追加 @return 頂点番号
    int pushPoint(float x, float y, float z, float u, float v);

    // 三角形作成
    void pushTriangle(int p0, int p1, int p2);
    // 四角形(三角形x2)
    void pushSqure(int p0, int p1, int p2, int p3)
    {
        pushTriangle(p0, p1, p2);
        pushTriangle(p2, p3, p0);
    }

    //
    void build(MTL::Device* dev);

    //
    void release();

    MTL::Buffer* getVertexBuffer();
    MTL::Buffer* getIndexBuffer();

    [[nodiscard]] std::uintptr_t getIndexCount() const;
};

//
