/*
 *
 * Copyright 2022 Apple Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "metalapp/app.h"
#include "metalapp/camera.h"
#include "metalapp/shaderset.h"
#include "metalapp/simple2d.h"
#include "metalapp/textdraw.h"
#include "metalapp/texture.h"
#include "metalapp/vertex.h"
#include <cmath>
#include <iostream>
#include <matrix.h>
#include <memory>
#include <simd/simd.h>
#include <simd/vector_make.h>

static constexpr size_t kInstanceRows      = 10;
static constexpr size_t kInstanceColumns   = 10;
static constexpr size_t kInstanceDepth     = 10;
static constexpr size_t kNumInstances      = (kInstanceRows * kInstanceColumns * kInstanceDepth);
static constexpr size_t kMaxFramesInFlight = 3;
static constexpr float  ScreenWidth        = 1600.0f;
static constexpr float  ScreenHeight       = 1000.0f;

//
//
//
class Renderer : public DelegateLoop
{
  public:
    Renderer() = default;
    ~Renderer() override { finalize(); };
    void buildDepthStencilStates();
    void buildBuffers();

    const char* getTitle() const override { return "Metal Draw Test"; }
    void        initialize(MTL::Device* dev) override;
    void        finalize() override;
    void        draw(MTK::View* pView) override;
    float       getScreenWidth() const override { return ScreenWidth; }
    float       getScreenHeight() const override { return ScreenHeight; }

  private:
    MTL::Device*            _pDevice;
    MTL::CommandQueue*      _pCommandQueue;
    MTL::DepthStencilState* _pDepthStencilState;
    MTL::Buffer*            _pInstanceDataBuffer[kMaxFramesInFlight];
    Texture                 _texture;
    ShaderSet               _shaderSet;
    Vertex                  _vertex;
    Camera                  _camera;
    TextDraw                _textdraw;
    Simple2D                _render2d;
    float                   _angle = 0.0f;
    int                     _frame = 0;
    dispatch_semaphore_t    _semaphore;
    static const int        kMaxFramesInFlight;
};

const int Renderer::kMaxFramesInFlight = 3;

void
Renderer::initialize(MTL::Device* dev)
{
    _pDevice = dev;

    _pCommandQueue = _pDevice->newCommandQueue();
    _shaderSet.load(_pDevice, "shader/default.metal", "vertexMain", "fragmentMain", false);

    buildDepthStencilStates();

    _texture.loadFromJPG(_pDevice, "res/lake.jpg", 256, 256);

    buildBuffers();
    _camera.initialize(_pDevice, Renderer::kMaxFramesInFlight);
    auto aspect = ScreenWidth / ScreenHeight;
    _camera.setViewport(45.0f, aspect, 0.03f, 500.0f);

    _textdraw.initialize(_pDevice);
    _render2d.initialize(_pDevice, ScreenWidth, ScreenHeight);

    _semaphore = dispatch_semaphore_create(Renderer::kMaxFramesInFlight);
}

void
Renderer::finalize()
{
    _pDepthStencilState->release();
    for (int i = 0; i < kMaxFramesInFlight; ++i)
    {
        _pInstanceDataBuffer[i]->release();
    }
    _pCommandQueue->release();
    _camera.release();
    _vertex.release();
    _texture.release();
    _shaderSet.release();
    _render2d.finalize();
    _pDevice->release();
}

namespace shader_types
{
struct InstanceData
{
    simd::float4x4 instanceTransform;
    simd::float3x3 instanceNormalTransform;
    simd::float4   instanceColor;
};
} // namespace shader_types

void
Renderer::buildDepthStencilStates()
{
    MTL::DepthStencilDescriptor* pDsDesc = MTL::DepthStencilDescriptor::alloc()->init();
    pDsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
    pDsDesc->setDepthWriteEnabled(true);

    _pDepthStencilState = _pDevice->newDepthStencilState(pDsDesc);

    pDsDesc->release();
}

void
Renderer::buildBuffers()
{
    const float s = 0.5f;
    _vertex.reserve(6 * 4);
    _vertex.pushPoint(-s, -s, s, 0.0f, 1.0f);
    _vertex.pushPoint(s, -s, s, 1.0f, 1.0f);
    _vertex.pushPoint(s, s, s, 1.0f, 0.0f);
    _vertex.pushPoint(-s, s, s, 0.0f, 0.0f);
    _vertex.pushSqure(0, 1, 2, 3);
    _vertex.pushPoint(s, -s, s, 0.0f, 1.0f);
    _vertex.pushPoint(s, -s, -s, 1.0f, 1.0f);
    _vertex.pushPoint(s, s, -s, 1.0f, 0.0f);
    _vertex.pushPoint(s, s, s, 0.0f, 0.0f);
    _vertex.pushSqure(4, 5, 6, 7);
    _vertex.pushPoint(s, -s, -s, 0.0f, 1.0f);
    _vertex.pushPoint(-s, -s, -s, 1.0f, 1.0f);
    _vertex.pushPoint(-s, s, -s, 1.0f, 0.0f);
    _vertex.pushPoint(s, s, -s, 0.0f, 0.0f);
    _vertex.pushSqure(8, 9, 10, 11);
    _vertex.pushPoint(-s, -s, -s, 0.0f, 1.0f);
    _vertex.pushPoint(-s, -s, s, 1.0f, 1.0f);
    _vertex.pushPoint(-s, s, s, 1.0f, 0.0f);
    _vertex.pushPoint(-s, s, -s, 0.0f, 0.0f);
    _vertex.pushSqure(12, 13, 14, 15);
    _vertex.pushPoint(-s, s, s, 0.0f, 1.0f);
    _vertex.pushPoint(s, s, s, 1.0f, 1.0f);
    _vertex.pushPoint(s, s, -s, 1.0f, 0.0f);
    _vertex.pushPoint(-s, s, -s, 0.0f, 0.0f);
    _vertex.pushSqure(16, 17, 18, 19);
    _vertex.pushPoint(-s, -s, -s, 0.0f, 1.0f);
    _vertex.pushPoint(s, -s, -s, 1.0f, 1.0f);
    _vertex.pushPoint(s, -s, s, 1.0f, 0.0f);
    _vertex.pushPoint(-s, -s, s, 0.0f, 0.0f);
    _vertex.pushSqure(20, 21, 22, 23);
    _vertex.build(_pDevice);

    const size_t instanceDataSize = kMaxFramesInFlight * kNumInstances * sizeof(shader_types::InstanceData);
    for (size_t i = 0; i < kMaxFramesInFlight; ++i)
    {
        _pInstanceDataBuffer[i] = _pDevice->newBuffer(instanceDataSize, MTL::ResourceStorageModeManaged);
    }
}

void
Renderer::draw(MTK::View* pView)
{
    using simd::float3;
    using simd::float4;
    using simd::float4x4;

    auto* pPool = NS::AutoreleasePool::alloc()->init();

    _frame                           = (_frame + 1) % Renderer::kMaxFramesInFlight;
    MTL::Buffer* pInstanceDataBuffer = _pInstanceDataBuffer[_frame];

    auto* pCmd = _pCommandQueue->commandBuffer();
    dispatch_semaphore_wait(_semaphore, DISPATCH_TIME_FOREVER);
    Renderer* pRenderer = this;
    pCmd->addCompletedHandler(^void(MTL::CommandBuffer* pCmd) {
      dispatch_semaphore_signal(pRenderer->_semaphore);
    });

    _angle += 0.001f;

    const float scl           = 0.5f;
    auto*       pInstanceData = reinterpret_cast<shader_types::InstanceData*>(pInstanceDataBuffer->contents());

    float3 objectPosition = {0.f, 0.f, -10.f};

    float4x4 rt            = math::makeTranslate(objectPosition);
    float4x4 rr1           = math::makeYRotate(-_angle);
    float4x4 rr0           = math::makeXRotate(_angle * 0.5);
    float4x4 rtInv         = math::makeTranslate({-objectPosition.x, -objectPosition.y, -objectPosition.z});
    float4x4 fullObjectRot = rt * rr1 * rr0 * rtInv;

    size_t ix = 0;
    size_t iy = 0;
    size_t iz = 0;
    for (size_t i = 0; i < kNumInstances; ++i)
    {
        if (ix == kInstanceRows)
        {
            ix = 0;
            iy += 1;
        }
        if (iy == kInstanceRows)
        {
            iy = 0;
            iz += 1;
        }

        float4x4 scale = math::makeScale((float3){scl, scl, scl});
        float4x4 zrot  = math::makeZRotate(_angle * sinf((float)ix));
        float4x4 yrot  = math::makeYRotate(_angle * cosf((float)iy));

        float    x         = ((float)ix - (float)kInstanceRows / 3.f) * (3.f * scl) + scl;
        float    y         = ((float)iy - (float)kInstanceColumns / 3.f) * (3.f * scl) + scl;
        float    z         = ((float)iz - (float)kInstanceDepth / 3.f) * (3.f * scl);
        float4x4 translate = math::makeTranslate(math::add(objectPosition, {x, y, z}));

        pInstanceData[i].instanceTransform       = fullObjectRot * translate * yrot * zrot * scale;
        pInstanceData[i].instanceNormalTransform = math::discardTranslation(pInstanceData[i].instanceTransform);

        float iDivNumInstances         = i / (float)kNumInstances;
        float r                        = iDivNumInstances;
        float g                        = 1.0f - r;
        float b                        = sinf(M_PI * 2.0f * iDivNumInstances);
        pInstanceData[i].instanceColor = (float4){r, g, b, 1.0f};

        ix += 1;
    }
    pInstanceDataBuffer->didModifyRange(NS::Range::Make(0, pInstanceDataBuffer->length()));

    static int tcnt    = 0;
    auto       targetX = sin((tcnt / 360.0) * M_PI * 2.0) * 8.0f;
    auto       targetY = cos((tcnt / 360.0) * M_PI * 2.0) * 8.0f;
    auto       posZ    = sin((tcnt / 360.0) * M_PI * 2.0) * 2.0f;
    tcnt               = (tcnt + 3) % 360;
    auto targetPos     = simd::make_float3(targetX, targetY, 20.0f);
    _camera.setTargetPosition(targetPos);
    auto eyePos = simd_make_float3(0.0f, 0.0f, posZ - 1.0f);
    _camera.setEyePosition(eyePos);

    // Update camera state:
    _camera.update(_frame);

    // Begin render pass:

    MTL::RenderPassDescriptor* pRpd = pView->currentRenderPassDescriptor();
    MTL::RenderCommandEncoder* pEnc = pCmd->renderCommandEncoder(pRpd);

    pEnc->setRenderPipelineState(_shaderSet.getRenderPipelineState());
    pEnc->setDepthStencilState(_pDepthStencilState);

    pEnc->setCullMode(MTL::CullModeBack);
    pEnc->setFrontFacingWinding(MTL::Winding::WindingCounterClockwise);

    constexpr NS::UInteger VertexId   = 0;
    constexpr NS::UInteger InstanceId = 1;
    constexpr NS::UInteger CameraId   = 2;
    constexpr NS::UInteger offset     = 0;
    constexpr NS::UInteger TextureId0 = 0;
    pEnc->setVertexBuffer(_vertex.getVertexBuffer(), offset, VertexId);
    pEnc->setVertexBuffer(pInstanceDataBuffer, offset, InstanceId);
    pEnc->setVertexBuffer(_camera.getCameraBuffer(), offset, CameraId);
    pEnc->setFragmentTexture(_texture.get(), TextureId0);
    pEnc->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, _vertex.getIndexCount(),
                                MTL::IndexType::IndexTypeUInt16, _vertex.getIndexBuffer(), 0, kNumInstances);

    _render2d.setupRender(pEnc);
    _textdraw.setSize(40.0f);
    _textdraw.print(200, 400, "こんにちは世界");
    static int cnt = 0;
    _textdraw.printf(400, 600, "明日は晴れだ: %d", cnt++);
    _textdraw.render(pEnc);
    _textdraw.clear();

    for (int l = 0; l < 200; l++)
    {
        float theta = ((float)(cnt + l % 360) / 360) * M_PI * 2.0f;
        float x1    = sinf(theta) * 400 + 800;
        float y1    = cosf(theta) * 400 + 500;
        float x2    = 800;
        float y2    = 500;
        float r     = fabs(sinf(theta));
        float g     = fabs(cosf(theta));
        float b     = fabs(sinf(theta + M_PI));
        _render2d.setDrawColor(r, g, b, 1.0f);
        _render2d.drawLine(x1, y1, x2, y2);
    }
    _render2d.setDrawColor(0.0f, 1.0f, 0.0f, 1.0f);
    _render2d.drawRect(100, 100, 1500, 900);
    _render2d.render(pEnc);
    _render2d.clearDraw();

    pEnc->endEncoding();
    pCmd->presentDrawable(pView->currentDrawable());
    pCmd->commit();

    pPool->release();
}

//
// start up
//
int
main(int argc, char* argv[])
{
    Launch(std::make_shared<Renderer>());
    return 0;
}

//
