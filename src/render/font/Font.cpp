#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
//#define STBTT_malloc(x,u)    tmpalloc(x,u)
//#define STBTT_free(x,u)      tmpfree(x,u)
#include "Font.h"

#include <utility>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <thread>

#include "FontRenderer.h"
#include "util/IOUtils.h"
#include "util/resources/Image.h"
#include "util/RectPacker.h"
#include "util/text/unicode_utils.h"
#include "util/math/math.h"

#include <glm/gtx/io.hpp> // TODO: remove
#include <chrono> // TODO: remove

namespace eng {

	glm::vec2 Glyph::getMinUV() const {
		return {
			static_cast<float>(uvMin.x) / static_cast<float>(font->font->getTexture().getWidth()),
			static_cast<float>(uvMin.y) / static_cast<float>(font->font->getTexture().getHeight()),
		};
	}
	glm::vec2 Glyph::getMaxUV() const {
		return {
			static_cast<float>(uvMax.x) / static_cast<float>(font->font->getTexture().getWidth()),
			static_cast<float>(uvMax.y) / static_cast<float>(font->font->getTexture().getHeight()),
		};
	}

	Font::Font(const FontData& fontData, const std::vector<FontData>& fallbacks) :
			fontData(fontData), fallbacks(fallbacks) {
		initFont();
	}
	Font::Font(FontData&& fontData, std::vector<FontData>&& fallbacks) :
			fontData(std::move(fontData)), fallbacks(std::move(fallbacks)) {
		initFont();
	}

	void Font::initFont() {
		this->fontData.font = this;
		for (auto& fd : this->fallbacks) {
			fd.font = this;
			if (fd.lineHeight > this->fontData.lineHeight) {
				this->fontData.lineGap += (fd.lineHeight - this->fontData.lineHeight);
				this->fontData.lineHeight = (this->fontData.ascent - this->fontData.descent) + this->fontData.lineGap;
			}
		}
		// TODO: use a mix of the maximum line height and the base font's line height,
		//		 or weight each font's contribution to the total line height by the number of glyphs used from them

		const auto glyphInitStart = std::chrono::steady_clock::now();
		std::cout << "beginning glyph initialization\n";
		initGlyphs();
		auto glyphInitDur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - glyphInitStart);
		std::cout << "glyph initialization complete (" << glyphInitDur.count() << "ms)\n";

