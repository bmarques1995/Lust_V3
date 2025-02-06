#include "Compiler.hpp"
#include <regex>
#include "Console.hpp"
#include "FileHandler.hpp"
#include "CompilerExceptions.hpp"
#include <json/json.h>

const std::list<std::pair<uint32_t, uint32_t>> Lust::Compiler::s_ValidHLSL =
{
	{6, 0},
	{6, 1},
	{6, 2},
	{6, 3},
	{6, 4},
	{6, 5},
	{6, 6},
	{6, 7},
	{6, 8}
};

const std::unordered_map<std::string, bool> Lust::Compiler::s_Keywords =
{
	{"AppendStructuredBuffer", false},
	{"BlendState", false},
	{"bool", true},
	{"break", false},
	{"Buffer", false},
	{"ByteAddressBuffer", false},
	{"case", false},
	{"cbuffer", false},
	{"centroid", false},
	{"class", false},
	{"column_major", false},
	{"compile", false},
	{"CompileShader", false},
	{"const", false},
	{"continue", false},
	{"ConsumeStructuredBuffer", false},
	{"default", false},
	{"DepthStencilState", false},
	{"discard", false},
	{"do", false},
	{"double", true},
	{"else", false},
	{"export", false},
	{"extern", false},
	{"false", false},
	{"float", true},
	{"for", false},
	{"GeometryShader", false},
	{"groupshared", false},
	{"half", true},
	{"if", false},
	{"in", false},
	{"inline", false},
	{"inout", false},
	{"InputPatch", false},
	{"int", true},
	{"interface", false},
	{"line", false},
	{"lineadj", false},
	{"linear", false},
	{"LineStream", false},
	{"matrix", false},
	{"min16float", true},
	{"min10float", true},
	{"min16int", true},
	{"min12int", true},
	{"min16uint", true},
	{"namespace", false},
	{"nointerpolation", false},
	{"noperspective", false},
	{"NULL", false},
	{"out", false},
	{"OutputPatch", false},
	{"packoffset", false},
	{"pass", false},
	{"PixelShader", false},
	{"point", false},
	{"PointStream", false},
	{"precise", false},
	{"RasterizerState", false},
	{"return", false},
	{"register", false},
	{"row_major", false},
	{"RWBuffer", false},
	{"RWByteAddressBuffer", false},
	{"RWStructuredBuffer", false},
	{"RWTexture1D", false},
	{"RWTexture1DArray", false},
	{"RWTexture2D", false},
	{"RWTexture2DArray", false},
	{"RWTexture3D", false},
	{"sample", false},
	{"sampler", false},
	{"SamplerState", false},
	{"SamplerComparisonState", false},
	{"shared", false},
	{"snorm", false},
	{"static", false},
	{"string", false},
	{"struct", false},
	{"switch", false},
	{"StructuredBuffer", false},
	{"tbuffer", false},
	{"technique", false},
	{"technique10", false},
	{"technique11", false},
	{"texture", false},
	{"Texture1D", false},
	{"Texture1DArray", false},
	{"Texture2D", false},
	{"Texture2DArray", false},
	{"Texture2DMS", false},
	{"Texture2DMSArray", false},
	{"Texture3D", false},
	{"TextureCube", false},
	{"TextureCubeArray", false},
	{"true", false},
	{"typedef", false},
	{"triangle", false},
	{"triangleadj", false},
	{"TriangleStream", false},
	{"uint", true},
	{"uniform", false},
	{"unorm", false},
	{"unsigned", false},
	{"vector", false},
	{"VertexShader", false},
	{"void", false},
	{"volatile", false},
	{"while", false},
};

const std::unordered_map<std::string, bool> Lust::Compiler::s_SysValues =
{
	{"CLIPDISTANCE", true},
	{"CULLDISTANCE", true},
	{"COVERAGE", false},
	{"DEPTH", false},
	{"DEPTHGREATEREQUAL", false},
	{"DEPTHLESSEQUAL", false},
	{"DISPATCHTHREADID", false},
	{"DOMAINLOCATION", false},
	{"GROUPID", false},
	{"GROUPINDEX", false},
	{"GROUPTHREADID", false},
	{"GSINSTANCEID", false},
	{"INNERCOVERAGE", false},
	{"INSIDETESSFACTOR", false},
	{"INSTANCEID", false},
	{"ISFRONTFACE", false},
	{"OUTPUTCONTROLPOINTID", false},
	{"POSITION", false},
	{"PRIMITIVEID", false},
	{"RENDERTARGETARRAYINDEX", false},
	{"SAMPLEINDEX", false},
	{"STENCILREF", false},
	{"TARGET", true},
	{"TESSFACTOR", false},
	{"VERTEXID", false},
	{"VIEWPORTARRAYINDEX", false},
	{"SHADINGRATE", false},
};

