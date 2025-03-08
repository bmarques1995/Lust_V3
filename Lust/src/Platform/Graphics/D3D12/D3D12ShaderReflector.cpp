#include "D3D12ShaderReflector.hpp"
#include "FileHandler.hpp"
#include <Application.hpp>

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

namespace Lust
{
	namespace Declaration
	{
		constexpr uint16_t cbvLocation = 0;
		constexpr uint16_t srvLocation = 1;
		constexpr uint16_t uavLocation = 2;
		constexpr uint16_t samplerLocation = 3;
	}
}

const std::unordered_map<D3D_SHADER_INPUT_TYPE, uint16_t> Lust::D3D12ShaderReflector::s_TypesMap =
{
	{ D3D_SIT_CBUFFER, Lust::Declaration::cbvLocation },
	{ D3D_SIT_TBUFFER, Lust::Declaration::srvLocation },
	{ D3D_SIT_TEXTURE, Lust::Declaration::srvLocation },
	{ D3D_SIT_SAMPLER, Lust::Declaration::samplerLocation },
	{ D3D_SIT_UAV_RWTYPED, Lust::Declaration::uavLocation },
	{ D3D_SIT_STRUCTURED, Lust::Declaration::srvLocation },
	{ D3D_SIT_UAV_RWSTRUCTURED, Lust::Declaration::uavLocation },
	{ D3D_SIT_BYTEADDRESS, Lust::Declaration::srvLocation },
	{ D3D_SIT_UAV_RWBYTEADDRESS, Lust::Declaration::srvLocation },
	{ D3D_SIT_UAV_APPEND_STRUCTURED, Lust::Declaration::uavLocation },
	{ D3D_SIT_UAV_CONSUME_STRUCTURED, Lust::Declaration::uavLocation },
	{ D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER, Lust::Declaration::uavLocation },
	{ D3D_SIT_RTACCELERATIONSTRUCTURE, Lust::Declaration::srvLocation },
	{ D3D_SIT_UAV_FEEDBACKTEXTURE, Lust::Declaration::uavLocation }
};

const std::unordered_map<D3D12_ROOT_PARAMETER_TYPE, uint16_t> Lust::D3D12ShaderReflector::s_ReturnTypesMap =
{
	{ D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS, Lust::Declaration::cbvLocation },
	{ D3D12_ROOT_PARAMETER_TYPE_CBV, Lust::Declaration::cbvLocation },
	{ D3D12_ROOT_PARAMETER_TYPE_SRV, Lust::Declaration::srvLocation },
	{ D3D12_ROOT_PARAMETER_TYPE_UAV, Lust::Declaration::uavLocation }
};

const std::unordered_map<D3D12_DESCRIPTOR_RANGE_TYPE, uint16_t> Lust::D3D12ShaderReflector::s_ReturnRangeTypesMap = {

	{D3D12_DESCRIPTOR_RANGE_TYPE_SRV, Lust::Declaration::srvLocation },
	{D3D12_DESCRIPTOR_RANGE_TYPE_UAV, Lust::Declaration::uavLocation },
	{D3D12_DESCRIPTOR_RANGE_TYPE_CBV, Lust::Declaration::cbvLocation },
	{D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, Lust::Declaration::samplerLocation }
};

Lust::D3D12ShaderReflector::D3D12ShaderReflector(std::string_view jsonFilepath, uint32_t stages, uint32_t numInstances) :
	ShaderReflector(stages, numInstances)
{
	StartDxc();
	InitJsonAndPaths(jsonFilepath);
	m_InputBufferLayout.Clear();
	m_SmallBufferLayout.Clear();
	m_UniformLayout.Clear();
	m_StructuredBufferLayout.Clear();
	m_TextureLayout.Clear();
	m_SamplerLayout.Clear();
	UploadBlob("rs", m_RootBlob.GetAddressOf());
	ReflectRootBlob();
	for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
	{
		UploadBlob(*it, m_ShaderBlobs[*it].GetAddressOf());
		ReflectStage(*it, m_ShaderBlobs[*it].Get());
		PushExternalElementPreInfo(m_ShaderReflections[*it].Get(), 0);
	}
	ReflectRootSignature();
	GenerateInputBufferLayout();
}

Lust::D3D12ShaderReflector::~D3D12ShaderReflector()
{
}

