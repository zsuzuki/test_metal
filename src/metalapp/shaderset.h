//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <cinttypes>
#include <string>

namespace MTL
{
class Device;
class Library;
class RenderPipelineState;
} // namespace MTL

//
//
//
class ShaderSet
{
    MTL::Library*             shaderLibrary_ = nullptr;
    MTL::RenderPipelineState* rpState_       = nullptr;

  public:
    ShaderSet() = default;
    virtual ~ShaderSet();

    bool build(MTL::Device* dev, const char* program, const char* vsMain, const char* fgMain, bool blendAlpha = false);
    bool load(MTL::Device* dev, std::string path, const char* vsMain, const char* fgMain, bool blendAlpha = false);
    void release();

    MTL::Library*             getShaderLibrary() { return shaderLibrary_; }
    MTL::RenderPipelineState* getRenderPipelineState() { return rpState_; }
};

//
