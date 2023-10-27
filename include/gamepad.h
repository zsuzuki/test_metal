//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#pragma once

#include "Metal/MTLArgument.hpp"

namespace GamePad
{

//
//
//
struct PadState
{
    //
    class Button
    {
        bool on_;
        bool prev_;

      public:
        Button() : on_(false), prev_(false) {}
        explicit Button(bool swon, bool prev) : on_(swon), prev_(prev) {}
        Button(const Button& other) = default;
        Button(Button&& other)      = delete;
        ~Button()                   = default;

        Button& operator=(const Button&) = default;
        Button& operator=(Button&&)      = delete;

        [[nodiscard]] bool Pressed() const { return on_; }
        [[nodiscard]] bool On() const { return on_ && !prev_; }
        [[nodiscard]] bool Release() const { return !on_ && prev_; }
    };

    bool   enabled_;
    Button buttonUp;
    Button buttonDown;
    Button buttonLeft;
    Button buttonRight;
    Button buttonA;
    Button buttonB;
    Button buttonC;
    Button buttonD;
    Button shoulderL;
    Button shoulderR;
    Button buttonMenu;
    Button buttonOptions;
    float  leftX;
    float  leftY;
    float  rightX;
    float  rightY;
    float  triggerL;
    float  triggerR;
};

//
bool GetPadState(int idx, PadState& state);

}; // namespace GamePad
