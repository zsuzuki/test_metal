//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <cmath>
#include <gamepad.h>
#include <simd/geometry.h>
#include <simd/quaternion.h>
#include <testloop.h>

namespace TestLoop
{

//
//
//
void
Update(Context& context)
{
    context.SetDrawColor(1.0f, 1.0f, 1.0f);
    context.Print(200, 400, "こんにちは世界");
    GamePad::PadState padState;
    if (GamePad::GetPadState(0, padState))
    {
        static simd::float3 eye{0.0f, 0.0f, -10.0f};
        static simd::float3 tgt{0.0f, 0.0f, 10.0f};
        static simd::float3 upv{0.0f, 1.0f, 0.0f};

        auto tvec = tgt - eye;
        auto len  = simd_length(tvec);
        tvec      = simd_normalize(tvec);

        auto rad  = padState.rightX * M_PI / 300.0f;
        auto rotY = simd_quaternion(rad, upv);
        tvec      = simd_act(rotY, tvec);

        eye[0] -= padState.leftX * 0.2f;
        eye[2] += padState.leftY * 0.2f;
        tvec *= len;
        tgt = eye + tvec;

        auto& cam = context.GetCamera();
        cam.setEyePosition(eye);
        cam.setTargetPosition(tgt);
    }
    else
    {
        context.SetDrawColor(1.0f, 0.8f, 0.1f);
        context.Print(600, 150, "Pad Disable");
    }

    context.SetDrawColor(1.0f, 1.0f, 1.0f);
    context.Print(600, 100, "TestLoop");
}

} // namespace TestLoop
