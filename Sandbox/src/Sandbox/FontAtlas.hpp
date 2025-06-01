#pragma once

#include <msdf-atlas-gen/msdf-atlas-gen.h>
#include <vector>
#include <string>
#include <utility>

namespace Lust
{
	struct CharsetRange
	{
		uint32_t Begin, End;
	};
	struct FontChars
	{
		std::string Path;
		std::vector<CharsetRange> Ranges;
	};

	class FontAtlas
	{
	public:
		FontAtlas();
		FontAtlas(const std::string& outputPath);

		~FontAtlas();

		void PushFont(const FontChars& fontInfo);
		void ExportFont();
		void SetOutputPath(std::string_view outputPath);
		void ClearFontPack();

		static void LoadMSDF();
		static void UnloadMSDF();


	private:
		static msdfgen::FreetypeHandle* m_FTHandle;
		std::vector<std::pair<msdfgen::FontHandle*, msdf_atlas::Charset>> m_PackedFonts;
		std::string m_OutputPath;
		std::vector<msdf_atlas::GlyphGeometry> m_Glyphs;
		msdf_atlas::FontGeometry m_FontGeometry;
	};
}