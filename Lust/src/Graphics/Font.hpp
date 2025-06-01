#pragma once

#include "LustDLLMacro.hpp"
#include <artery-font/structures.h>
#include <artery-font/std-artery-font.h>
#include <filesystem>
#include "Texture.hpp"
#include "Image.hpp"

namespace Lust
{
	class LUST_API Font
	{
	public:
		Font(const std::filesystem::path& arteryFont);

		const std::shared_ptr<Image> GetTexture();
		artery_font::Glyph<float> GetGlyph(uint32_t code);
	private:

		void LoadGlyphs(const artery_font::StdArteryFont<float>* font);

		std::vector<artery_font::Glyph<float>> m_Glyphs;
		std::shared_ptr<Image> m_Texture;
	};
}
