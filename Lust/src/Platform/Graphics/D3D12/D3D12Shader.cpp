#ifdef LUST_USES_WINDOWS

#include "D3D12Shader.hpp"
#include "FileHandler.hpp"
#include "Application.hpp"
#include <filesystem>

namespace fs = std::filesystem;

const std::unordered_map<std::string, std::function<void(IDxcBlob**, D3D12_GRAPHICS_PIPELINE_STATE_DESC*)>> Lust::D3D12Shader::s_ShaderPusher =
{
	{"vs", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->VS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"ps", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->PS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"gs", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->GS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"ds", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->DS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
	{"hs", [](IDxcBlob** blob, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)-> void
	{
		graphicsDesc->HS = {(*blob)->GetBufferPointer(), (*blob)->GetBufferSize()};
	}},
};

const std::list<std::string> Lust::D3D12Shader::s_GraphicsPipelineStages =
{
	"vs",
	"ps",
	"gs",
	"ds",
	"hs",
};

Lust::D3D12Shader::D3D12Shader(const std::shared_ptr<D3D12Context>* context, std::string json_controller_path, InputBufferLayout layout) :
	m_Context(context), m_Layout(layout)
{
	HRESULT hr;
	auto device = (*m_Context)->GetDevicePtr();

	InitJsonAndPaths(json_controller_path);

	auto nativeElements = m_Layout.GetElements();
	D3D12_INPUT_ELEMENT_DESC* ied = new D3D12_INPUT_ELEMENT_DESC[nativeElements.size()];

	for (size_t i = 0; i < nativeElements.size(); i++)
	{
		ied[i].SemanticName = nativeElements[i].GetName().c_str();
		ied[i].SemanticIndex = 0;
		ied[i].Format = GetNativeFormat(nativeElements[i].GetType());
		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = nativeElements[i].GetOffset();
		ied[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}

	assert(m_RootSignatureSize <= 256);

	CreateGraphicsRootSignature(m_RootSignature.GetAddressOf(), device);

	//https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_pipeline_state_subobject_type
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsDesc = {};
	graphicsDesc.NodeMask = 1;
	graphicsDesc.InputLayout = { ied, (uint32_t)nativeElements.size() };
	graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	graphicsDesc.pRootSignature = m_RootSignature.Get();
	graphicsDesc.SampleMask = UINT_MAX;
	graphicsDesc.NumRenderTargets = 1;
	graphicsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	graphicsDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
		graphicsDesc.SampleDesc.Count = 1;
	graphicsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
	{
		PushShader(*it, &graphicsDesc);
	}

	BuildBlender(&graphicsDesc);
	BuildRasterizer(&graphicsDesc);
	BuildDepthStencil(&graphicsDesc);

	D3D12_PIPELINE_STATE_STREAM_DESC sample;

	hr = device->CreateGraphicsPipelineState(&graphicsDesc, IID_PPV_ARGS(m_GraphicsPipeline.GetAddressOf()));
	assert(hr == S_OK);

	delete[] ied;
}

Lust::D3D12Shader::~D3D12Shader()
{
}

void Lust::D3D12Shader::Stage()
{
	auto cmdList = (*m_Context)->GetCurrentCommandList();
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->SetGraphicsRootSignature(m_RootSignature.Get());
	cmdList->SetPipelineState(m_GraphicsPipeline.Get());
}

uint32_t Lust::D3D12Shader::GetStride() const
{
	return m_Layout.GetStride();
}

uint32_t Lust::D3D12Shader::GetOffset() const
{
	return 0;
}

void Lust::D3D12Shader::CreateGraphicsRootSignature(ID3D12RootSignature** rootSignature, ID3D12Device10* device)
{
	HRESULT hr;
	std::string shaderName = m_PipelineInfo["BinShaders"]["rs"]["filename"].asString();
	std::stringstream shaderFullPath;
	shaderFullPath << m_ShaderDir << "/" << shaderName;
	std::string shaderPath = shaderFullPath.str();

	if (!FileHandler::FileExists(shaderPath))
		return;

	ComPointer<IDxcUtils> lib;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(lib.GetAddressOf()));
	assert(hr == S_OK);

	size_t blobSize;
	std::byte* blobData;

	if (!FileHandler::ReadBinFile(shaderPath, &blobData, &blobSize))
		return;

	// Create blob from memory
	hr = lib->CreateBlob((void*)blobData, blobSize, DXC_CP_ACP, (IDxcBlobEncoding**)m_RootBlob.GetAddressOf());

	hr = device->CreateRootSignature(0, m_RootBlob->GetBufferPointer(), m_RootBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature));
	assert(hr == S_OK);
}

