#pragma once
#include <QTranslator>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

class TranslationCompiler
{
	struct TranslationScan {
		fs::path directory;           // Shared directory
		std::vector<std::string> names; // Filenames without extensions
	};
public:
	TranslationCompiler() = default;
	~TranslationCompiler() = default;
	/**
	 * @brief Compiles the translation files into a single file
	 * @param inputFiles List of input translation files
	 * @param outputFile Output file for the compiled translations
	 * @return true if compilation was successful, false otherwise
	 */
	bool Compile(const std::string& inputDir, const std::string& lreleaseDir);
private:

	TranslationScan FindTranslationSources(const fs::path& folder, const std::string& extension);/* {
		TranslationScan result;
		result.directory = folder;

		for (const auto& entry : fs::directory_iterator(folder)) {
			if (entry.is_regular_file() && entry.path().extension() == extension) {
				result.names.push_back(entry.path().stem().string());
			}
		}

		return result;
	}*/
};
