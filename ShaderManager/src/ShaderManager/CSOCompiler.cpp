#ifdef LUST_SHADER_MNG_USES_WINDOWS

#include "CSOCompiler.hpp"
#include "CompilerExceptions.hpp"
#include <regex>
#include <sstream>
#include <json/json.h>
#include "FileHandler.hpp"
#include "Console.hpp"

Lust::CSOCompiler::CSOCompiler(std::string baseEntry, std::string hlslFeatureLevel) :
	Lust::Compiler(".cso", ".d3d12", baseEntry, hlslFeatureLevel)
{
}

Lust::CSOCompiler::~CSOCompiler()
{
}

void Lust::CSOCompiler::CompilePackedShader()
{
	static const std::regex pattern("^(.*[\\/])([^\\/]+)\\.hlsl$");
	std::smatch matches;

	Json::Value root;
	Json::StreamWriterBuilder builder;
	const std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	uint32_t presentStages = 0;

	for (auto& shaderPath : m_ShaderFilepaths)
	{
		std::string basepath;
		std::regex_match(shaderPath.first, matches, pattern);
		std::vector<std::pair<std::string, Lust::PipelineStage>> objectPresentStages;
		std::stringstream buffer;
		buffer << matches[1].str();
		buffer << matches[2].str();
		basepath = buffer.str();
		buffer.str("");
		std::string shader;
		ReadShaderSource(shaderPath.first, &shader);
		PushRootSignatureArgs("rs_controller");
		CompileRootSignature(shader, basepath);
		buffer << matches[2].str() << ".rs" << m_BackendExtension;
		root["BinShaders"]["rs"]["filename"] = buffer.str();
		buffer.str("");
		m_ArgList.clear();
		PushPresentStages(shaderPath.first, shader, &objectPresentStages);
		for (auto& stage : objectPresentStages)
		{
			PushArgList(stage.first);
			if (CompileStage(shader, stage.first, basepath))
			{
				presentStages |= (uint32_t)stage.second;
				buffer << matches[2].str() << "." << stage.first << m_BackendExtension;
				root["BinShaders"][stage.first]["filename"] = buffer.str();
				buffer.str("");
			}
			m_ArgList.clear();
		}
		if (!ValidatePipeline(presentStages, shaderPath.second))
			throw InvalidPipelineException("Invalid pipeline type");
		root["HLSLFeatureLevel"] = m_HLSLFeatureLevel;
		writer->write(root, &buffer);
		std::string jsonResult = buffer.str();
		buffer.str("");
		buffer << basepath << m_GraphicsAPIExtension << ".json";
		std::string jsonPath = buffer.str();
		buffer.str("");
		FileHandler::WriteTextFile(jsonPath, jsonResult);
		presentStages = 0;
		root.clear();
	}
}

void Lust::CSOCompiler::PushArgList(std::string stage)
{
	std::stringstream buffer;
	std::string tempbuffer;

	buffer << stage << m_BaseEntry;
	tempbuffer = buffer.str();
	ValidateNameOverKeywords(tempbuffer);
	ValidateNameOverSysValues(tempbuffer);
	ValidateNameOverBuiltinFunctions(tempbuffer);
	m_CurrentEntrypointCopy = tempbuffer;
	m_CurrentEntrypoint = std::wstring(tempbuffer.begin(), tempbuffer.end());
	buffer.str("");
	buffer << stage << m_HLSLFeatureLevel;
	tempbuffer = buffer.str();
	m_CurrentFormattedStage = std::wstring(tempbuffer.begin(), tempbuffer.end());
	buffer.str("");

	m_ArgList.push_back(L"-Zpc");
	m_ArgList.push_back(L"-HV");
	m_ArgList.push_back(L"2021");
	m_ArgList.push_back(L"-T");
	m_ArgList.push_back(m_CurrentFormattedStage.c_str());
	m_ArgList.push_back(L"-E");
	m_ArgList.push_back(m_CurrentEntrypoint.c_str());
	if (m_DebugMode)
		m_ArgList.push_back(L"-O0");
	else
		m_ArgList.push_back(L"-O3");
}

void Lust::CSOCompiler::PushRootSignatureArgs(std::string rs_name)
{
	std::stringstream buffer;
	std::string tempbuffer;

	buffer << rs_name;
	tempbuffer = buffer.str();
	ValidateNameOverKeywords(tempbuffer);
	ValidateNameOverSysValues(tempbuffer);
	ValidateNameOverBuiltinFunctions(tempbuffer);
	m_CurrentEntrypointCopy = tempbuffer;
	m_CurrentEntrypoint = std::wstring(tempbuffer.begin(), tempbuffer.end());
	buffer.str("");

	m_ArgList.push_back(L"-HV");
	m_ArgList.push_back(L"2021");
	m_ArgList.push_back(L"-T");
	m_ArgList.push_back(L"rootsig_1_1");
	m_ArgList.push_back(L"-E");
	m_ArgList.push_back(m_CurrentEntrypoint.c_str());
	if (m_DebugMode)
		m_ArgList.push_back(L"-O0");
	else
		m_ArgList.push_back(L"-O3");
}

void Lust::CSOCompiler::CompileRootSignature(std::string source, std::string basepath)
{
	HRESULT hr;
	std::stringstream buffer;
	std::string outputPath;

	buffer << basepath << ".rs" << m_BackendExtension;
	outputPath = buffer.str();
	buffer.str("");

	ComPointer<IDxcBlob> blob;
	ComPointer<IDxcBlob> errorBlob;
	ComPointer<IDxcUtils> utils;
	ComPointer<IDxcCompiler3> compiler;

	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf()));

	DxcBuffer srcBuffer =
	{
		.Ptr = (void*)&*source.begin(),
		.Size = (uint32_t)source.size(),
		.Encoding = 0
	};

	ComPointer<IDxcResult> result;
	hr = compiler->Compile(&srcBuffer, m_ArgList.data(), (uint32_t)m_ArgList.size(), nullptr, IID_PPV_ARGS(result.GetAddressOf()));

	blob.Release();
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr);

	if (blob->GetBufferSize() == 0)
	{
		hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBlob.GetAddressOf()), nullptr);
		Console::CoreError("DXC Status: {}", (const char*)errorBlob->GetBufferPointer());
		throw InvalidPipelineException("Root Signature is Mandatory");
	}
	else
		FileHandler::WriteBinFile(outputPath, (std::byte*)blob->GetBufferPointer(), blob->GetBufferSize());
}

#endif
