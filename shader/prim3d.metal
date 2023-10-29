#include <metal_stdlib>
using namespace metal;

struct VertexData
{
  float3 position;
  float4 color;
};

struct CameraData
{
    float4x4 perspectiveTransform;
    float4x4 worldTransform;
    float3x3 worldNormalTransform;
};

struct v2f
{
    float4 position [[position]];
    half4 color;
};

//
//
//
vertex v2f primVert3d( device const VertexData* vertexData [[buffer(0)]],
                       device const CameraData& cameraData [[ buffer(2)]],
                       uint vID [[vertex_id]])
{
    v2f o;

    const device VertexData& vd = vertexData[ vID ];
    float4 pos = float4( vd.position, 1.0 );
    pos = cameraData.perspectiveTransform * cameraData.worldTransform * pos;
    o.position = pos;

    o.color = half4(vd.color);

    return o;
}

fragment half4 primFrag3d( v2f in [[stage_in]] )
{
    return in.color;
}
