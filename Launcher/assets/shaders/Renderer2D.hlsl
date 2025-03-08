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
CBV(b1)

struct VSInput
{
    [[vk::location(0)]]float3 pos : POSITION;
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

struct PSInput
{
    float4 pos : SV_POSITION;
};

PSInput vs_main(VSInput vsinput)
{
    PSInput vsoutput;
    vsoutput.pos = float4(vsinput.pos, 1.0f);
    vsoutput.pos = mul(m_SmallMVP.Model, vsoutput.pos);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.V);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.P);
    return vsoutput;
}

float4 ps_main(PSInput psinput) : SV_TARGET0
{
    return m_SmallMVP.Color;
}