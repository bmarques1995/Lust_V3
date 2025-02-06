#pragma pack_matrix(column_major)
#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
| DENY_HULL_SHADER_ROOT_ACCESS \
| DENY_DOMAIN_SHADER_ROOT_ACCESS), \
RootConstants(num32BitConstants=16, b0), \
CBV(b1),\
CBV(b2),\

struct SmallMVP
{
    float4x4 M;
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
    float4x4 M;
    float4x4 V;
    float4x4 P;
    float4x4 Mips;
};

#ifdef VK_HLSL

[[vk::push_constant]] SmallMVP m_SmallMVP;

#else

cbuffer u_SmallMVP : register(b0)
{
    SmallMVP m_SmallMVP;
};

#endif

[[vk::binding(1, 0)]]
cbuffer u_CompleteMVP : register(b1)
{
    CompleteMVP m_CompleteMVP;
};

[[vk::binding(2, 0)]]
cbuffer u_SSBO : register(b2)
{
    SSBO m_SSBO;
};

struct VSInput
{
    [[vk::location(0)]]float3 pos : POSITION;
    [[vk::location(1)]]float4 col : COLOR;
};
struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};
PSInput vs_main(VSInput vsInput)
{
    PSInput vsoutput;
    vsoutput.pos = mul(float4(vsInput.pos, 1.0f), m_SmallMVP.M);
    vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.M);
    vsoutput.pos = mul(vsoutput.pos, m_SSBO.M);
    vsoutput.col = vsInput.col;
    return vsoutput;
}
float4 ps_main(PSInput psInput) : SV_TARGET0
{
    return psInput.col;
}