		std::cout << "line height: " << this->fontData.lineHeight << '\n';
	}

	FontData::FontData(std::vector<unsigned char>&& fileData, int offset) : data(std::move(fileData)) {
		if (!stbtt_InitFont(&fontInfo, data.data(), offset)) // initialize the stb_truetype font info object
			throw std::runtime_error("Failed to initialize font");

		fontScale = stbtt_ScaleForPixelHeight(&fontInfo, FontRenderer::sdf_size);

		int asc, desc, lg;
		stbtt_GetFontVMetrics(&fontInfo, &asc, &desc, &lg);
		ascent = static_cast<float>(asc) * fontScale;
		descent = static_cast<float>(desc) * fontScale;
		lineGap = static_cast<float>(lg) * fontScale;
		lineHeight = (ascent - descent) + lineGap;
	}

	Font Font::loadFont(const std::string& filePath, std::initializer_list<std::string> fallbackPaths) {
		return loadFont(filePath, { fallbackPaths.begin(), fallbackPaths.end() });
	}
	Font Font::loadFont(const std::string& filePath, const std::span<const std::string> fallbackPaths) {
		std::vector<FontData> fallbackFonts;
		for (const std::string& fPath : fallbackPaths) {
			if (!fPath.empty()) {
				try {
					fallbackFonts.emplace_back(readBinaryFile("res/fonts/" + fPath));
				} catch (const std::runtime_error& e) {
					std::cerr << "Error loading font data from \"" << fPath << "\":\n\t" << e.what() << '\n';
				} catch (...) {
					std::cerr << "Error loading font data from \"" << fPath << "\"\n";
				}
			}
		}
		if (filePath.empty())
			throw std::runtime_error("Empty font file path");
		return Font({ readBinaryFile("res/fonts/" + filePath) }, std::move(fallbackFonts));
	}

	const Texture& Font::getTexture() const noexcept { return texture; }

	const Glyph& Font::getGlyph(const char32_t codepoint) const {
		const auto it = glyphs.find(codepoint);
		if (it != glyphs.end()) return it->second;
		return glyphs.at(unicode::replacement_character);
	}

	float Font::getKernAdvance(const Glyph& glyph1, const Glyph& glyph2) const {
		int kernAdvance = stbtt_GetGlyphKernAdvance(&glyph1.font->fontInfo, glyph1.index, glyph2.index);
		if (kernAdvance) return static_cast<float>(kernAdvance) * glyph1.font->fontScale;
		kernAdvance = stbtt_GetGlyphKernAdvance(&glyph2.font->fontInfo, glyph1.index, glyph2.index);
		return static_cast<float>(kernAdvance) * glyph2.font->fontScale;
	}

	// creates a vector of codepoints in the range [begin, end] (inclusive)
	static std::vector<char32_t> createCodepointRange(char32_t first, char32_t last) {
		if (first > last) std::swap(first, last);
		std::vector<char32_t> codepoints((last - first) + 1);
		std::iota(codepoints.begin(), codepoints.end(), first);
		return codepoints;
	}
	// creates a vector of codepoints in a list of ranges  [begin, end] (inclusive)
	static std::vector<char32_t> createCodepointRanges(std::initializer_list<std::pair<char32_t, char32_t>> ranges) {
		std::vector<char32_t> codepoints;
		for (const auto& [first, last] : ranges) {
			for (char32_t codepoint = first; codepoint <= last; codepoint++) {
				codepoints.push_back(codepoint);
			}
		}
		return codepoints;
	}

	void Font::initGlyphs() {
		// TODO: get from font config
		auto codepoints = createCodepointRanges({
			// replacement character
			{ unicode::replacement_character, unicode::replacement_character },
			// ascii
			{ 0x0000, 0x001F }, // control characters
			{ 0x0020, 0x007F }, // latin basic
			{ 0x0080, 0x00FF }, // latin 1
			// latin extended
			{ 0x0100, 0x017F }, // latin a
			{ 0x0180, 0x024F }, // latin b
			{ 0x1E00, 0x1EFF }, // latin additional
			{ 0x2C60, 0x2C7F }, // latin c
			{ 0xA720, 0xA7FF }, // latin d
			{ 0xAB30, 0xAB6F }, // latin e
			// symbols, etc.
			{ 0x2000, 0x206F }, // general punctuation
			//{ 0x20A0, 0x20CF }, // currency symbols
			{ 0x2100, 0x214F }, // letterlike symbols
			{ 0x2160, 0x217F }, // roman numerals
			//{ 0x2190, 0x21FF }, // arrows
			//{ 0x2200, 0x22FF }, // mathematical operators
			{ 0x2300, 0x23FF }, // misc. technical
			//{ 0x2500, 0x257F }, // box drawing
			//{ 0x2580, 0x259F }, // block elements
			//{ 0x25A0, 0x25FF }, // geometric shapes
			{ 0x2600, 0x26FF }, // misc. symbols
			{ 0x1F300, 0x1F5FF }, // misc. symbols & pictographs
			{ 0x1F600, 0x1F64F }, // emoticons
			{ 0x1F900, 0x1F9FF }, // supplemental symbols and pictographs
			//{ 0x1D100, 0x1D1FF }, // musical
			{ 0x1F700, 0x1F77F }, // alchemical
			// ancient languages
			{ 0x0370, 0x03FF }, // greek and Coptic)
			{ 0x16A0, 0x16FF }, // runic
			//{ 0x10330, 0x1034F }, // gothic
			//{ 0x10900, 0x1091F }, // phoenician
			{ 0x13010, 0x13010 }, // egyptian hieroglyph A014
			{ 0x130B8, 0x130B8 }, // egyptian hieroglyph D052
		});

		std::vector<FontData*> fontDataList { &this->fontData };
		for (auto& f : fallbacks) fontDataList.push_back(&f);

		struct PreGlyphSDF {
			glm::i16vec2 sdfSize, glyphOffset;
			Image1 sdfData;
			PreGlyphSDF(const glm::i16vec2& sdfSize, const glm::i16vec2& glyphOffset, Image1&& sdfData) :
					sdfSize(sdfSize), glyphOffset(glyphOffset), sdfData(std::move(sdfData)) {}
		};
		struct PreGlyph {
			FontData* const font;
			const char32_t codepoint;
			const int32_t index;
			std::optional<PreGlyphSDF> sdf;
			PreGlyph(FontData* const font, const char32_t codepoint, const int32_t index) noexcept :
					font(font), codepoint(codepoint), index(index), sdf(std::nullopt) {}
		};
		std::unordered_map<char32_t, PreGlyph> preGlyphs;
		// first char32 is a codepoint that corresponds to a glyph which is already present in preGlyphs
		// second char32 is the codepoint key of the glyph in preGlyphs
		std::unordered_map<char32_t, char32_t> copyGlyphs;

		size_t missingGlyphs = 0;
		for (auto codepoint : codepoints) {
			// skip glyphs that have already been created
			if (preGlyphs.find(codepoint) != preGlyphs.end()) continue;
			if (copyGlyphs.find(codepoint) != copyGlyphs.end()) continue;

			bool preGlyphCreated = false;
			for (auto fd : fontDataList) {
				if (const int32_t glyphIndex = stbtt_FindGlyphIndex(&fd->fontInfo, static_cast<int>(codepoint)); glyphIndex != 0) {
					const auto existingPreGlyphIt = std::find_if(preGlyphs.begin(), preGlyphs.end(), [&](const auto& entry) {
						const auto& [key, val] = entry;
						return (glyphIndex == val.index) && (fd == val.font);
					});
					if (existingPreGlyphIt != preGlyphs.end()) {
						const auto& [key, existing] = *existingPreGlyphIt;
						if (existing.codepoint != unicode::replacement_character) {
							copyGlyphs.try_emplace(codepoint, key);
							preGlyphCreated = true;
							break;
						}
					} else {
						preGlyphs.try_emplace(codepoint, fd, codepoint, glyphIndex);
						preGlyphCreated = true;
						break;
					}
				}
			}
			if (!preGlyphCreated) missingGlyphs++;
		}

		const float fontScale = fontData.fontScale;

		// generate the sdfs in parallel
		{
			const auto threadCount = vmin(vmax(static_cast<int>(std::thread::hardware_concurrency()) - 1, 1), 15);
			const auto glyphsPerThread = static_cast<int>(std::ceil(static_cast<float>(preGlyphs.size()) / static_cast<float>(threadCount)));

			std::vector<std::thread> sdfGenThreads {};
			const auto sdfGenFunction = [&](auto& entry) {
				auto& [codepoint, preGlyph] = entry;
				int w = 0, h = 0, xoff = 0, yoff = 0;
				auto sdfData = stbtt_GetGlyphSDF(
					&preGlyph.font->fontInfo, fontScale, preGlyph.index,
					FontRenderer::sdf_padding,
					FontRenderer::sdf_on_edge_value,
					FontRenderer::sdf_pixel_dist_scale,
					&w, &h, &xoff, &yoff
				);
				if (sdfData) {
					preGlyph.sdf.emplace(glm::i16vec2 { w, h }, glm::i16vec2 { xoff, yoff },
										 Image1(sdfData, static_cast<size_t>(w), static_cast<size_t>(h)));
					stbtt_FreeSDF(sdfData, &preGlyph.font->fontInfo.userdata);
				}
			};
			std::vector<decltype(preGlyphs)::value_type*> preGlyphsEntries {};
			preGlyphsEntries.reserve(preGlyphs.size());
			for (auto& preGlyphEntry : preGlyphs)
				preGlyphsEntries.push_back(&preGlyphEntry);
			const auto glyphGenThreadFunc = [&](const int startIndex, const int endIndex) {
				for (auto pgi = startIndex; pgi < endIndex; pgi++)
					sdfGenFunction(*preGlyphsEntries[pgi]);
			};
			size_t preGlyphStartIndex = 0;
			for (auto ti = 0; ti < threadCount; ti++) {
				auto preGlyphEndIndex = vmin(preGlyphStartIndex + glyphsPerThread, preGlyphs.size());
				if (preGlyphEndIndex <= preGlyphStartIndex) break;
				sdfGenThreads.emplace_back(glyphGenThreadFunc, preGlyphStartIndex, preGlyphEndIndex);
				preGlyphStartIndex = preGlyphEndIndex;
			}
			for (auto& thread : sdfGenThreads)
				thread.join();
		}

		RectPacker<int16_t> packer { glm::i16vec2{ initialAtlasSize, initialAtlasSize } };
		std::vector<std::pair<glm::i16vec2, Image1>> glyphSdfs;

		for (auto& [codepoint, preGlyph] : preGlyphs) {
			// get the xAdvance and leftSideBearing
			int xadv, lsb;
			stbtt_GetGlyphHMetrics(&preGlyph.font->fontInfo, preGlyph.index, &xadv, &lsb);
			float xAdvance = xadv * fontScale;
			float leftSideBearing = lsb * fontScale;

			if (preGlyph.sdf) {
				auto& preGlyphSdf = *preGlyph.sdf;

				// pack the glyph sdf into the atlas
				const auto atlasPos = packer.packRect(preGlyphSdf.sdfSize + int16_t { Glyph::padding * 2 });
				glm::i16vec2 uvMin = atlasPos + int16_t { Glyph::padding };
				glm::i16vec2 uvMax = uvMin + preGlyphSdf.sdfSize;

				// create the glyph object and map it to it's corresponding codepoint
				glyphs.try_emplace(codepoint,
								   preGlyph.font, preGlyph.codepoint, preGlyph.index,
								   preGlyphSdf.sdfSize, preGlyphSdf.glyphOffset,
								   xAdvance, leftSideBearing,
								   uvMin, uvMax);
				// keep the texture data around until after all glyphs have been packed
				glyphSdfs.emplace_back(uvMin, std::move(preGlyphSdf.sdfData));
			} else {
				// create the glyph object and map it to it's corresponding codepoint
				glyphs.try_emplace(codepoint,
								   preGlyph.font, preGlyph.codepoint, preGlyph.index,
								   glm::i16vec2(0, 0), glm::i16vec2(0, 0),
								   xAdvance, leftSideBearing,
								   glm::i16vec2(0, 0), glm::i16vec2(0, 0));
			}
		}
		for (auto [codepoint, existingCodePoint] : copyGlyphs) {
			const auto& existingGlyph = glyphs.at(existingCodePoint);
			// create duplicate glyph objects
			glyphs.try_emplace(codepoint,
							   existingGlyph.font, existingGlyph.codepoint, existingGlyph.index,
							   existingGlyph.sdfSize, existingGlyph.offset,
							   existingGlyph.xAdvance, existingGlyph.leftSideBearing,
							   existingGlyph.uvMin, existingGlyph.uvMax);
		}


		if (glyphs.size() == 0) {
			std::cerr << "failed to create font glyphs" << std::endl;
			throw std::runtime_error("Error initializing font");
		}

		auto atlasSize = packer.getMinPackedBinSize();
		if (atlasSize.x % 2 == 1) atlasSize.x += 1; // round atlas width up to next even number
		if (atlasSize.y % 2 == 1) atlasSize.y += 1; // round atlas height up to next even number

		std::cout << "font glyphs created: " << glyphs.size() << '\n';
		std::cout << "missing glyphs: " << missingGlyphs << '\n';
		std::cout << "               (" << (glyphs.size()/* - missingGlyphs*/) << "/" << codepoints.size() << ')' << '\n';

		const auto packerBinSize = packer.getBinSize();
		std::cout << "glyph packer bin size: " << packerBinSize.x << " x " << packerBinSize.y << '\n';

		if ((atlasSize.x == 0) || (atlasSize.y == 0)) {
			std::cerr << "failed to create font atlas" << std::endl;
			throw std::runtime_error("Error creating font glyph atlas");
		}

		// create the glyph atlas texture data
		Image1 atlasData(static_cast<size_t>(atlasSize.x), static_cast<size_t>(atlasSize.y));
		for (const auto& [atlasPos, sdfTexData] : glyphSdfs)
			atlasData.setSubData(sdfTexData, { int16_t{atlasPos.x}, int16_t{atlasPos.y} });

		constexpr auto bytesPerKB = 1024;
		constexpr auto bytesPerMB = bytesPerKB * 1024;
		const auto atlasDataSize = atlasData.getSizeBytes();
		const bool displayAtlasSizeMB = atlasDataSize >= bytesPerMB;
		const auto atlasSizeDisplayVal = atlasDataSize / static_cast<float>(displayAtlasSizeMB ? bytesPerMB : bytesPerKB);
		std::cout << "glyph atlas size: " << atlasSize.x << " x " << atlasSize.y
				<< "\t(" << atlasSizeDisplayVal << (displayAtlasSizeMB ? " MB)" : " KB)") << '\n';

		// write the glyph atlas to a png
		atlasData.transform([](unsigned char v) { return glm::u8vec4 { 0, 0, 0, v }; }).writeImage("glyph_atlas.png");

		// update the font texture
		texture.setScaleMode(TextureScaleMode::LINEAR).setWrapMode(TextureWrapMode::CLAMP_EDGE);
		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
		texture.setData(std::move(atlasData));
		//glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		//texture.genMipmaps();
	}

}