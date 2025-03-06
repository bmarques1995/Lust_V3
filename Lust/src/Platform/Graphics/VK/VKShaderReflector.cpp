#include "VKShaderReflector.hpp"
#include <cstring>
#include "FileHandler.hpp" 

Lust::RawBuffer::RawBuffer()
{
    m_Data = nullptr;
    m_Size = 0;
}

Lust::RawBuffer::RawBuffer(uint8_t* data, size_t size)
{
    m_Size = size;
	m_Data = new uint8_t[m_Size];
    memcpy(m_Data, data, m_Size);
}

Lust::RawBuffer::~RawBuffer()
{
    if(m_Data != nullptr)
        delete[] m_Data;
}

void Lust::RawBuffer::RecreateBuffer(uint8_t* data, size_t size)
{
    if (m_Data != nullptr)
        delete[] m_Data;

	m_Size = size;
	m_Data = new uint8_t[m_Size];
	memcpy(m_Data, data, m_Size);
}

const uint8_t* Lust::RawBuffer::GetData() const
{
    return m_Data;
}

const size_t Lust::RawBuffer::GetSize() const
{
    return m_Size;
}

Lust::VKShaderReflector::VKShaderReflector(std::string_view jsonFilepath)
{
    InitJsonAndPaths(jsonFilepath);
	for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
	{
		UploadBlob(*it, &m_ShaderBlobs[*it]);
		ReflectStage(*it, m_ShaderBlobs[*it]);
	}
	GenerateInputBufferLayout();
}

Lust::VKShaderReflector::~VKShaderReflector()
{
}

void Lust::VKShaderReflector::GenerateInputBufferLayout()
{
	SpvReflectResult result;
	uint32_t var_count = 0;
	result = spvReflectEnumerateInputVariables(&m_ShaderReflections["vs"], &var_count, nullptr);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
	std::vector<SpvReflectInterfaceVariable*> input_vars(var_count);
	result = spvReflectEnumerateInputVariables(&m_ShaderReflections["vs"], &var_count, input_vars.data());
	assert(result == SPV_REFLECT_RESULT_SUCCESS);

	for (auto it = input_vars.begin(); it != input_vars.end(); it++)
	{
		InputBufferElement ibe(CastToShaderDataType((*it)->format), (*it)->name, false);
		m_InputBufferLayout.PushBack(ibe);
	}
}

void Lust::VKShaderReflector::UploadBlob(std::string_view shader_stage, RawBuffer* blob)
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

	blob->RecreateBuffer((uint8_t *)blobData, blobSize);

	delete[] blobData;
}

void Lust::VKShaderReflector::ReflectStage(std::string_view shader_stage, const RawBuffer& blob)
{
	if (blob.GetData() == nullptr)
		return;

	SpvReflectResult result;
	result = spvReflectCreateShaderModule(blob.GetSize(), blob.GetData(), &m_ShaderReflections[shader_stage.data()]);
	assert(result == SPV_REFLECT_RESULT_SUCCESS);
}

Lust::ShaderDataType Lust::VKShaderReflector::CastToShaderDataType(SpvReflectFormat format)
{
	static const std::unordered_map<SpvReflectFormat, ShaderDataType> caster =
	{
		{ SPV_REFLECT_FORMAT_R32_SFLOAT, ShaderDataType::Float },
		{ SPV_REFLECT_FORMAT_R32G32_SFLOAT, ShaderDataType::Float2 },
		{ SPV_REFLECT_FORMAT_R32G32B32_SFLOAT, ShaderDataType::Float3 },
		{ SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT, ShaderDataType::Float4 },
		{ SPV_REFLECT_FORMAT_R32_UINT, ShaderDataType::Uint },
		{ SPV_REFLECT_FORMAT_R32G32_UINT, ShaderDataType::Uint2 },
		{ SPV_REFLECT_FORMAT_R32G32B32_UINT, ShaderDataType::Uint3 },
		{ SPV_REFLECT_FORMAT_R32G32B32A32_UINT, ShaderDataType::Uint4 },
	};

	auto it = caster.find(format);
	if (it != caster.end())
		return it->second;
	else
		return ShaderDataType::None;
}
