// Flat Color Shader

/*
#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out
vec4 color;

uniform vec4 u_Color;

void main()
{
    color = u_Color;
}
*/

#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
    | DENY_GEOMETRY_SHADER_ROOT_ACCESS \
    | DENY_HULL_SHADER_ROOT_ACCESS \
    | DENY_DOMAIN_SHADER_ROOT_ACCESS), \
RootConstants(num32BitConstants=20, b0), \
CBV(b1), \
DescriptorTable(SRV(t1, numDescriptors = 2)), \
DescriptorTable(Sampler(s1, numDescriptors = 2)), \

struct VSInput
{
    [[vk::location(0)]]float3 pos : POSITION;
    [[vk::location(1)]]float2 txc : TEXCOORD;
};

struct SmallBuffer
{
    float4x4 Model;
    float4 Color;
};

struct CompleteMVP
{
    float4x4 M;
    float4x4 V;
    float4x4 P;
    float4x4 Mips;
};

#ifdef VK_HLSL

[[vk::push_constant]] SmallBuffer m_SmallMVP;

#else

ConstantBuffer<SmallBuffer> m_SmallMVP : register(b0);

#endif

[[vk::binding(1, 0)]] ConstantBuffer<CompleteMVP> m_CompleteMVP : register(b1);
[[vk::binding(2, 0)]] Texture2D<float4> renderTexture[2] : register(t1);
[[vk::binding(3, 0)]] SamplerState dynamicSampler[2] : register(s1);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 txc : TEXCOORD;
};

PSInput vs_main(VSInput vsinput)
{
    PSInput vsoutput;
    vsoutput.pos = float4(vsinput.pos, 1.0f);
    vsoutput.pos = mul(m_SmallMVP.Model, vsoutput.pos);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.V);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.P);
    vsoutput.txc = vsinput.txc;
    return vsoutput;
}

float4 ps_main(PSInput psinput) : SV_TARGET0
{
    float4 final_color = float4(m_SmallMVP.Color.xyz, 1.0f);
    if (m_SmallMVP.Color.w == 0.0f)
        return renderTexture[0].SampleLevel(dynamicSampler[0], psinput.txc, 0.0f) * final_color;
    else
        return renderTexture[1].SampleLevel(dynamicSampler[1], psinput.txc * m_SmallMVP.Color.w, 0.0f) * final_color;
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}