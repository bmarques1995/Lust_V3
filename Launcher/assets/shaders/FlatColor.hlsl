#pragma pack_matrix(column_major)

#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
	| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
	| DENY_HULL_SHADER_ROOT_ACCESS \
	| DENY_DOMAIN_SHADER_ROOT_ACCESS), \
RootConstants(num32BitConstants=24, b0), \
CBV(b1),\
SRV(t0)

struct VSInput{
	[[vk::location(0)]]float3 pos : POSITION;
	[[vk::location(1)]]float2 txc : TEXCOORD;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 txc : TEXCOORD;
	uint instanceID : INSTANCE;
};

struct SmallBuffer{
	float4x4 Model;
	float4 Color1;
	float4 Color2;
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

[[vk::binding(1, 0)]] cbuffer u_CompleteMVP : register(b1)
{
	CompleteMVP m_CompleteMVP;
};

[[vk::binding(2, 0)]] StructuredBuffer<float4x4> u_InstancedMVP : register(t0);

PSInput vs_main(VSInput vsinput, uint instanceID : SV_InstanceID)
{
	PSInput vsoutput;
	float4x4 elementModelMatrix = u_InstancedMVP[instanceID];
	vsoutput.pos = float4(vsinput.pos, 1.0f);
	vsoutput.pos = mul(vsoutput.pos, elementModelMatrix);
	vsoutput.pos = mul(vsoutput.pos, m_SmallMVP.Model);
	vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.V);
	vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.P);
	vsoutput.txc = vsinput.txc;
	vsoutput.instanceID = instanceID;
	return vsoutput;
}

float4 ps_main(PSInput input) : SV_TARGET0
{
	return ((input.instanceID % 2 == 0) ? m_SmallMVP.Color1 : m_SmallMVP.Color2);
}