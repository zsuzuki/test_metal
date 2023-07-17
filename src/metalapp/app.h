//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include <memory>

namespace MTL
{
class Device;
}
namespace MTK
{
class View;
}

//
//
//
class DelegateLoop
{
  public:
    virtual ~DelegateLoop() = default;

    virtual const char* getTitle() const             = 0;
    virtual void        initialize(MTL::Device* dev) = 0;
    virtual void        finalize()                   = 0;
    virtual void        draw(MTK::View* view)        = 0;
    virtual float       getScreenWidth() const       = 0;
    virtual float       getScreenHeight() const      = 0;
};

void Launch(std::shared_ptr<DelegateLoop> dlp);

//
