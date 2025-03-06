#include "D3D12ShaderReflector.hpp"
#include "FileHandler.hpp"

const std::unordered_map<std::string, bool> Lust::D3D12ShaderReflector::s_HLSLSysVals =
{
	{"SV_CLIPDISTANCE", true},
	{"SV_CULLDISTANCE", true},
	{"SV_COVERAGE", false},
	{"SV_DEPTH", false},
	{"SV_DEPTHGREATEREQUAL", false},
	{"SV_DEPTHLESSEQUAL", false},
	{"SV_DISPATCHTHREADID", false},
	{"SV_DOMAINLOCATION", false},
	{"SV_GROUPID", false},
	{"SV_GROUPINDEX", false},
	{"SV_GROUPTHREADID", false},
	{"SV_GSINSTANCEID", false},
	{"SV_INNERCOVERAGE", false},
	{"SV_INSIDETESSFACTOR", false},
	{"SV_INSTANCEID", false},
	{"SV_ISFRONTFACE", false},
	{"SV_OUTPUTCONTROLPOINTID", false},
	{"SV_POSITION", false},
	{"SV_PRIMITIVEID", false},
	{"SV_RENDERTARGETARRAYINDEX", false},
	{"SV_SAMPLEINDEX", false},
	{"SV_STENCILREF", false},
	{"SV_TARGET", true},
	{"SV_TESSFACTOR", false},
	{"SV_VERTEXID", false},
	{"SV_VIEWPORTARRAYINDEX", false},
	{"SV_SHADINGRATE", false},
};

Lust::D3D12ShaderReflector::D3D12ShaderReflector(std::string_view jsonFilepath)
{
	StartDxc();
	InitJsonAndPaths(jsonFilepath);
	UploadBlob("rs", m_RootBlob.GetAddressOf());
	ReflectRootBlob();
	for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
	{
		UploadBlob(*it, m_ShaderBlobs[*it].GetAddressOf());
		ReflectStage(*it, m_ShaderBlobs[*it].Get());
	}
	GenerateInputBufferLayout();
}

Lust::D3D12ShaderReflector::~D3D12ShaderReflector()
{
}

void Lust::D3D12ShaderReflector::GenerateInputBufferLayout()
{
	D3D12_SHADER_DESC shader_desc;
	m_ShaderReflections["vs"]->GetDesc(&shader_desc);
	m_InputBufferLayout.Clear();

	for (size_t i = 0; i < shader_desc.InputParameters; i++)
	{

		D3D12_SIGNATURE_PARAMETER_DESC signature_param_desc;
		m_ShaderReflections["vs"]->GetInputParameterDesc(i, &signature_param_desc);
		auto type = CastToShaderDataType(signature_param_desc.Mask, signature_param_desc.ComponentType);
		std::string castedSemanticName = signature_param_desc.SemanticName;
		std::transform(castedSemanticName.begin(), castedSemanticName.end(), castedSemanticName.begin(), ::toupper);
		auto it = s_HLSLSysVals.find(castedSemanticName);
		if (it == s_HLSLSysVals.end())
		{
			InputBufferElement ibe(type, signature_param_desc.SemanticName, false);
			m_InputBufferLayout.PushBack(ibe);
		}
	}
}

void Lust::D3D12ShaderReflector::UploadBlob(std::string_view shader_stage, IDxcBlob** blob)
{
	HRESULT hr;

	std::string shaderName = m_PipelineInfo["BinShaders"][shader_stage.data()]["filename"].asString();
	if ((shaderName.size() == 0))
		return;
	std::stringstream shaderFullPath;
	shaderFullPath << m_ShaderDir << "/" << shaderName;
	std::string shaderPath = shaderFullPath.str();

	if (!FileHandler::FileExists(shaderPath))
		return;

	size_t blobSize;
	std::byte* blobData;

	if (!FileHandler::ReadBinFile(shaderPath, &blobData, &blobSize))
		return;

	// Create blob from memory
	hr = m_DxcLib->CreateBlob((void*)blobData, blobSize, DXC_CP_ACP, (IDxcBlobEncoding**)blob);
	assert(hr == S_OK);

	delete[] blobData;
}

void Lust::D3D12ShaderReflector::StartDxc()
{
	HRESULT hr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(m_DxcLib.GetAddressOf()));
	assert(hr == S_OK);

	hr = DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(m_DxcContainerReflection.GetAddressOf()));
	assert(hr == S_OK);
}

void Lust::D3D12ShaderReflector::ReflectRootBlob()
{
	HRESULT hr;
	hr = D3D12CreateRootSignatureDeserializer(m_RootBlob->GetBufferPointer(), m_RootBlob->GetBufferSize(), IID_PPV_ARGS(m_RootSigDeserializer.GetAddressOf()));
	assert(hr == S_OK);
}

void Lust::D3D12ShaderReflector::ReflectStage(std::string_view shader_stage, IDxcBlob* shader_blob)
{
	if (shader_blob == nullptr)
		return;

	HRESULT hr;

	UINT32 shaderIdx;
	hr = m_DxcContainerReflection->Load(shader_blob);
	assert(hr == S_OK);
	hr = m_DxcContainerReflection->FindFirstPartKind(DXC_PART_DXIL, &shaderIdx);
	assert(hr == S_OK);

	ComPointer<ID3D12ShaderReflection> shaderReflection;
	hr = m_DxcContainerReflection->GetPartReflection(shaderIdx, IID_PPV_ARGS(m_ShaderReflections[shader_stage.data()].GetAddressOf()));
	assert(hr == S_OK);
}

Lust::ShaderDataType Lust::D3D12ShaderReflector::CastToShaderDataType(int8_t mask, D3D_REGISTER_COMPONENT_TYPE type)
{
	D3D_REGISTER_COMPONENT_FLOAT32;
	D3D_REGISTER_COMPONENT_UINT32;

	static const std::unordered_map<uint16_t, ShaderDataType> s_MaskToDataType =
	{
		{((1 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_FLOAT32),ShaderDataType::Float},
		{((2 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_FLOAT32),ShaderDataType::Float2},
		{((3 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_FLOAT32),ShaderDataType::Float3},
		{((4 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_FLOAT32),ShaderDataType::Float4},
		{((1 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_UINT32),ShaderDataType::Uint},
		{((2 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_UINT32),ShaderDataType::Uint2},
		{((3 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_UINT32),ShaderDataType::Uint3},
		{((4 << 8) + (uint8_t)D3D_REGISTER_COMPONENT_UINT32),ShaderDataType::Uint4}
	};
	size_t i = 0;
	for (i = 0; ((1 << i) & mask) != 0; i++);

	auto it = s_MaskToDataType.find((i << 8) + (uint8_t)type);
	if (it != s_MaskToDataType.end())
		return it->second;
	else
		return ShaderDataType::None;
}
