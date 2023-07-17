//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include "Foundation/NSString.hpp"
#include <AppKit/AppKit.hpp>
#include <CoreFoundation/CoreFoundation.h>
#include <Metal/Metal.hpp>
#include <MetalKit/MetalKit.hpp>

#include "shaderset.h"
#include <fstream>
#include <iostream>

//
//
//
ShaderSet::~ShaderSet() { release(); }

//
//
//
bool
ShaderSet::build(MTL::Device* dev, const char* program, const char* vsMain, const char* fgMain, bool blendAlpha)
{
    // 基本的にサンプルのまま
    using NS::StringEncoding::UTF8StringEncoding;

    NS::Error* pError   = nullptr;
    auto*      pLibrary = dev->newLibrary(NS::String::string(program, UTF8StringEncoding), nullptr, &pError);
    if (pLibrary == nullptr)
    {
        std::cerr << pError->localizedDescription()->utf8String() << std::endl;
        assert(false);
        return false;
    }

    MTL::Function* pVertexFn = nullptr;
    MTL::Function* pFragFn   = nullptr;
    if (vsMain)
    {
        pVertexFn = pLibrary->newFunction(NS::String::string(vsMain, UTF8StringEncoding));
    }
    if (fgMain)
    {
        pFragFn = pLibrary->newFunction(NS::String::string(fgMain, UTF8StringEncoding));
    }

    auto* pDesc = MTL::RenderPipelineDescriptor::alloc()->init();
    if (pVertexFn)
    {
        pDesc->setVertexFunction(pVertexFn);
    }
    if (pFragFn)
    {
        pDesc->setFragmentFunction(pFragFn);
    }

    auto* clrAtt = pDesc->colorAttachments()->object(0);
    clrAtt->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    if (blendAlpha)
    {
        clrAtt->setBlendingEnabled(true);
        clrAtt->setSourceRGBBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
        clrAtt->setDestinationRGBBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);
        clrAtt->setRgbBlendOperation(MTL::BlendOperation::BlendOperationAdd);
    }
    pDesc->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth16Unorm);

    rpState_ = dev->newRenderPipelineState(pDesc, &pError);
    if (rpState_ == nullptr)
    {
        std::cerr << pError->localizedDescription()->utf8String() << std::endl;
        assert(false);
        return false;
    }

    if (pVertexFn)
    {
        pVertexFn->release();
    }
    if (pFragFn)
    {
        pFragFn->release();
    }
    pDesc->release();

    return true;
}

//
//
//
bool
ShaderSet::load(MTL::Device* dev, std::string path, const char* vsMain, const char* fgMain, bool blendAlpha)
{
    std::ifstream file(path);
    if (file.fail())
    {
        std::cerr << "shader file read failed: " << path << std::endl;
        return false;
    }

    file.seekg(0, std::ios_base::end);
    auto sz = file.tellg();
    file.seekg(0);

    std::string buffer;
    buffer.resize(sz);
    file.read(buffer.data(), sz);

    return build(dev, buffer.c_str(), vsMain, fgMain, blendAlpha);
}

//
//
//
void
ShaderSet::release()
{
    if (shaderLibrary_)
    {
        shaderLibrary_->release();
        shaderLibrary_ = nullptr;
    }
    if (rpState_)
    {
        rpState_->release();
        rpState_ = nullptr;
    }
}

//
