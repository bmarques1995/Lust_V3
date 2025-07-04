#pragma once

#include <json/json.h>
#include "Languages.hpp"
#include "LustDLLMacro.hpp"
#include "flatbuffers/flatbuffers.h"
#include "translations_generated.h"
#include "RawBuffer.hpp"

namespace Lust
{
	class LUST_API Translator
	{
	public:
		static void Translate(const std::string& json_basepath);
		static void LoadTranslation(std::string fbd_basepath);
    private:
        static void LoadJson(const std::string& filepath, Json::Value* json_root);
        static void JsonToTranslationMap(const Json::Value& json_context, std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>>* translations);
		static void TranslationMapToFlatbuffers(std::shared_ptr<RawBuffer>* outputBuffer, const std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>>& data);
	};
}
