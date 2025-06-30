#include "Languages.hpp"

const std::unordered_map<std::string, Lust::Language> Lust::LanguageDefinitions::s_LanguageMapper =
{
	{"EN_US", Lust::Language::EN_US},
	{"PT_BR", Lust::Language::PT_BR},
	{"ES_MX", Lust::Language::ES_MX},
	{"FR_CA", Lust::Language::FR_CA},
	{"JA_JP", Lust::Language::JA_JP}
};

const std::unordered_map<Lust::Language, std::string> Lust::LanguageDefinitions::s_LanguageReverseMapper =
{
	{Language::EN_US, "EN_US"},
	{Language::PT_BR, "PT_BR"},
	{Language::ES_MX, "ES_MX"},
	{Language::FR_CA, "FR_CA"},
	{Language::JA_JP, "JA_JP"}
};