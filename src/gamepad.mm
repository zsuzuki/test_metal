//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#import "gamepad.h"
#import <Foundation/Foundation.h>
#import <GameController/GCKeyboard.h>
#include <GameController/GameController.h>

namespace GamePad
{
//
//
//
bool
GetPadState(int idx, PadState& state)
{
    NSArray<GCController*>* const padArray = [GCController controllers];
    if (idx < 0 || idx >= padArray.count)
    {
        state.enabled_ = false;
        return false;
    }
    auto pad   = padArray[idx];
    auto input = [pad extendedGamepad];
    if (input == nullptr)
    {
        state.enabled_ = false;
        return false;
    }

    state.enabled_   = true;
    auto setupButton = [&](PadState::Button& btn, GCControllerButtonInput* src)
    {
        const PadState::Button newState{src.isPressed, btn.On()};
        btn = newState;
    };
    setupButton(state.buttonMenu, input.buttonMenu);
    setupButton(state.buttonOptions, input.buttonOptions);
    setupButton(state.buttonA, input.buttonA);
    setupButton(state.buttonB, input.buttonB);
    setupButton(state.buttonC, input.buttonX);
    setupButton(state.buttonD, input.buttonY);
    setupButton(state.shoulderL, input.leftShoulder);
    setupButton(state.shoulderR, input.rightShoulder);
    setupButton(state.buttonUp, input.dpad.up);
    setupButton(state.buttonDown, input.dpad.down);
    setupButton(state.buttonLeft, input.dpad.left);
    setupButton(state.buttonRight, input.dpad.right);
    auto lStick    = input.leftThumbstick;
    auto rStick    = input.rightThumbstick;
    state.leftX    = lStick.xAxis.value;
    state.leftY    = lStick.yAxis.value;
    state.rightX   = rStick.xAxis.value;
    state.rightY   = rStick.yAxis.value;
    state.triggerL = input.leftTrigger.analog ? input.leftTrigger.value : 0.0f;
    state.triggerR = input.rightTrigger.analog ? input.rightTrigger.value : 0.0f;

    return true;
}

} // namespace GamePad
