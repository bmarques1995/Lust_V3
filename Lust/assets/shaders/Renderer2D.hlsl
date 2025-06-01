#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
    | DENY_GEOMETRY_SHADER_ROOT_ACCESS \
    | DENY_HULL_SHADER_ROOT_ACCESS \
    | DENY_DOMAIN_SHADER_ROOT_ACCESS), \
CBV(b1), \
SRV(t0), \
DescriptorTable(SRV(t1, numDescriptors = 256)), \
DescriptorTable(Sampler(s1, numDescriptors = 16)), \

//RootConstants(num32BitConstants=24, b0), \

struct VSInput
{
    [[vk::location(0)]]float3 pos : POSITION;
    uint instanceID : SV_INSTANCEID;
    uint vertexID : SV_VERTEXID;
};

struct CompleteMVP
{
    float4x4 M;
    float4x4 V;
    float4x4 P;
    float4x4 Mips;
};

struct SSBO
{
    float4x4 Model;
    /*
    Controllers.x = texture index, Controllers.y = sampler index
    
    high(Controllers.z) = msdf_effect, low(Controllers.z) = mip level, Controllers.w = mip bias
    */
    uint4 Controllers;
    float4 TexCoordsEdges;
    uint4 EdgeColors;
};

//[[vk::binding(0, 0)]] ConstantBuffer<SmallMVP> m_SmallMVP : register(b0);
[[vk::binding(0, 0)]] ConstantBuffer<CompleteMVP> m_CompleteMVP : register(b1);
[[vk::binding(1, 0)]] Texture2D<float4> renderTexture[256] : register(t1);
[[vk::binding(2, 0)]] SamplerState dynamicSampler[16] : register(s1);
[[vk::binding(3, 0)]] StructuredBuffer<SSBO> u_InstancedMVP : register(t0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 txc : TEXCOORD;
    float4 col : COLOR;
    uint instanceID : INSTANCEID;
};

float4 GenerateColor(uint edgeColor)
{
    float4 color;
    color.w = ((edgeColor % 256) * 1.0f) / 255.0f;
    edgeColor /= 256;
    color.z = ((edgeColor % 256) * 1.0f) / 255.0f;
    edgeColor /= 256;
    color.y = ((edgeColor % 256) * 1.0f) / 255.0f;
    edgeColor /= 256;
    color.x = ((edgeColor % 256) * 1.0f) / 255.0f;
    return color;
}

float2 GenerateTexCoords(uint vertexID, float4 texCoordsEdges)
{
    float2 texCoord;
    switch(vertexID %4)
    {
        case 0:
            texCoord = float2(texCoordsEdges.x, texCoordsEdges.w);
            break;
        case 1:
            texCoord = float2(texCoordsEdges.x, texCoordsEdges.y);
            break;
        case 2:
            texCoord = float2(texCoordsEdges.z, texCoordsEdges.w);
            break;
        case 3:
            texCoord = float2(texCoordsEdges.z, texCoordsEdges.y);
            break;
        default:
            texCoord = float2(0.0f, 0.0f);
            break;
    }
    return texCoord;
}

PSInput vs_main(VSInput vsinput)
{
    PSInput vsoutput;
    vsoutput.pos = float4(vsinput.pos, 1.0f);
    vsoutput.pos = mul(u_InstancedMVP[vsinput.instanceID].Model, vsoutput.pos);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.M);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.V);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.P);
    vsoutput.txc = GenerateTexCoords(vsinput.vertexID, u_InstancedMVP[vsinput.instanceID].TexCoordsEdges);
    vsoutput.col = GenerateColor(u_InstancedMVP[vsinput.instanceID].EdgeColors[vsinput.vertexID]);
    vsoutput.instanceID = vsinput.instanceID;
    return vsoutput;
}


float median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

uint2 spvTextureSize(Texture2D<float4> Tex, uint Level, out uint Param)
{
    uint2 ret;
    Tex.GetDimensions(Level, ret.x, ret.y, Param);
    return ret;
}

float screenPxRange(uint textureIndex, float2 texCoords)
{
    const float pxRange = 2.0f;
    uint discarder;
    uint2 dimensions = spvTextureSize(renderTexture[textureIndex], 0, discarder);
    float2 unitRange = pxRange.xx / float2(dimensions);
    float2 screenTexSize = 1.0f.xx / fwidth(texCoords * .015625f);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0f);
}

float msdfOpacity(uint textureIndex, uint samplerIndex, float2 texCoords)
{
    float3 msd = renderTexture[textureIndex].SampleLevel(dynamicSampler[samplerIndex], texCoords, 0.0f).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange(textureIndex, texCoords) * (sd - 0.5f);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    return opacity;
}

float4 ps_main(PSInput psinput) : SV_TARGET0
{
    /*
    vec4 bgColor = vec4(Input.Color.rgb, 0.0); // TODO(Yan): outlines
    vec4 fgColor = Input.Color;

	// NOTE(Yan): MSDF texture has no mips (only LOD 0), but in the future it might
	//            be nice to do some sort of fading/smoothing when camera is far
    vec3 msd = texture(u_FontAtlases[int(TexIndex)], Input.TexCoord).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = ScreenPxRange() * (sd - 0.5f);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    color = mix(bgColor, fgColor, opacity);
    */
    uint textureIndex = u_InstancedMVP[psinput.instanceID].Controllers.x;
    uint samplerIndex = u_InstancedMVP[psinput.instanceID].Controllers.y;
    
    float opacity = msdfOpacity(textureIndex, samplerIndex, psinput.txc);
    float4 pixel = lerp(0.0f.xxxx, psinput.col, opacity);
    if (pixel.a == 0.0f)
        discard;
    return pixel;
    //return renderTexture[textureIndex].SampleLevel(dynamicSampler[samplerIndex], psinput.txc, 0.0f) * psinput.col;
}