const std::list<std::string> Lust::Compiler::s_BuiltinFunctions =
{
	{"abort"},
	{"abs"},
	{"acos"},
	{"all"},
	{"AllMemoryBarrier"},
	{"AllMemoryBarrierWithGroupSync"},
	{"any"},
	{"asdouble"},
	{"asfloat"},
	{"asin"},
	{"asint"},
	{"asuint"},
	{"asuint"},
	{"atan"},
	{"atan2"},
	{"ceil"},
	{"CheckAccessFullyMapped"},
	{"clamp"},
	{"clip"},
	{"cos"},
	{"cosh"},
	{"countbits"},
	{"cross"},
	{"D3DCOLORtoUBYTE4"},
	{"ddx"},
	{"ddx_coarse"},
	{"ddx_fine"},
	{"ddy"},
	{"ddy_coarse"},
	{"ddy_fine"},
	{"degrees"},
	{"determinant"},
	{"DeviceMemoryBarrier"},
	{"DeviceMemoryBarrierWithGroupSync"},
	{"distance"},
	{"dot"},
	{"dst"},
	{"errorf"},
	{"EvaluateAttributeCentroid"},
	{"EvaluateAttributeAtSample"},
	{"EvaluateAttributeSnapped"},
	{"exp"},
	{"exp2"},
	{"f16tof32"},
	{"f32tof16"},
	{"faceforward"},
	{"firstbithigh"},
	{"firstbitlow"},
	{"floor"},
	{"fma"},
	{"fmod"},
	{"frac"},
	{"frexp"},
	{"fwidth"},
	{"GetRenderTargetSampleCount"},
	{"GetRenderTargetSamplePosition"},
	{"GroupMemoryBarrier"},
	{"GroupMemoryBarrierWithGroupSync"},
	{"InterlockedAdd"},
	{"InterlockedAnd"},
	{"InterlockedCompareExchange"},
	{"InterlockedCompareStore"},
	{"InterlockedExchange"},
	{"InterlockedMax"},
	{"InterlockedMin"},
	{"InterlockedOr"},
	{"InterlockedXor"},
	{"isfinite"},
	{"isinf"},
	{"isnan"},
	{"ldexp"},
	{"length"},
	{"lerp"},
	{"lit"},
	{"log"},
	{"log10"},
	{"log2"},
	{"mad"},
	{"max"},
	{"min"},
	{"modf"},
	{"msad4"},
	{"mul"},
	{"noise"},
	{"normalize"},
	{"pow"},
	{"printf"},
	{"Process2DQuadTessFactorsAvg"},
	{"Process2DQuadTessFactorsMax"},
	{"Process2DQuadTessFactorsMin"},
	{"ProcessIsolineTessFactors"},
	{"ProcessQuadTessFactorsAvg"},
	{"ProcessQuadTessFactorsMax"},
	{"ProcessQuadTessFactorsMin"},
	{"ProcessTriTessFactorsAvg"},
	{"ProcessTriTessFactorsMax"},
	{"ProcessTriTessFactorsMin"},
	{"radians"},
	{"rcp"},
	{"reflect"},
	{"refract"},
	{"reversebits"},
	{"round"},
	{"rsqrt"},
	{"saturate"},
	{"sign"},
	{"sin"},
	{"sincos"},
	{"sinh"},
	{"smoothstep"},
	{"sqrt"},
	{"step"},
	{"tan"},
	{"tanh"},
	{"tex1D"},
	{"tex1D"},
	{"tex1Dbias"},
	{"tex1Dgrad"},
	{"tex1Dlod"},
	{"tex1Dproj"},
	{"tex2D"},
	{"tex2D"},
	{"tex2Dbias"},
	{"tex2Dgrad"},
	{"tex2Dlod"},
	{"tex2Dproj"},
	{"tex3D"},
	{"tex3D"},
	{"tex3Dbias"},
	{"tex3Dgrad"},
	{"tex3Dlod"},
	{"tex3Dproj"},
	{"texCUBE"},
	{"texCUBE"},
	{"texCUBEbias"},
	{"texCUBEgrad"},
	{"texCUBElod"},
	{"texCUBEproj"},
	{"transpose"},
	{"trunc"}
};

