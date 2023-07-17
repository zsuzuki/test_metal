#include <metal_stdlib>
using namespace metal;

struct VertexData2D
{
    float2 position;
    float4 color;    
    float2 texcoord;
};

struct ScreenData
{
    float2 size;
};

struct p2f
{
    float4 pos [[position]];
    half4 color;
    float2 texcoord;
};

vertex p2f vert2d(const device VertexData2D* vertexArray [[buffer(0)]],const device ScreenData* screenData [[buffer(1)]], unsigned int vID [[vertex_id]])
{
    const device VertexData2D& vd2d = vertexArray[vID];

    float negy = screenData->size.y - vd2d.position.y;
    float2 pos = float2(vd2d.position.x, negy);

    p2f out;
    out.pos      = float4(pos / screenData->size * 2.0 - 1.0, 0.0, 1.0);
    out.color    = half4(vd2d.color.rgba);
    out.texcoord = vd2d.texcoord.xy;

    return out;
}

fragment half4 frag2d(p2f in [[stage_in]], texture2d<half, access::sample> tex [[texture(0)]] )
{
    constexpr sampler s( address::repeat, filter::linear );
    half4 texel = tex.sample( s, in.texcoord ).rgba;
    return in.color * texel;
}

//
