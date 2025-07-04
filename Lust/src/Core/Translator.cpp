#include "Translator.hpp"
#include <flatbuffers/flatbuffers.h>
#include "FileHandler.hpp"
#include "Console.hpp"

void Lust::Translator::Translate(const std::string& json_basepath)
{
    std::string json_path = json_basepath + ".json";
    std::string fbd_path = json_basepath + ".fbd";
    Json::Value json_context;
    std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>> data;
    LoadJson(json_path, &json_context);
	JsonToTranslationMap(json_context, &data);
    std::shared_ptr<RawBuffer> outputBuffer;
	TranslationMapToFlatbuffers(&outputBuffer, data);
    outputBuffer->DeflateBuffer();
    outputBuffer->CypherBuffer();
    /*
    TODO: Add sha512 checksum (openssl needs compilation)
    TODO: Add aes encryption (openssl needs compilation)
    */
    FileHandler::WriteBinFile(fbd_path, outputBuffer);
}

void Lust::Translator::LoadTranslation(std::string fbd_basepath)
{
    std::shared_ptr<RawBuffer> inputBuffer;
    FileHandler::ReadBinFile(fbd_basepath, &inputBuffer);
    inputBuffer->DecypherBuffer();
    inputBuffer->InflateBuffer();

    // Step 2: Verify the buffer (optional but good practice)
    flatbuffers::Verifier verifier(inputBuffer->GetData(), inputBuffer->GetSize());
    if (!LustBuffers::VerifyRootBuffer(verifier)) {
        Console::CoreError("FlatBuffer file is invalid.\n");
        return;
    }
    // Step 3: Access the root object
    const LustBuffers::Root* root = flatbuffers::GetRoot<LustBuffers::Root>((const void*)inputBuffer->GetData());
    std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>> translations;

    for (const LustBuffers::Context* ctx : *root->contexts()) {
        std::string context_name = ctx->context()->str();
        for (const LustBuffers::Key* key : *ctx->keys()) {
            auto element = key->value()->str();
            translations[context_name][key->key()->str()] = reinterpret_cast<const char8_t*>(element.data());
        }
    }
}

void Lust::Translator::JsonToTranslationMap(const Json::Value& json_context, std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>>* translations)
{
    for (const auto& ctx_json : json_context) {
        std::string context_name = ctx_json["context"].asString();
        auto& key_map = (*translations)[context_name]; // creates context if missing

        for (const auto& key_obj : ctx_json["keys"]) {
            for (const auto& key : key_obj.getMemberNames()) {
                if (key_map.count(key)) {
                    Console::CoreError("Duplicate key in context '{}': {}", context_name, key );
                    continue;
                }
                std::string str = key_obj[key].asString();
                std::u8string cast_str(reinterpret_cast<const char8_t*>(str.data()), str.size());
                key_map[key] = cast_str;
            }
        }
    }
}

void Lust::Translator::LoadJson(const std::string& filepath, Json::Value* json_root)
{
    Json::Reader reader;
    std::string jsonResult;
    FileHandler::ReadTextFile(filepath, &jsonResult);
    reader.parse(jsonResult, *json_root);
}

void Lust::Translator::TranslationMapToFlatbuffers(std::shared_ptr<RawBuffer>* outputBuffer, const std::unordered_map<std::string, std::unordered_map<std::string, std::u8string>>& data)
{
    outputBuffer->reset(new RawBuffer());
    flatbuffers::FlatBufferBuilder builder(1024);
    std::vector<flatbuffers::Offset<LustBuffers::Context>> fb_contexts;

    for (const auto& [context_name, key_map] : data) {
        std::vector<flatbuffers::Offset<LustBuffers::Key>> fb_keys;

        for (const auto& [key, value] : key_map) {
            std::string cast_str(reinterpret_cast<const char*>(value.data()), value.size());
            auto fb_key = LustBuffers::CreateKey(
                builder,
                builder.CreateString(key),
                builder.CreateString(cast_str)
            );
            fb_keys.push_back(fb_key);
        }

        auto fb_context = LustBuffers::CreateContext(
            builder,
            builder.CreateString(context_name),
            builder.CreateVector(fb_keys)
        );

        fb_contexts.push_back(fb_context);
    }

    auto root = CreateRoot(builder, builder.CreateVector(fb_contexts));
    builder.Finish(root);

	(*outputBuffer)->RecreateBuffer(builder.GetBufferPointer(), builder.GetSize());
}