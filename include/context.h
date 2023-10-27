//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include "camera_interface.h"
#include <cinttypes>
#include <string>

//
//
//
class Context
{

  public:
    Context()          = default;
    virtual ~Context() = default;

    // get 3d camera control
    virtual CameraInterface& GetCamera() = 0;
    // set draw color
    virtual void SetDrawColor(float r, float g, float b, float a = 1.0f) = 0;
    // display message on screen
    virtual void Print(float x, float y, std::string msg) = 0;
    //
    virtual void DrawLine(float x1, float y1, float x2, float y2) = 0;
};