const std::unordered_map<std::string, Lust::PipelineStage> Lust::Compiler::s_ShaderStages =
{
	{"vs", PipelineStage::VERTEX_STAGE},
	{"ps", PipelineStage::PIXEL_STAGE},
	{"hs", PipelineStage::HULL_STAGE},
	{"ds", PipelineStage::DOMAIN_STAGE},
	{"gs", PipelineStage::GEOMETRY_STAGE},
	{"cs", PipelineStage::COMPUTE_STAGE},
	{"ms", PipelineStage::MESH_STAGE},
	{"as", PipelineStage::AMPLIFICATION_STAGE},
	{"lib", PipelineStage::RAYTRACING_STAGE}
};

Lust::Compiler::Compiler(std::string_view backendExtension, std::string_view graphicsAPIExtension, std::string baseEntry, std::string hlslFeatureLevel) :
	m_PackedShaders(true),
	m_BackendExtension(backendExtension),
	m_GraphicsAPIExtension(graphicsAPIExtension),
	m_BaseEntry(baseEntry),
	m_DebugMode(true)
{
	ValidateHLSLFeatureLevel(hlslFeatureLevel);
	m_HLSLFeatureLevel = hlslFeatureLevel;
	
}

Lust::Compiler::~Compiler()
{
}

void Lust::Compiler::SetBaseEntry(std::string baseEntry)
{
	m_BaseEntry = baseEntry;
}

void Lust::Compiler::PushShaderPath(std::string filepath, PipelineType pipelineType)
{
	std::regex pattern("^(.*[\\/])([^\\/]+)\\.hlsl$");

	std::smatch matches;

	// Check if the input matches the pattern
	if (std::regex_match(filepath, matches, pattern))
	{
		std::stringstream buffer;
		buffer << matches[1].str();
		buffer << matches[2].str();		
	}
	else
	{
		throw InvalidFilepathException("Invalid filename");
	}
	std::pair<std::string, PipelineType> shaderPath;
	shaderPath.first = filepath;
	shaderPath.second = pipelineType;
	m_ShaderFilepaths.push_back(shaderPath);
}

void Lust::Compiler::SetBuildMode(bool isDebug)
{
	m_DebugMode = isDebug;
}

void Lust::Compiler::SetHLSLFeatureLevel(std::string version)
{
	ValidateHLSLFeatureLevel(version);
	m_HLSLFeatureLevel = version;
}

void Lust::Compiler::ValidateHLSLFeatureLevel(std::string version)
{
	std::regex pattern("^_(\\d+)_(\\d+)$");
	std::smatch matches;
	uint32_t major = 0;
	uint32_t minor = 0;
	if (std::regex_search(version, matches, pattern)) {
		std::stringstream buffer;
		buffer << matches[1].str() << " " << matches[2].str();
		std::istringstream reader(buffer.str());
		reader >> major >> minor;
	}
	else
	{
		throw InvalidHLSLVersion("The HLSL version must match the pattern \"^_(\\d+ )_(\\d+)$\"");
	}
	auto it = SearchHLSLVersion(std::make_pair(major, minor));
	if (it == s_ValidHLSL.end())
	{
		std::stringstream valid_hlsl;
		valid_hlsl << "\"";
		for (auto it = s_ValidHLSL.begin(); it != s_ValidHLSL.end(); it++)
			valid_hlsl << "_" << it->first << "_" << it->second << "|";
		valid_hlsl << "\"";
		throw InvalidHLSLVersion("The valid HLSL versions are: {}");
	}
}