void Lust::D3D12Shader::BuildBlender(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	graphicsDesc->BlendState.AlphaToCoverageEnable = false;
	graphicsDesc->BlendState.RenderTarget[0].BlendEnable = true;
	graphicsDesc->BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	graphicsDesc->BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsDesc->BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	graphicsDesc->BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	graphicsDesc->BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	graphicsDesc->BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	graphicsDesc->BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void Lust::D3D12Shader::BuildRasterizer(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	graphicsDesc->RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	graphicsDesc->RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	graphicsDesc->RasterizerState.FrontCounterClockwise = false;
	graphicsDesc->RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	graphicsDesc->RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	graphicsDesc->RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	graphicsDesc->RasterizerState.DepthClipEnable = true;
	graphicsDesc->RasterizerState.MultisampleEnable = false;
	graphicsDesc->RasterizerState.AntialiasedLineEnable = false;
	graphicsDesc->RasterizerState.ForcedSampleCount = 0;
	graphicsDesc->RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
}

void Lust::D3D12Shader::BuildDepthStencil(D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	graphicsDesc->DepthStencilState.DepthEnable = true;
	graphicsDesc->DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsDesc->DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsDesc->DepthStencilState.StencilEnable = true;
	graphicsDesc->DepthStencilState.FrontFace.StencilFailOp = graphicsDesc->DepthStencilState.FrontFace.StencilDepthFailOp = graphicsDesc->DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	graphicsDesc->DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	graphicsDesc->DepthStencilState.BackFace = graphicsDesc->DepthStencilState.FrontFace;
}

void Lust::D3D12Shader::PushShader(std::string_view stage, D3D12_GRAPHICS_PIPELINE_STATE_DESC* graphicsDesc)
{
	std::string shaderName = m_PipelineInfo["BinShaders"][stage.data()]["filename"].asString();
	std::stringstream shaderFullPath;
	shaderFullPath << m_ShaderDir << "/" << shaderName;
	std::string shaderPath = shaderFullPath.str();

	if (!FileHandler::FileExists(shaderPath))
		return;

	ComPointer<IDxcUtils> lib;
	HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(lib.GetAddressOf()));
	assert(hr == S_OK);

	size_t blobSize;
	std::byte* blobData;

	if (!FileHandler::ReadBinFile(shaderPath, &blobData, &blobSize))
		return;

	// Create blob from memory
	ComPointer<IDxcBlob> pBlob;
	hr = lib->CreateBlob((void*)blobData, blobSize, DXC_CP_ACP, (IDxcBlobEncoding**)pBlob.GetAddressOf());
	assert(hr == S_OK);
	m_ShaderBlobs[stage.data()] = pBlob;

	auto it = s_ShaderPusher.find(stage.data());
	if (it != s_ShaderPusher.end())
		it->second(m_ShaderBlobs[stage.data()].GetAddressOf(), graphicsDesc);

	delete[] blobData;
}

void Lust::D3D12Shader::InitJsonAndPaths(std::string json_controller_path)
{
	Json::Reader reader;
	std::string jsonResult;
	FileHandler::ReadTextFile(json_controller_path, &jsonResult);
	reader.parse(jsonResult, m_PipelineInfo);

	fs::path location = json_controller_path;
	m_ShaderDir = location.parent_path().string();
}

DXGI_FORMAT Lust::D3D12Shader::GetNativeFormat(ShaderDataType type)
{
	switch (type)
	{
	case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
	case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
	case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Uint: return DXGI_FORMAT_R32_UINT;
	case ShaderDataType::Uint2: return DXGI_FORMAT_R32G32_UINT;
	case ShaderDataType::Uint3: return DXGI_FORMAT_R32G32B32_UINT;
	case ShaderDataType::Uint4: return DXGI_FORMAT_R32G32B32A32_UINT;
	case ShaderDataType::Bool: return DXGI_FORMAT_R8_UINT;
	default: return DXGI_FORMAT_UNKNOWN;
	}
}

#endif
