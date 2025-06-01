#include "Font.hpp"
#include <artery-font/stdio-serialization.hpp>
#include "FileHandler.hpp"
#include "Image.hpp"

Lust::Font::Font(const std::filesystem::path& arteryFont)
{
	artery_font::StdArteryFont<float> arfont = { };
	artery_font::readFile(arfont, arteryFont.string().c_str());

	m_Texture.reset(Image::CreateImage((std::byte*)&(arfont.images[0].data[0]), arfont.images[0].data.length(), ImageFormat::PNG));
	LoadGlyphs(&arfont);

}

const std::shared_ptr<Lust::Image> Lust::Font::GetTexture()
{
	return m_Texture;
}

artery_font::Glyph<float> Lust::Font::GetGlyph(uint32_t code)
{
	return artery_font::Glyph<float>();
}

void Lust::Font::LoadGlyphs(const artery_font::StdArteryFont<float>* font)
{
	size_t numberOfGlyphs = (size_t)font->variants[0].glyphs.length();
	m_Glyphs.reserve(numberOfGlyphs);
	m_Glyphs.resize(numberOfGlyphs);
	memcpy(&m_Glyphs[0], &(font->variants[0].glyphs[0]), numberOfGlyphs * sizeof(artery_font::Glyph<float>));
}