bool Lust::Compiler::CompileStage(std::string source, std::string stage, std::string basepath)
{
	HRESULT hr;
	std::stringstream buffer;
	std::string outputPath;

	buffer << basepath << "." << stage << m_BackendExtension;
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
		.Ptr = (void *) &*source.begin(),
		.Size = (uint32_t) source.size(),
		.Encoding = 0
	};

	ComPointer<IDxcResult> result;
	hr = compiler->Compile(&srcBuffer, m_ArgList.data(),(uint32_t) m_ArgList.size(), nullptr, IID_PPV_ARGS(result.GetAddressOf()));

	blob.Release();
	hr = result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(blob.GetAddressOf()), nullptr);

	if (blob->GetBufferSize() == 0)
	{
		hr = result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errorBlob.GetAddressOf()), nullptr);
		Console::CoreError("DXC Status: {}", (const char*)errorBlob->GetBufferPointer());
	}
	else
		FileHandler::WriteBinFile(outputPath, (std::byte*)blob->GetBufferPointer(), blob->GetBufferSize());
	
	return !(blob->GetBufferSize() == 0);
}

void Lust::Compiler::ReadShaderSource(std::string_view path, std::string* shaderSource)
{
	if (!FileHandler::FileExists(path))
		throw InvalidFilepathException("File not found");
	
	FileHandler::ReadTextFile(path, shaderSource);
}

std::list<std::string>::const_iterator Lust::Compiler::SearchBuiltinName(std::string name)
{
	for (auto it = s_BuiltinFunctions.begin(); it != s_BuiltinFunctions.end(); it++)
		if (it->compare(name) == 0)
			return it;
	return s_BuiltinFunctions.end();
}

std::list<std::pair<uint32_t, uint32_t>>::const_iterator Lust::Compiler::SearchHLSLVersion(std::pair<uint32_t, uint32_t> value)
{
	for (auto it = s_ValidHLSL.begin(); it != s_ValidHLSL.end(); it++)
		if ((it->first == value.first) && (it->second == value.second))
			return it;
	return s_ValidHLSL.end();
}

void Lust::Compiler::ValidateNameOverKeywords(std::string name)
{
	std::regex variablePattern("[a-zA-Z_][a-zA-Z0-9_]*$");
	std::regex typeExtended("([1-4]|[1-4]x[1-4])$");
	std::regex typeExtendedCapture("^(.*?)([1-4]|[1-4]x[1-4])$");
	std::smatch matches;
	std::string subType;
	bool usesSizer = false;
	if (!std::regex_match(name, matches, variablePattern))
		throw InvalidNameException("The name must start with a letter or \"_\" and can only be followed by a letter a digit or \"_\"");
	if (std::regex_match(name, matches, typeExtended))
	{
		std::regex_match(name, matches, typeExtendedCapture);
		subType = matches[1].str();
		usesSizer = true;
	}
	else
	{
		subType = name;
		usesSizer = false;
	}

	auto it = s_Keywords.find(subType);
	if (it != s_Keywords.end())
		if(it->second == usesSizer)
			throw InvalidNameException("You cannot use HLSL keywords");
}

void Lust::Compiler::ValidateNameOverSysValues(std::string name)
{
	std::regex svPattern("^SV_(\\w+)");
	std::regex enumerationPattern("^(\\D+)(\\d+)$");

	std::smatch matches;
	std::string treatedName;
	bool enumerated = false;

	if (std::regex_search(name, matches, svPattern))
	{
		treatedName = matches[1].str();
	}
	else
	{
		return;
	}

	if (std::regex_search(treatedName, matches, enumerationPattern))
	{
		treatedName = matches[1].str();
		enumerated = true;
	}

	auto it = s_SysValues.find(treatedName);
	if (it != s_SysValues.end())
		if (it->second == enumerated)
		{
			std::stringstream error;
			std::string castedError;
			error << name << " is a HLSL System Value (SV_) and cannot be used";
			castedError = error.str();
			throw InvalidNameException(castedError);
		}
}

void Lust::Compiler::ValidateNameOverBuiltinFunctions(std::string name)
{
	auto it = SearchBuiltinName(name);
	if (it != s_BuiltinFunctions.end())
	{
		std::stringstream error;
		std::string castedError;
		error << it->c_str() << " is a builtin function";
		castedError = error.str();
		throw InvalidNameException(castedError);
	}
}

