/*std::string flatColorShaderVertexSrc = R"(
	#version 330 core
			
	layout(location = 0) in vec3 a_Position;

	uniform mat4 u_ViewProjection;
	uniform mat4 u_Transform;

	out vec3 v_Position;

	void main()
	{
		v_Position = a_Position;
		gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
	}
)";

std::string flatColorShaderFragmentSrc = R"(
	#version 330 core
			
	layout(location = 0) out vec4 color;

	in vec3 v_Position;
			
	uniform vec3 u_Color;

	void main()
	{
		color = vec4(u_Color, 1.0);
	}
)";*/

#pragma pack_matrix(column_major)

#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
	| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
	| DENY_HULL_SHADER_ROOT_ACCESS \
	| DENY_DOMAIN_SHADER_ROOT_ACCESS), \
RootConstants(num32BitConstants=19, b0), \
CBV(b1),

struct VSInput{
	[[vk::location(0)]]float3 pos : POSITION;
	[[vk::location(1)]]float2 txc : TEXCOORD;
};

struct PSInput{
	float4 pos : SV_POSITION;
	float2 txc : TEXCOORD;
};

struct SmallBuffer{
	float4x4 Model;
	float3 Color;
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

PSInput vs_main(VSInput vsinput)
{
	PSInput vsoutput;
	vsoutput.pos = mul(float4(vsinput.pos, 1.0f), m_SmallMVP.Model);
	vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.V);
	vsoutput.pos = mul(vsoutput.pos, m_CompleteMVP.P);
	vsoutput.txc = vsinput.txc;
	return vsoutput;
}

float4 ps_main(PSInput input) : SV_TARGET0
{
	return float4(m_SmallMVP.Color, 1.0f);
}