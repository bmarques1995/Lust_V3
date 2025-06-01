#include "FontAtlas.hpp"

#include <Image.hpp>
#include <FileHandler.hpp>

msdfgen::FreetypeHandle* Lust::FontAtlas::m_FTHandle;

Lust::FontAtlas::FontAtlas() :
    m_OutputPath(""), m_FontGeometry(&m_Glyphs)
{
}

Lust::FontAtlas::FontAtlas(const std::string& outputPath) :
    m_OutputPath(outputPath), m_FontGeometry(&m_Glyphs)
{
}

Lust::FontAtlas::~FontAtlas()
{
    ClearFontPack();
}

void Lust::FontAtlas::PushFont(const FontChars& fontInfo)
{
    msdfgen::FontHandle* font = msdfgen::loadFont(m_FTHandle, fontInfo.Path.c_str());
    msdf_atlas::Charset charset;
    for (const auto& range : fontInfo.Ranges)
    {
        for (size_t i = range.Begin; i < range.End; i++)
        {
            charset.add(i);
        }
    }
    m_PackedFonts.push_back(std::make_pair(font, charset));
}

void Lust::FontAtlas::ExportFont()
{
    for (auto& packedFont : m_PackedFonts)
    {
        m_FontGeometry.loadCharset(packedFont.first, 1.0, packedFont.second);
    }
    
    const double maxCornerAngle = .0;
    for (msdf_atlas::GlyphGeometry& glyph : m_Glyphs)
        glyph.edgeColoring(&msdfgen::edgeColoringInkTrap, maxCornerAngle, 0);

    msdf_atlas::TightAtlasPacker packer;
    packer.setMinimumScale(160.0);
    packer.setPixelRange(2.0);
    packer.setMiterLimit(1.0);
    packer.pack(m_Glyphs.data(), m_Glyphs.size());
    int width = 0, height = 0;
    packer.getDimensions(width, height);

    // Round width up to nearest multiple of 64
    int paddedWidth = (width + 63) & ~63;

    msdf_atlas::ImmediateAtlasGenerator<
        float,
        4,
        msdf_atlas::mtsdfGenerator,
        msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 4>
    > generator(paddedWidth, height);
    msdf_atlas::GeneratorAttributes attributes;
    attributes.config.overlapSupport = true;
    attributes.config.errorCorrection.mode = attributes.config.errorCorrection.EDGE_PRIORITY;
    attributes.config.errorCorrection.distanceCheckMode = attributes.config.errorCorrection.ALWAYS_CHECK_DISTANCE;
    attributes.scanlinePass = true;
    generator.setAttributes(attributes);
    generator.setThreadCount(8);
    generator.generate(m_Glyphs.data(), m_Glyphs.size());
    // Retrieve the bitmap buffer
    const msdf_atlas::BitmapAtlasStorage<msdf_atlas::byte, 4>& atlas = generator.atlasStorage();
    const msdfgen::BitmapConstRef<uint8_t, 4> bitmap = atlas;

    msdf_atlas::ArteryFontExportProperties arfontProps;
    arfontProps.fontSize = 160.0f;
    arfontProps.pxRange = 2.0f;
    arfontProps.imageType = msdf_atlas::ImageType::MTSDF;
    arfontProps.imageFormat = msdf_atlas::ImageFormat::PNG;
    arfontProps.yDirection = msdf_atlas::YDirection::TOP_DOWN;
    msdf_atlas::exportArteryFont<float>(&m_FontGeometry, 1, bitmap, m_OutputPath.c_str(), arfontProps);
}

void Lust::FontAtlas::SetOutputPath(std::string_view outputPath)
{
    m_OutputPath = outputPath;
}

void Lust::FontAtlas::ClearFontPack()
{
    for (auto& packedFont : m_PackedFonts)
    {
        msdfgen::destroyFont(packedFont.first);
        packedFont.first = nullptr;
    }
    m_PackedFonts.clear();
}

void Lust::FontAtlas::LoadMSDF()
{
    m_FTHandle = msdfgen::initializeFreetype();
}

void Lust::FontAtlas::UnloadMSDF()
{
    msdfgen::deinitializeFreetype(m_FTHandle);
}
