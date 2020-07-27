#include "VertexFormat.h"

#include <stdexcept>

namespace eng {

	VertexFormat::VertexFormat(std::initializer_list<VertexAttribute> attributes) :
			attributes{attributes}, stride(0) {
		std::size_t offset = 0;
		for (std::size_t i = 0; i < attributes.size(); i++) {
			this->attributes[i].offset = offset;
			offset += this->attributes[i].getSizeBytes();
			stride += this->attributes[i].getSizeBytes();
		}
	}
	VertexFormat::VertexFormat(const size_t stride, std::initializer_list<VertexAttribute> attributes) :
			attributes { attributes }, stride(stride) {
		std::size_t offset = 0;
		for (std::size_t i = 0; i < attributes.size(); i++) {
			this->attributes[i].offset = offset;
			offset += this->attributes[i].getSizeBytes();
		}
	}

}