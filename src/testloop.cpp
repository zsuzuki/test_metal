//
// Copyright 2023 Suzuki Yoshinori(wave.suzuki.z@gmail.com)
//
#include <cmath>
#include <gamepad.h>
#include <simd/geometry.h>
#include <simd/quaternion.h>
#include <simd/vector_make.h>
#include <simd/vector_types.h>
#include <testloop.h>

namespace TestLoop
{

//
//
//
void
Update(Context& context)
{
    context.SetDrawColor(0.0f, 1.0f, 0.0f, 1.0f);
    context.DrawRect2D({100, 100}, {600, 300});
    context.SetDrawColor(1.0f, 1.0f, 1.0f);
    context.Print(200, 400, "こんにちは世界");
    context.SetDrawColor(1.0f, 1.0f, 1.0f);
    context.Print(150, 80, "TestLoop");

    GamePad::PadState padState;
    if (GamePad::GetPadState(0, padState))
    {
        static simd::float3 eye{0.0f, 0.0f, -10.0f};
        static simd::float3 tgt{0.0f, 0.0f, 10.0f};
        static simd::float3 upv{0.0f, 1.0f, 0.0f};

        auto tvec = tgt - eye;
        auto len  = simd_length(tvec);
        tvec      = simd_normalize(tvec);

        auto rad  = padState.rightX * M_PI / 200.0f;
        auto rotY = simd_quaternion(rad, upv);
        tvec      = simd_act(rotY, tvec);

        auto sideV = simd_normalize(simd::cross(upv, tvec));
        eye -= sideV * padState.leftX * 0.2f;
        eye += tvec * padState.leftY * 0.2f;
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
    context.DrawLine2D({100, 100}, {500, 500});

    // 90度ずつ4つのラジアンを作成
    static int           rotCnt = 0;
    std::array<float, 4> rotRad;
    for (auto& rad : rotRad)
    {
        rad = (rotCnt / 360.0f) * M_PI * 2.0f;
        rotCnt += 90;
    }
    rotCnt = (rotCnt + 1) % 360;

    // 90度ずつ4つのsin/cosを作成
    auto buildRotPos = [&](auto func)
    {
        for (int i = 0; i < 4; i++)
        {
            auto rad = rotRad[i];
            func(i, sinf(rad), cosf(rad));
        }
    };

    // YZ平面で回転する座標列
    std::array<simd::float3, 4> rotPosYZ;
    buildRotPos([&](int i, float s, float c) { rotPosYZ[i] = simd_make_float3(0.0f, s, c) * 5.0f; });

    // 縦回転する線
    auto lx = simd_make_float3(-15, 0, 0);
    auto rx = simd_make_float3(15, 0, 0);
    context.SetDrawColor(1.0f, 0.8f, 0.0f);
    for (int i = 0; i < 4; i++)
    {
        auto p1 = lx + rotPosYZ[i];
        auto p2 = rx + rotPosYZ[i];
        context.DrawLine3D(p1, p2);
    }
    // 線の蓋になる左右の四角形
    context.SetDrawColor(1.0f, 0.0f, 0.0f);
    auto drawSquare = [&](simd::float3 xv)
    {
        auto& pos = rotPosYZ;
        context.DrawRect3D(pos[0] + xv, pos[1] + xv, pos[2] + xv, pos[3] + xv);
    };
    drawSquare(lx);
    drawSquare(rx);

    // 回転する床
    context.SetDrawColor(0.0f, 0.2f, 0.3f);
    std::array<simd::float3, 4> rotPosXZ;
    buildRotPos([&](int i, float s, float c) { rotPosXZ[i] = simd_make_float3(s, -0.5f, c) * 20.0f; });
    context.DrawPlane3D(rotPosXZ[0], rotPosXZ[1], rotPosXZ[2], rotPosXZ[3]);
}

} // namespace TestLoop