bool Lust::Compiler::ValidatePipeline(uint32_t stages, PipelineType pipelineType)
{
	uint32_t validStageCombination;
	uint32_t mandatoryStageCombination1;
	uint32_t mandatoryStageCombination2;
	uint32_t filteredStages;
	uint32_t invalidStages;
	uint32_t invalidStagesAusent;

	switch (pipelineType)
	{
	case Lust::PipelineType::Graphics:
	{
		mandatoryStageCombination1 = (uint32_t)PipelineStage::VERTEX_STAGE | (uint32_t)PipelineStage::PIXEL_STAGE;
		mandatoryStageCombination2 = (uint32_t)PipelineStage::VERTEX_STAGE | (uint32_t)PipelineStage::HULL_STAGE |
			(uint32_t)PipelineStage::DOMAIN_STAGE | (uint32_t)PipelineStage::PIXEL_STAGE;
		filteredStages = stages & mandatoryStageCombination2;
		invalidStages = (uint32_t)PipelineStage::MESH_STAGE | (uint32_t)PipelineStage::AMPLIFICATION_STAGE |
			(uint32_t)PipelineStage::RAYTRACING_STAGE | (uint32_t)PipelineStage::COMPUTE_STAGE;
		invalidStagesAusent = (invalidStages ^ (stages & invalidStages));
		return (((filteredStages == mandatoryStageCombination1) || (filteredStages == mandatoryStageCombination2)) && (invalidStagesAusent == invalidStages));
	}
	case Lust::PipelineType::Compute:
	{
		validStageCombination = (uint32_t)PipelineStage::COMPUTE_STAGE;
		filteredStages = stages & validStageCombination;
		invalidStages = (uint32_t)PipelineStage::MESH_STAGE | (uint32_t)PipelineStage::AMPLIFICATION_STAGE |
			(uint32_t)PipelineStage::RAYTRACING_STAGE | (uint32_t)PipelineStage::VERTEX_STAGE |
			(uint32_t)PipelineStage::HULL_STAGE | (uint32_t)PipelineStage::DOMAIN_STAGE |
			(uint32_t)PipelineStage::PIXEL_STAGE | (uint32_t)PipelineStage::GEOMETRY_STAGE;
		invalidStagesAusent = (invalidStages ^ (stages & invalidStages));
		return ((stages == filteredStages) && (invalidStagesAusent == invalidStages));
	}
	case Lust::PipelineType::Mesh:
	{
		mandatoryStageCombination1 = (uint32_t)PipelineStage::MESH_STAGE | (uint32_t)PipelineStage::PIXEL_STAGE;
		filteredStages = stages & mandatoryStageCombination1;
		invalidStages = (uint32_t)PipelineStage::VERTEX_STAGE | (uint32_t)PipelineStage::HULL_STAGE |
			(uint32_t)PipelineStage::RAYTRACING_STAGE | (uint32_t)PipelineStage::COMPUTE_STAGE |
			(uint32_t)PipelineStage::GEOMETRY_STAGE | (uint32_t)PipelineStage::DOMAIN_STAGE;
		invalidStagesAusent = (invalidStages ^ (stages & invalidStages));
		return ((filteredStages == mandatoryStageCombination1) && (invalidStagesAusent == invalidStages));
	}
	case Lust::PipelineType::RayTracing:
	{	
		validStageCombination = (uint32_t)PipelineStage::RAYTRACING_STAGE;
		filteredStages = stages & validStageCombination;
		invalidStages = (uint32_t)PipelineStage::MESH_STAGE | (uint32_t)PipelineStage::AMPLIFICATION_STAGE |
			(uint32_t)PipelineStage::COMPUTE_STAGE | (uint32_t)PipelineStage::VERTEX_STAGE |
			(uint32_t)PipelineStage::HULL_STAGE | (uint32_t)PipelineStage::DOMAIN_STAGE |
			(uint32_t)PipelineStage::PIXEL_STAGE | (uint32_t)PipelineStage::GEOMETRY_STAGE;
		invalidStagesAusent = (invalidStages ^ (stages & invalidStages));
		return (stages == filteredStages) && (invalidStagesAusent == invalidStages);
	}
	default:
		return false;
	}
}
