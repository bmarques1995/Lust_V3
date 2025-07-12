#pragma once

#include <string>
#include <unordered_map>
#include "LustDLLMacro.hpp"
#include "Languages.hpp"

namespace Lust
{
	class LUST_API Translation
	{
		friend class Translator;
	public:
		Translation();
		~Translation();

		const std::string& GetName();
		const std::u8string& GetTranslation(const std::string& context, const std::string& key);
		const Language& GetLanguage();

	private:
		Language m_Language;
		std::string m_Name;
		std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>> m_Translations;
	};
}