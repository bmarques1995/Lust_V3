#include "Translation.hpp"

Lust::Translation::Translation() :
	m_Name(""), m_Language(Language::EN_US)
{
}

Lust::Translation::~Translation()
{
}

const std::string& Lust::Translation::GetName()
{
	return m_Name;
}

const std::u8string& Lust::Translation::GetTranslation(const std::string& context, const std::string& key)
{
	return m_Translations[context][key];
}

const Lust::Language& Lust::Translation::GetLanguage()
{
	return m_Language;
}
