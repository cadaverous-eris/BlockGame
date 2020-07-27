#include "Image.h"

namespace eng {

	[[nodiscard]] ImageVariant loadImage(std::string_view path) {
		using data_type = unsigned char;

		const std::string imgPath { path };
		int width, height, numChannels;
		data_type* texData = stbi_load(imgPath.c_str(), &width, &height, &numChannels, 0);
		if (!texData) {
			//std::cerr << "Failed to load image at " << imgPath << "\n";
			throw std::runtime_error("Failed to load image at " + imgPath); // TODO: use a default texture instead
		}

		switch (numChannels) {
		case 1:
		{
			Image1 textureData(texData, static_cast<size_t>(width), static_cast<size_t>(height));
			if (texData) stbi_image_free(texData);
			return textureData;
		}
		break;
		case 2:
		{
			Image2 textureData(texData, static_cast<size_t>(width), static_cast<size_t>(height));
			if (texData) stbi_image_free(texData);
			return textureData;
		}
		break;
		case 3:
		{
			Image3 textureData(texData, static_cast<size_t>(width), static_cast<size_t>(height));
			if (texData) stbi_image_free(texData);
			return textureData;
		}
		break;
		case 4:
		{
			Image4 textureData(texData, static_cast<size_t>(width), static_cast<size_t>(height));
			if (texData) stbi_image_free(texData);
			return textureData;
		}
		break;
		default:
			if (texData) stbi_image_free(texData);
			std::cerr << "Invalid channel count (" << numChannels << ") in image loaded from " << imgPath << "\n";
			throw std::runtime_error("Invalid channel count in image loaded from " + imgPath); // TODO: use a default texture instead
		}
	}



}
