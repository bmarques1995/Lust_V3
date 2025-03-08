#include "VKShaderReflector.hpp"
#include <cstring>
#include "FileHandler.hpp" 
#include <Application.hpp>
#include <Stack>

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
	if (m_Data != nullptr)
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

Lust::VKShaderReflector::VKShaderReflector(std::string_view jsonFilepath, uint32_t stages, uint32_t numInstances) :
	ShaderReflector(stages, numInstances)
{
	InitJsonAndPaths(jsonFilepath);
	m_InputBufferLayout.Clear();
	m_SmallBufferLayout.Clear();
	for (auto it = s_GraphicsPipelineStages.begin(); it != s_GraphicsPipelineStages.end(); it++)
	{
		UploadBlob(*it, &m_ShaderBlobs[*it]);
		ReflectStage(*it, m_ShaderBlobs[*it]);
		GenerateSmallBufferLayout(&m_ShaderReflections[*it]);
		GenerateBuffersLayout(&m_ShaderReflections[*it]);
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
		if ((*it)->name)
		{
			InputBufferElement ibe(CastToShaderDataType((*it)->format), (*it)->name, false);
			m_InputBufferLayout.PushBack(ibe);
		}
	}
}

void Lust::VKShaderReflector::GenerateSmallBufferLayout(const SpvReflectShaderModule* module)
{
	if (module->entry_point_name == nullptr)
		return;
	uint32_t pushConstantCount = 0;
	spvReflectEnumeratePushConstantBlocks(module, &pushConstantCount, nullptr);
	std::vector<SpvReflectBlockVariable*> pushConstants(pushConstantCount);
	spvReflectEnumeratePushConstantBlocks(module, &pushConstantCount, pushConstants.data());
	auto context = Application::GetInstance()->GetContext();
	for (auto it = pushConstants.begin(); it != pushConstants.end(); it++)
	{
		if (m_SmallBufferLayout.GetElements().find((*it)->name) == m_SmallBufferLayout.GetElements().end())
		{
			SmallBufferElement sbe((*it)->offset, (*it)->size, 0, context->GetSmallBufferAttachment(), (*it)->name);
			m_SmallBufferLayout.Upload(sbe);
		}
	}
}

void Lust::VKShaderReflector::GenerateBuffersLayout(const SpvReflectShaderModule* module)
{
	static const std::unordered_map<SpvReflectDescriptorType, std::function<void(SpvReflectDescriptorBinding**, VKShaderReflector*)>> s_ElementCreators =
	{
		{ SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE, std::bind(&VKShaderReflector::CreateTextureElement, std::placeholders::_1, std::placeholders::_2) },
		{ SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER, std::bind(&VKShaderReflector::CreateSamplerElement, std::placeholders::_1, std::placeholders::_2) },
		{ SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER, std::bind(&VKShaderReflector::CreateUniformElement, std::placeholders::_1, std::placeholders::_2) },
		{ SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER, std::bind(&VKShaderReflector::CreateStructuredBufferElement, std::placeholders::_1, std::placeholders::_2) },
	};
	uint32_t set_count = 0;
	spvReflectEnumerateDescriptorSets(module, &set_count, nullptr);
	std::vector<SpvReflectDescriptorSet*> sets(set_count);

	spvReflectEnumerateDescriptorSets(module, &set_count, sets.data());
	for (auto it = sets.begin(); it != sets.end(); it++)
	{
		for (size_t i = 0; i < (*it)->binding_count; i++)
		{
			SpvReflectDescriptorBinding* binder = (*it)->bindings[i];
			auto it = s_ElementCreators.find(binder->descriptor_type);
			if (it != s_ElementCreators.end())
			{
				it->second(&binder, this);
			}
		}
	}
	Console::CoreLog("sets: {}", set_count);
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

	blob->RecreateBuffer((uint8_t*)blobData, blobSize);

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

void Lust::VKShaderReflector::CreateUniformElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance)
{
	auto context = Application::GetInstance()->GetContext();
	UniformElement ue(BufferType::UNIFORM_CONSTANT_BUFFER, (*reflector_binder)->block.size, (*reflector_binder)->binding, (*reflector_binder)->set, 0,
		AccessLevel::ROOT_BUFFER, 1, context->GetUniformAttachment(), 1, (*reflector_binder)->name);
	instance->m_UniformLayout.Upload(ue);
}

void Lust::VKShaderReflector::CreateTextureElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance)
{
}

void Lust::VKShaderReflector::CreateSamplerElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance)
{
}

void Lust::VKShaderReflector::CreateStructuredBufferElement(SpvReflectDescriptorBinding** reflector_binder, VKShaderReflector* instance)
{ 
	auto context = Application::GetInstance()->GetContext();
	auto b_block = (*reflector_binder)->block;
#if 0
	size_t recursivebufferStride = GetStructuredBufferStrideRecursively((*reflector_binder)->type_description);
#endif
	size_t bufferStride = GetStructuredBufferStride((*reflector_binder)->type_description);
	StructuredBufferElement sbe((*reflector_binder)->binding, (uint32_t)0, (*reflector_binder)->set, 0, bufferStride,
		AccessLevel::ROOT_BUFFER, context->GetUniformAttachment(), (*reflector_binder)->name, instance->m_NumberOfInstances);
	instance->m_StructuredBufferLayout.Upload(sbe);
}

size_t Lust::VKShaderReflector::GetStructuredBufferStrideRecursively(const SpvReflectTypeDescription* type_desc)
{
	size_t stride = 0;
	if (type_desc->member_count == 0)
	{
		if(type_desc->traits.numeric.matrix.column_count == 0)
			return ((type_desc->traits.numeric.scalar.width >> 3) * type_desc->traits.numeric.vector.component_count );
		else
			return ((type_desc->traits.numeric.scalar.width >> 3) * type_desc->traits.numeric.matrix.row_count * type_desc->traits.numeric.matrix.column_count);
	}
	for (size_t i = 0; i < type_desc->member_count; i++)
	{
		stride += GetStructuredBufferStrideRecursively(&type_desc->members[i]);
	}
	return stride;
}

size_t Lust::VKShaderReflector::GetStructuredBufferStride(const SpvReflectTypeDescription* type_desc)
{
	size_t stride = 0;
	std::stack<const SpvReflectTypeDescription*> stack;
	stack.push(type_desc);

	while (!stack.empty()) {
		const SpvReflectTypeDescription* current = stack.top();
		stack.pop();

		if (current->member_count == 0) {
			if (current->traits.numeric.matrix.column_count == 0)
				stride += ((current->traits.numeric.scalar.width >> 3) * current->traits.numeric.vector.component_count);
			else
				stride += ((current->traits.numeric.scalar.width >> 3) * current->traits.numeric.matrix.row_count * current->traits.numeric.matrix.column_count);
		}
		else {
			for (size_t i = 0; i < current->member_count; i++) {
				stack.push(&current->members[i]);
			}
		}
	}

	return stride;
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
