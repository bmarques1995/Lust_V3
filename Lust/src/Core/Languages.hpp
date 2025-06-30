#pragma once
#include <unordered_map>
#include <string>
#include "LustDLLMacro.hpp"

namespace Lust
{
	enum Language
	{
		EN_US,
		PT_BR,
		ES_MX,
		FR_CA,
		JA_JP
	};

	class LUST_API LanguageDefinitions
	{
	public:
		static const std::unordered_map<std::string, Language> s_LanguageMapper;
		static const std::unordered_map<Language, std::string> s_LanguageReverseMapper;
	};
} 