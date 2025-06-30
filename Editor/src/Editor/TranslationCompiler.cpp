#include "TranslationCompiler.hpp"
#include <QProcess>

bool TranslationCompiler::Compile(const std::string& inputDir, const std::string& lreleaseDir)
{
	fs::path lreleasePath = fs::absolute(lreleaseDir) / "lrelease";
	auto files = FindTranslationSources(fs::path(inputDir), ".xml");
	for (auto& file : files.names)
	{
		QProcess p;
		std::string raw_translation = (files.directory / file).string();
		std::string input_file = raw_translation + ".xml";
		std::string output_file = raw_translation + ".qm";
		p.start(lreleasePath.string().c_str(), {input_file.c_str(), "-qm", output_file.c_str()});
		p.waitForFinished();
	}
	return true;
}

TranslationCompiler::TranslationScan TranslationCompiler::FindTranslationSources(const fs::path& folder, const std::string& extension)
{
	TranslationScan result;
	result.directory = fs::absolute(folder);

	for (const auto& entry : fs::directory_iterator(folder)) {
		if (entry.is_regular_file() && entry.path().extension() == extension) {
			result.names.push_back(entry.path().stem().string());
		}
	}

	return result;
}