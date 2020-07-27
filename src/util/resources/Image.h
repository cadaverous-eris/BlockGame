#pragma once

#include <string>
#include <memory>
#include <utility>
#include <algorithm>
#include <variant>
#include <cassert>
#include <stdexcept>
#include <cstring>
#include <iostream>

#include <stb_image.h>
#include <stb_image_write.h>

#include "util/type_traits.h"
#include "util/math/math.h"
#include "util/Color.h"

namespace eng {

	namespace image_traits {

		// get type used to represent a pixel of an Image based on it's data type and number of channels
		template<size_t channels, typename D, typename = void> struct image_pixel_type {};
		template<typename D> struct image_pixel_type<4, D, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = glm::vec<4, D>; };
		template<typename D> struct image_pixel_type<3, D, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = glm::vec<3, D>; };
		template<typename D> struct image_pixel_type<2, D, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = glm::vec<2, D>; };
		template<typename D> struct image_pixel_type<1, D, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = D; };
		template<size_t channels, typename D> using image_pixel_type_t = typename image_pixel_type<channels, D>::type;
		
		template<typename PixelT, typename = void> struct pixel_type_channels : std::integral_constant<size_t, 0> {};
		template<typename D> struct pixel_type_channels<glm::vec<4, D>, std::enable_if_t<std::is_arithmetic_v<D>>> : std::integral_constant<size_t, 4> {};
		template<typename D> struct pixel_type_channels<glm::vec<3, D>, std::enable_if_t<std::is_arithmetic_v<D>>> : std::integral_constant<size_t, 3> {};
		template<typename D> struct pixel_type_channels<glm::vec<2, D>, std::enable_if_t<std::is_arithmetic_v<D>>> : std::integral_constant<size_t, 2> {};
		template<typename D> struct pixel_type_channels<D, std::enable_if_t<std::is_arithmetic_v<D>>> : std::integral_constant<size_t, 1> {};
		template<typename PixelT> constexpr size_t pixel_type_channels_v = pixel_type_channels<PixelT>::value;

		template<typename PixelT, typename = void> struct pixel_type_data_type {};
		template<typename D> struct pixel_type_data_type<glm::vec<4, D>, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = D; };
		template<typename D> struct pixel_type_data_type<glm::vec<3, D>, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = D; };
		template<typename D> struct pixel_type_data_type<glm::vec<2, D>, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = D; };
		template<typename D> struct pixel_type_data_type<D, std::enable_if_t<std::is_arithmetic_v<D>>> { using type = D; };
		template<typename PixelT> using pixel_type_data_type_t = typename pixel_type_data_type<PixelT>::type;

		template<size_t channelsIn, typename DataTypeIn, typename F, typename = void>
		struct transformer_channels_out : std::integral_constant<size_t, 0> {};
		template<size_t channelsIn, typename DataTypeIn, typename F>
		struct transformer_channels_out<channelsIn, DataTypeIn, F,
				std::enable_if_t<(pixel_type_channels_v<std::invoke_result_t<F, image_pixel_type_t<channelsIn, DataTypeIn>>> > 0)>
		> : std::integral_constant<size_t, pixel_type_channels_v<std::invoke_result_t<F, image_pixel_type_t<channelsIn, DataTypeIn>>>> {};
		template<size_t channels, typename D, typename F>
		constexpr size_t transformer_channels_out_v = transformer_channels_out<channels, D, F>::value;

		template<size_t channelsIn, typename DataTypeIn, typename F, typename = void>
		struct transformer_data_type_out {};
		template<size_t channelsIn, typename DataTypeIn, typename F>
		struct transformer_data_type_out<channelsIn, DataTypeIn, F,
			std::void_t<pixel_type_data_type_t<std::invoke_result_t<F, image_pixel_type_t<channelsIn, DataTypeIn>>>>
		> { using type = pixel_type_data_type_t<std::invoke_result_t<F, image_pixel_type_t<channelsIn, DataTypeIn>>>; };
		template<size_t channels, typename D, typename F>
		using transformer_data_type_out_t = typename transformer_data_type_out<channels, D, F>::type;

	}

	template<size_t channels, typename D, typename = void>
	struct Image {};

	using Image4 = Image<4, unsigned char>;
	using Image3 = Image<3, unsigned char>;
	using Image2 = Image<2, unsigned char>;
	using Image1 = Image<1, unsigned char>;
	using ImageRGBA = Image4;
	using ImageRGB = Image3;

	using ImageVariant = std::variant<Image4, Image3, Image2, Image1>;

	template<size_t C, typename D>
	struct Image<C, D, std::enable_if_t<(C > 0) && (C <= 4) && std::is_arithmetic_v<D>>> {
		static constexpr size_t channels = C;
		using data_type = D;
		using data_ptr = std::unique_ptr<data_type[]>;
		using pixel_type = image_traits::image_pixel_type_t<channels, data_type>;
		using coord_type = glm::vec<2, size_t>;
	private:
		data_ptr data;
		size_t width, height;

		static data_ptr makeDataPtr(const size_t width, const size_t height) {
			const auto size = width * height * channels;
			return (size > 0) ? std::make_unique<data_type[]>(size) : nullptr;
		}
	public:
		Image() : Image(nullptr, 0, 0) {}
		// transfers ownership of texture data from existing unique_ptr
		Image(data_ptr data, size_t width, size_t height) :
				data(std::move(data)), width(width), height(height) {
			if (!this->data && ((width * height) > 0))
				this->data = std::make_unique<data_type[]>(width * height * channels);
			//assert(this->data.get() != nullptr);
		}
		// copies texture data from raw pointer
		Image(const data_type* data, size_t width, size_t height) :
				Image(makeDataPtr(width, height), width, height) {
			if (data && this->data) std::memcpy(this->data.get(), data, getSize() * sizeof(data_type));
			//else std::fill(this->data.get(), this->data.get() + getSize(), data_type(0));
		}
		Image(size_t width, size_t height) : Image(nullptr, width, height) {}

		Image(const Image& b) : Image(b.data.get(), b.width, b.height) {}
		Image(Image&& b) : Image(std::move(b.data), b.width, b.height) {}
		Image& operator =(const Image& b) {
			if (this != &b) {
				if (b) {
					data = std::make_unique<data_type[]>(b.getSize());
					std::memcpy(data.get(), b.data.get(), b.getSize() * sizeof(data_type)); // copy data
				} else {
					data = nullptr;
				}
				width = b.width;
				height = b.height;
			}
			return *this;
		}
		Image& operator =(Image&& b) {
			if (this != &b) {
				data = std::move(b.data);
				width = b.width;
				height = b.height;
			}
			return *this;
		}

		const data_type& operator [](const size_t i) const {
			if (i >= getSize()) throw std::out_of_range("invalid Image subscript");
			return data[i];
		}
		data_type& operator [](const size_t i) {
			if (i >= getSize()) throw std::out_of_range("invalid Image subscript");
			return data[i];
		}

		inline const data_type* get() const noexcept { return data.get(); }
		inline data_type* get() noexcept { return data.get(); }

		inline size_t getWidth() const noexcept { return width; }
		inline size_t getHeight() const noexcept { return height; }
		static inline constexpr size_t numChannels() noexcept { return channels; }

		// returns the number of pixels multiplied by the number of channels
		size_t getSize() const noexcept { return width * height * channels; }
		// returns the number of pixels
		size_t getArea() const noexcept { return width * height; }
		// returns the size of the image in bytes
		size_t getSizeBytes() const noexcept { return width * height * channels * sizeof(data_type); }

		operator bool() const noexcept { return data && ((width * height) > 0); }

		[[nodiscard]] Image getSubData(const coord_type& offset, const size_t subWidth, const size_t subHeight) const {
			const size_t subDataSize = subWidth * subHeight * channels;
			data_ptr subData = std::make_unique<data_type[]>(subDataSize);
			if (data) {
				const size_t subDataMaxY = vmin(height, offset.y + subHeight);
				const size_t subDataMaxX = vmin(width, offset.x + subWidth);
				const size_t copyHeight = subDataMaxY - offset.y, copyWidth = subDataMaxX - offset.x;
				for (size_t y = 0; y < copyHeight; y++)
					std::memcpy(&subData[y * subWidth * channels], &data[(((y + offset.y) * width) + offset.x) * channels], copyWidth * channels * sizeof(data_type));
			}
			return Image(std::move(subData), subWidth, subHeight);
		}
		void setSubData(const Image& subData, const coord_type& offset) {
			setSubData(subData.data, offset, subData.width, subData.height);
		}
		void setSubData(const data_ptr& subData, const coord_type& offset, const size_t subWidth, const size_t subHeight) {
			setSubData(subData.get(), offset, subWidth, subHeight);
		}
		void setSubData(const data_type* subData, const coord_type& offset, const size_t subWidth, const size_t subHeight) {
			//assert(data != nullptr);
			const size_t subDataMaxY = vmin(height, offset.y + subHeight);
			const size_t subDataMaxX = vmin(width, offset.x + subWidth);
			const size_t copyHeight = subDataMaxY - offset.y, copyWidth = subDataMaxX - offset.x;
			if (data && (getArea() > 0)) {
				const auto copyBytes = copyWidth * channels * sizeof(data_type);
				for (size_t y = 0; y < copyHeight; y++) {
					const auto dataRowStart = (((y + offset.y) * width) + offset.x) * channels;
					const auto subDataRowStart = y * subWidth * channels;
					std::memcpy(&data[dataRowStart], &subData[subDataRowStart], copyBytes);
				}
			}
		}

		// TODO: pixel iterators

		// TODO: rotation function(s)
		// TODO: bicubic scaling functions?
		// TODO: box sampling scaling functions?

		[[nodiscard]] Image flipX() const {
			Image result(width, height);
			for (size_t y = 0; y < height; y++) {
				for (size_t x = 0; x < width; x++) {
					const size_t fx = (width - 1) - x;
					for (size_t c = 0; c < channels; c++)
						result.data[(((y * width) + x) * channels) + c] = data[(((y * width) + fx) * channels) + c];
				}
			}
			return result;
		}
		[[nodiscard]] Image flipY() const {
			Image result(width, height);
			if (data) {
				for (size_t y = 0; y < height; y++) {
					const size_t fy = (height - 1) - y;
					std::memcpy(&result.data[y * width * channels], &data[fy * width * channels], width * channels * sizeof(data_type));
				}
			}
			return result;
		}
		[[nodiscard]] Image flipXY() const {
			Image result(width, height);
			for (size_t y = 0; y < height; y++) {
				const size_t fy = (height - 1) - y;
				for (size_t x = 0; x < width; x++) {
					const size_t fx = (width - 1) - x;
					for (size_t c = 0; c < channels; c++)
						result.data[(((y * width) + x) * channels) + c] = data[(((fy * width) + fx) * channels) + c];
				}
			}
			return result;
		}

		template<typename T, typename U, std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>, int> = 0>
		[[nodiscard]] Image scaleNearest(const T xScale, const U yScale) const {
			const bool doFlipX = xScale < 0, doFlipY = yScale < 0;
			if constexpr (std::is_signed_v<T>) xScale = glm::abs(xScale);
			if constexpr (std::is_signed_v<U>) yScale = glm::abs(yScale);
			const size_t scaledWidth = static_cast<size_t>(xScale * width);
			const size_t scaledHeight = static_cast<size_t>(yScale * height);
			if (doFlipX && doFlipY) {
				return (scaledWidth * scaledHeight < width * height)
					? resizeNearest(scaledWidth, scaledHeight).flipXY()
					: flipXY().resizeNearest(scaledWidth, scaledHeight);
			}
			if (doFlipX) {
				return (scaledWidth * scaledHeight < width * height)
					? resizeNearest(scaledWidth, scaledHeight).flipX()
					: flipX().resizeNearest(scaledWidth, scaledHeight);
			}
			if (doFlipY) {
				return (scaledWidth * scaledHeight < width * height)
					? resizeNearest(scaledWidth, scaledHeight).flipY()
					: flipY().resizeNearest(scaledWidth, scaledHeight);
			}
			return resizeNearest(scaledWidth, scaledHeight);
		}
		template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		[[nodiscard]] Image scaleNearest(T scale) const {
			const bool flip = scale < 0;
			if constexpr (std::is_signed_v<T>) scale = glm::abs(scale);
			const size_t scaledWidth = static_cast<size_t>(scale * width);
			const size_t scaledHeight = static_cast<size_t>(scale * height);
			if (flip) {
				return (scale < 1)
					? resizeNearest(scaledWidth, scaledHeight).flipXY()
					: flipXY().resizeNearest(scaledWidth, scaledHeight);
			}
			return resizeNearest(scaledWidth, scaledHeight);
		}

		[[nodiscard]] Image resizeNearest(size_t scaledWidth, size_t scaledHeight) const {
			if ((scaledWidth == 0) && (width != 0)) scaledWidth = 1;
			if ((scaledHeight == 0) && (height != 0)) scaledHeight = 1;
			if ((getArea() == 0) || ((scaledWidth * scaledHeight) == 0))
				return Image(scaledWidth, scaledHeight);
			const double scaleX = static_cast<double>(width) / static_cast<double>(scaledWidth);
			const double scaleY = static_cast<double>(height) / static_cast<double>(scaledHeight);
			Image result(scaledWidth, scaledHeight);
			for (size_t y = 0; y < scaledHeight; y++) {
				const size_t srcY = static_cast<size_t>(y * scaleY);
				for (size_t x = 0; x < scaledWidth; x++) {
					const size_t srcX = static_cast<size_t>(x * scaleX);
					for (size_t c = 0; c < channels; c++)
						result.data[(((y * scaledWidth) + x) * channels) + c] = data[(((srcY * width) + srcX) * channels) + c];
				}
			}
			return result;
		}

		template<typename T, typename U, std::enable_if_t<std::is_arithmetic_v<T> && std::is_arithmetic_v<U>, int> = 0>
		[[nodiscard]] Image scaleLinear(const T xScale, const U yScale) const {
			const bool doFlipX = xScale < 0, doFlipY = yScale < 0;
			if constexpr (std::is_signed_v<T>) xScale = glm::abs(xScale);
			if constexpr (std::is_signed_v<U>) yScale = glm::abs(yScale);
			const size_t scaledWidth = static_cast<size_t>(xScale * width);
			const size_t scaledHeight = static_cast<size_t>(yScale * height);
			if (doFlipX && doFlipY) {
				return (scaledWidth * scaledHeight < width * height)
					? resizeLinear(scaledWidth, scaledHeight).flipXY()
					: flipXY().resizeLinear(scaledWidth, scaledHeight);
			}
			if (doFlipX) {
				return (scaledWidth * scaledHeight < width * height)
					? resizeLinear(scaledWidth, scaledHeight).flipX()
					: flipX().resizeLinear(scaledWidth, scaledHeight);
			}
			if (doFlipY) {
				return (scaledWidth * scaledHeight < width * height)
					? resizeLinear(scaledWidth, scaledHeight).flipY()
					: flipY().resizeLinear(scaledWidth, scaledHeight);
			}
			return resizeLinear(scaledWidth, scaledHeight);
		}
		template<typename T, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
		[[nodiscard]] Image scaleLinear(T scale) const {
			const bool doFlip = scale < 0;
			if constexpr (std::is_signed_v<T>) scale = glm::abs(scale);
			const size_t scaledWidth = static_cast<size_t>(scale * width);
			const size_t scaledHeight = static_cast<size_t>(scale * height);
			if (doFlip) {
				return (scale < 1)
					? resizeLinear(scaledWidth, scaledHeight).flipXY()
					: flipXY().resizeLinear(scaledWidth, scaledHeight);
			}
			return resizeLinear(scaledWidth, scaledHeight);
		}

		[[nodiscard]] Image resizeLinear(size_t scaledWidth, size_t scaledHeight) const {
			if ((scaledWidth == 0) && (width != 0)) scaledWidth = 1;
			if ((scaledHeight == 0) && (height != 0)) scaledHeight = 1;
			if ((getArea() == 0) || ((scaledWidth * scaledHeight) == 0))
				return Image(scaledWidth, scaledHeight);
			const double scaleX = static_cast<double>(width - 1) / static_cast<double>(scaledWidth);
			const double scaleY = static_cast<double>(height - 1) / static_cast<double>(scaledHeight);
			Image result(scaledWidth, scaledHeight);
			for (size_t y = 0; y < scaledHeight; y++) {
				const double srcY = (y + 0.5) * scaleY;
				const size_t srcY0 = static_cast<size_t>(srcY);
				for (size_t x = 0; x < scaledWidth; x++) {
					const double srcX = (x + 0.5) * scaleX;
					const size_t srcX0 = static_cast<size_t>(srcX);
					const auto c00 = getPixel(srcX0, srcY0);
					const auto c10 = getPixel(srcX0 + 1, srcY0);
					const auto c01 = getPixel(srcX0, srcY0 + 1);
					const auto c11 = getPixel(srcX0 + 1, srcY0 + 1);
					const auto cLerpY0 = glm::mix(c00, c10, srcX - srcX0);
					const auto cLerpY1 = glm::mix(c01, c11, srcX - srcX0);
					const auto cLerp = glm::mix(cLerpY0, cLerpY1, srcY - srcY0);
					result.setPixel(x, y, cLerp);
				}
			}
			return result;
		}

		template<size_t c = channels, std::enable_if_t<std::is_same_v<data_type, unsigned char>, int> = 0>
		[[nodiscard]] Image whiten() const {
			// find the maximum luma value of the image
			float maxLuma = 0;
			for (size_t i = 0; i < (width * height); i++) {
				const auto p = getPixel(i);
				if constexpr (channels == 1) {
					const auto y = p / 255.0f;
					if (y > maxLuma) maxLuma = y;
				} else if constexpr (channels == 2) {
					if (p.y > 0) {
						const auto y = p.x / 255.0f;
						if (y > maxLuma) maxLuma = y;
					}
				} else if constexpr (channels == 3) {
					const auto y = Color{ p }.getLuma();
					if (y > maxLuma) maxLuma = y;
				} else if constexpr (channels == 4) {
					if (p.w > 0) {
						const auto y = Color{ p }.getLuma();
						if (y > maxLuma) maxLuma = y;
					}
				}
			}
			Image<channels, data_type> result(width, height);
			for (size_t i = 0; i < (width * height); i++) {
				const auto p = getPixel(i);
				if constexpr (channels == 1) {
					const auto y = static_cast<data_type>(((p / 255.0f) / maxLuma) * 255.0f);
					result.setPixel(i, y);
				} else if constexpr (channels == 2) {
					const auto y = static_cast<data_type>(((p.x / 255.0f) / maxLuma) * 255.0f);
					result.setPixel(i, pixel_type{ y, p.y });
				} else if constexpr (channels == 3) {
					const auto y = static_cast<data_type>((Color{ p }.getLuma() / maxLuma) * 0xFF);
					result.setPixel(i, pixel_type{ y, y, y });
				} else if constexpr (channels == 4) {
					const auto y = static_cast<data_type>((Color{ p }.getLuma() / maxLuma) * 0xFF);
					result.setPixel(i, pixel_type{ y, y, y, p.w });
				}
			}
			return result;
		}


		template<typename F,
				size_t channelsOut = image_traits::transformer_channels_out_v<channels, data_type, F>,
				typename DataTypeOut = image_traits::transformer_data_type_out_t<channels, data_type, F>>
		[[nodiscard]] Image<channelsOut, DataTypeOut> transform(const F& transformer) const {
			Image<channelsOut, DataTypeOut> result(width, height);
			for (size_t i = 0; i < (width * height); i++)
				result.setPixel(i, transformer(getPixel(i)));
			return result;
		}


		template<size_t c = channels, std::enable_if_t<std::is_same_v<data_type, unsigned char>, int> = 0>
		void writeImage(const char* path) const {
			const std::string imgPath(path);
			if (data && (getArea() > 0))
				stbi_write_png(path, width, height, channels, data.get(), width * channels);
		}
		template<size_t c = channels, std::enable_if_t<std::is_same_v<data_type, unsigned char>, int> = 0>
		void writeImage(const std::string& path) const {
			const std::string imgPath(path);
			if (data && (getArea() > 0))
				stbi_write_png(path.c_str(), width, height, channels, data.get(), width * channels);
		}


		inline pixel_type getPixel(const coord_type& pos) const {
			return getPixel((pos.y * width) + pos.x);
		}
		inline pixel_type getPixel(const size_t x, const size_t y) const {
			return getPixel((y * width) + x);
		}
		inline pixel_type getPixel(const size_t i) const {
			if constexpr (channels == 1) {
				return data[i];
			} else if constexpr (channels == 2) {
				return { data[i * channels], data[(i * channels) + 1] };
			} else if constexpr (channels == 3) {
				return { data[i * channels], data[(i * channels) + 1], data[(i * channels) + 2] };
			} else if constexpr (channels == 4) {
				return { data[i * channels], data[(i * channels) + 1], data[(i * channels) + 2], data[(i * channels) + 3] };
			}
		}

		inline void setPixel(const coord_type& pos, const pixel_type& data) {
			setPixel((pos.y * width) + pos.x, data);
		}
		inline void setPixel(const size_t x, const size_t y, const pixel_type& data) {
			setPixel((y * width) + x, data);
		}
		void setPixel(const size_t i, const pixel_type& v) {
			if constexpr (channels == 1) {
				data[i * channels] = v;
			} else {
				data[i * channels] = v.x;
				data[(i * channels) + 1] = v.y;
				if constexpr (channels > 2)
					data[(i * channels) + 2] = v.z;
				if constexpr (channels > 3)
					data[(i * channels) + 3] = v.w;
			}
		}

		template<size_t c = channels, std::enable_if_t<(std::is_same_v<data_type, unsigned char> || std::is_same_v<data_type, float>), int> = 0>
		inline Color getPixelColor(const coord_type& pos) const {
			return getPixelColor((pos.y * width) + pos.x);
		}
		template<size_t c = channels, std::enable_if_t<(std::is_same_v<data_type, unsigned char> || std::is_same_v<data_type, float>), int> = 0>
		inline Color getPixelColor(const size_t x, const size_t y) const {
			return getPixelColor((y * width) + x);
		}
		template<size_t c = channels, std::enable_if_t<(std::is_same_v<data_type, unsigned char> || std::is_same_v<data_type, float>), int> = 0>
		Color getPixelColor(size_t i) const {
			i *= channels;
			if constexpr (channels == 1) {
				return { data[i], data[i], data[i] };
			} else if constexpr (channels == 2) {
				return { data[i], data[i], data[i], data[i + 1] };
			} else if constexpr (channels == 3) {
				return { data[i], data[i + 1], data[i + 2] };
			} else if constexpr (channels == 4) {
				return { data[i], data[i + 1], data[i + 2], data[i + 3] };
			}
		}

		template<size_t c = channels, std::enable_if_t<((c == 3) || (c == 4)) && (std::is_same_v<data_type, unsigned char> || std::is_same_v<data_type, float>), int> = 0>
		void setPixelColor(const coord_type& pos, const Color& color) {
			setPixel((pos.y * width) + pos.x, static_cast<pixel_type>(color));
		}
		template<size_t c = channels, std::enable_if_t<((c == 3) || (c == 4)) && (std::is_same_v<data_type, unsigned char> || std::is_same_v<data_type, float>), int> = 0>
		void setPixelColor(const size_t x, const size_t y, const Color& color) {
			setPixel((y * width) + x, static_cast<pixel_type>(color));
		}
		template<size_t c = channels, std::enable_if_t<((c == 3) || (c == 4)) && (std::is_same_v<data_type, unsigned char> || std::is_same_v<data_type, float>), int> = 0>
		void setPixelColor(size_t i, const Color& color) {
			setPixel(i, static_cast<pixel_type>(color));
		}

	};

	[[nodiscard]] ImageVariant loadImage(std::string_view path);

	template<size_t channels>
	[[nodiscard]] Image<channels, unsigned char> loadImage(std::string_view path) {
		using data_type = unsigned char;

		const std::string imgPath { path };
		int width, height;
		data_type* texData = stbi_load(imgPath.c_str(), &width, &height, nullptr, channels);
		if (!texData) {
			//std::cerr << "Failed to load image at " << imgPath << "\n";
			throw std::runtime_error("Failed to load image at " + imgPath); // TODO: use a default texture instead
		}
		Image<channels, data_type> textureData(texData, static_cast<size_t>(width), static_cast<size_t>(height));
		if (texData) stbi_image_free(texData);
		return textureData;
	}

}