void Lust::D3D12ShaderReflector::GenerateInputBufferLayout()
{
	D3D12_SHADER_DESC shader_desc;
	m_ShaderReflections["vs"]->GetDesc(&shader_desc);
	

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

void Lust::D3D12ShaderReflector::PushExternalElementPreInfo(ID3D12ShaderReflection* reflection, size_t resourceIndex)
{
	if (reflection == nullptr)
		return;
	HRESULT hr;
	D3D12_SHADER_INPUT_BIND_DESC inputDesc;
	/* For structured buffers (SSBO), the variable `NumSamples` stores the number of bytes USED by the shader.
	* So, to get the full amount of declared bytes, u must use all the elements of the buffer.
	*/
	hr = reflection->GetResourceBindingDesc(resourceIndex, &inputDesc);
	assert(hr == S_OK);
	auto it = s_TypesMap.find(inputDesc.Type);
	uint32_t index = 0xffffffff;
	if (it != s_TypesMap.end())
		index = ((uint32_t)it->second << 16);
	index += inputDesc.BindPoint;
	m_BuffersMap[index] = inputDesc;
	m_BuffersMapName[index] = inputDesc.Name;

	if (inputDesc.Type == D3D_SIT_CBUFFER)
	{
		ID3D12ShaderReflectionConstantBuffer* pCB = reflection->GetConstantBufferByIndex(inputDesc.uID);

		D3D12_SHADER_BUFFER_DESC cbDesc;
		pCB->GetDesc(&cbDesc);

		m_BuffersMapSizes[index] = cbDesc;
	}
}

void Lust::D3D12ShaderReflector::PushRootElement(const D3D12_ROOT_PARAMETER& param, uint32_t rootIndex)
{
	uint32_t index = 0xffffffff;
	auto context = Application::GetInstance()->GetContext();
	auto it = s_ReturnTypesMap.find(param.ParameterType);
	if (it != s_ReturnTypesMap.end())
	{
		index = ((uint32_t)it->second << 16);
		index += param.Constants.ShaderRegister;
	}
	auto buffersIt = m_BuffersMap.find(index);
	size_t bufferSize = 0;
	D3D12_SHADER_INPUT_BIND_DESC bufferDesc = buffersIt != m_BuffersMap.end() ? buffersIt->second : D3D12_SHADER_INPUT_BIND_DESC{};

	auto bufferNamesIt = m_BuffersMapName.find(index);
	std::string bufferName = bufferNamesIt != m_BuffersMapName.end() ? bufferNamesIt->second : "";

	if (bufferDesc.Type == D3D_SIT_CBUFFER)
	{
		auto bufferMapIt = m_BuffersMapSizes.find(index);
		D3D12_SHADER_BUFFER_DESC cbDesc = bufferMapIt != m_BuffersMapSizes.end() ? bufferMapIt->second : D3D12_SHADER_BUFFER_DESC{};
		bufferSize = cbDesc.Size;
	}
	if (bufferDesc.Type == D3D_SIT_STRUCTURED)
	{
		bufferSize = bufferDesc.NumSamples;
	}

	switch (bufferDesc.Type)
	{

	case D3D_SIT_CBUFFER:
	{
		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS)
		{
			m_SmallBufferLayout.Upload(SmallBufferElement(0, bufferSize, rootIndex, context->GetSmallBufferAttachment(), bufferName));
		}
		else
		{
			m_UniformLayout.Upload(UniformElement(BufferType::UNIFORM_CONSTANT_BUFFER, bufferSize, 0, 0, rootIndex, AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 0, bufferName));
		}
		break;
	}
	case D3D_SIT_STRUCTURED:
		m_StructuredBufferLayout.Upload(StructuredBufferElement(0, rootIndex, 0, 0, bufferSize, AccessLevel::ROOT_BUFFER, context->GetUniformAttachment(), bufferName, m_NumberOfInstances));
		break;
	default:
		break;
	}
}

