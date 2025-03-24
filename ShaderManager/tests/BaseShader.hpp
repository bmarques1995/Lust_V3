#pragma once

namespace Lust
{
	constexpr const char* GraphicsFullShader = R"(
#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
	| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
	| DENY_HULL_SHADER_ROOT_ACCESS \
	| DENY_DOMAIN_SHADER_ROOT_ACCESS), 

struct VertexInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct HullInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct DomainInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct GeometryInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

cbuffer TransformBuffer : register(b0) {
    matrix model;
    matrix view;
    matrix projection;
}

// Vertex Shader (VS)
GeometryInput vs_main(VertexInput input) {
    GeometryInput output;
    matrix mvp = mul(projection, mul(view, model));
    output.position = mul(mvp, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

// Pixel Shader (PS)
float4 ps_main(PixelInput input) : SV_Target {
    return float4(input.normal * 0.5f + 0.5f, 1.0f);
}
)";

    constexpr const char* GraphicsFullShaderMissingPixel = R"(
#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
	| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
	| DENY_HULL_SHADER_ROOT_ACCESS \
	| DENY_DOMAIN_SHADER_ROOT_ACCESS), 

struct VertexInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct HullInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct DomainInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct GeometryInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

cbuffer TransformBuffer : register(b0) {
    matrix model;
    matrix view;
    matrix projection;
}

// Vertex Shader (VS)
GeometryInput vs_main(VertexInput input) {
    GeometryInput output;
    matrix mvp = mul(projection, mul(view, model));
    output.position = mul(mvp, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

)";

    constexpr const char* GraphicsPixelShaderError = R"(
#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
	| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
	| DENY_HULL_SHADER_ROOT_ACCESS \
	| DENY_DOMAIN_SHADER_ROOT_ACCESS), 

struct VertexInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct HullInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct DomainInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct GeometryInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

cbuffer TransformBuffer : register(b0) {
    matrix model;
    matrix view;
    matrix projection;
}

// Vertex Shader (VS)
GeometryInput vs_main(VertexInput input) {
    GeometryInput output;
    matrix mvp = mul(projection, mul(view, model));
    output.position = mul(mvp, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

// Pixel Shader (PS)
float4 ps_main(PixelInput input) : SV_Target {
    return float4(input.normal * 0.5f + 0.5f, 1.0f)
}
)";

    constexpr const char* GraphicsRootSignatureError = R"(
#define rs_controller \
RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT\
	| DENY_GEOMETRY_SHADER_ROOT_ACCESS\
	| DENY_HULL_SHADER_ROOT_ACCESS \
	| DENY_DOMAIN_SHADER_ROOT_ACCESS

struct VertexInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct HullInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct DomainInput {
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct GeometryInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD;
};

cbuffer TransformBuffer : register(b0) {
    matrix model;
    matrix view;
    matrix projection;
}

// Vertex Shader (VS)
GeometryInput vs_main(VertexInput input) {
    GeometryInput output;
    matrix mvp = mul(projection, mul(view, model));
    output.position = mul(mvp, float4(input.position, 1.0f));
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

// Pixel Shader (PS)
float4 ps_main(PixelInput input) : SV_Target {
    return float4(input.normal * 0.5f + 0.5f, 1.0f);
}
)";
}