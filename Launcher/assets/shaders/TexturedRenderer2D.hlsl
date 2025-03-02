#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
    | DENY_GEOMETRY_SHADER_ROOT_ACCESS \
    | DENY_HULL_SHADER_ROOT_ACCESS \
    | DENY_DOMAIN_SHADER_ROOT_ACCESS), \
RootConstants(num32BitConstants=20, b0), \
CBV(b1), \
DescriptorTable(SRV(t1, numDescriptors = 1)), \
DescriptorTable(Sampler(s1, numDescriptors = 1)), \

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

cbuffer u_SmallMVP : register(b0)
{
    SmallBuffer m_SmallMVP;
};

#endif

[[vk::binding(1, 0)]]
cbuffer u_CompleteMVP : register(b1)
{
    CompleteMVP m_CompleteMVP;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 txc : TEXCOORD;
};

[[vk::binding(2, 0)]] Texture2D<float4> renderTexture : register(t1);
[[vk::binding(3, 0)]] SamplerState dynamicSampler : register(s1);

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
    return renderTexture.SampleLevel(dynamicSampler, psinput.txc, 0.0f);
}