void Lust::D3D12ShaderReflector::PushDescriptorTable(const D3D12_ROOT_PARAMETER& param, uint32_t rootIndex)
{
	uint32_t index = 0xffffffff;
	uint32_t descriptorOffset = 0;
	auto context = Application::GetInstance()->GetContext();
	for (size_t j = 0; j < param.DescriptorTable.NumDescriptorRanges; j++)
	{
		for (size_t k = 0; k < param.DescriptorTable.pDescriptorRanges[j].NumDescriptors; k++, descriptorOffset++)
		{
			auto it = s_ReturnRangeTypesMap.find(param.DescriptorTable.pDescriptorRanges[j].RangeType);
			if (it != s_ReturnRangeTypesMap.end())
			{
				index = ((uint32_t)it->second << 16);
				index += (param.DescriptorTable.pDescriptorRanges[j].BaseShaderRegister + k);
			}

			auto buffersIt = m_BuffersMap.find(index);
			size_t bufferSize = 0;
			D3D12_SHADER_INPUT_BIND_DESC bufferDesc = buffersIt != m_BuffersMap.end() ? buffersIt->second : D3D12_SHADER_INPUT_BIND_DESC{};

			auto bufferNamesIt = m_BuffersMapName.find(index);
			std::string bufferName = bufferNamesIt != m_BuffersMapName.end() ? bufferNamesIt->second : "";

			if (bufferDesc.Type == D3D_SIT_CBUFFER)
			{
				auto bufferMapIt = m_BuffersMapSizes.find(index);
				D3D12_SHADER_BUFFER_DESC cbDesc = bufferMapIt != m_BuffersMapSizes.end() ? bufferMapIt->second : D3D12_SHADER_BUFFER_DESC{};
				bufferSize = cbDesc.Size;
			}
			if (bufferDesc.Type == D3D_SIT_STRUCTURED)
			{
				bufferSize = bufferDesc.NumSamples;
			}
			switch (bufferDesc.Type)
			{
			case D3D_SIT_CBUFFER:
				//BufferType bufferType, size_t size, uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, AccessLevel accessLevel, uint32_t numberOfBuffers, uint32_t bufferAttachment, uint32_t tableIndex, std::string name
				m_UniformLayout.Upload(UniformElement(BufferType::UNIFORM_CONSTANT_BUFFER, bufferSize, 0, 0, rootIndex, AccessLevel::DESCRIPTOR_BUFFER, 1, context->GetUniformAttachment(), descriptorOffset, bufferName));
				break;
			case D3D_SIT_TEXTURE:
				//uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t textureIndex, std::string name
				m_TextureLayout.Upload(TextureElement(0, 0, rootIndex, descriptorOffset, bufferName));
				break;
			case D3D_SIT_SAMPLER:
				/*SamplerFilter filter, AnisotropicFactor anisotropicFactor, AddressMode addressMode, ComparisonPassMode comparisonPassMode,
	uint32_t bindingSlot, uint32_t spaceSet, uint32_t shaderRegister, uint32_t samplerIndex, const std::string& name*/
				m_SamplerLayout.Upload(SamplerElement(0, 0, rootIndex, descriptorOffset, bufferName));
				break;
			case D3D_SIT_STRUCTURED:
				/*
				uint32_t bindingSlot, uint32_t shaderRegister, uint32_t spaceSet, uint32_t bufferIndex, size_t stride,
	AccessLevel accessLevel, size_t bufferAlignment, std::string name, uint32_t numberOfElements
				*/
				m_StructuredBufferLayout.Upload(StructuredBufferElement(0, rootIndex, 0, descriptorOffset, bufferSize, AccessLevel::DESCRIPTOR_BUFFER, context->GetUniformAttachment(), bufferName, m_NumberOfInstances));
				break;
			default:
				break;
			}
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

	D3D12_SHADER_DESC shader_desc;
	m_ShaderReflections[shader_stage.data()]->GetDesc(&shader_desc);

	for (size_t i = 0; i < shader_desc.BoundResources; i++)
	{
		PushExternalElementPreInfo(m_ShaderReflections[shader_stage.data()].Get(), i);
	}
}

void Lust::D3D12ShaderReflector::ReflectRootSignature()
{
	auto rootSigDesc = m_RootSigDeserializer->GetRootSignatureDesc();
	for (uint32_t i = 0; i < rootSigDesc->NumParameters; i++) {
		auto param = rootSigDesc->pParameters[i];
		uint32_t index = 0xffffffff;
		if (param.ParameterType != D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			PushRootElement(param, i);
		}
		else
		{
			PushDescriptorTable(param, i);
		}
	}
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
