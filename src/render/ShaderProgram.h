#pragma once

#include <string>
#include <string_view>
#include <map>

#include <glm/glm.hpp>

#include "Renderer.h"

namespace eng {

	class ShaderProgram {
	private:
		uint32_t id;
		std::map<std::string, uint32_t> uniforms;

		static uint32_t boundProgram;
	public:
		static ShaderProgram load(std::string_view vertexShaderPath, std::string_view fragmentShaderPath, std::string_view geometryShaderPath);
		static ShaderProgram load(std::string_view vertexShaderPath, std::string_view fragmentShaderPath);

		ShaderProgram(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc, const std::string& geometryShaderSrc);
		ShaderProgram(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc);

		~ShaderProgram();

		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&);

		ShaderProgram& operator =(const ShaderProgram&) = delete;
		ShaderProgram& operator =(ShaderProgram&&);

		void createUniform(std::string uniformName);

		void setUniform(const std::string& uniformName, float value) const;
		void setUniform(const std::string& uniformName, const glm::vec2& value) const;
		void setUniform(const std::string& uniformName, const glm::vec3& value) const;
		void setUniform(const std::string& uniformName, const glm::vec4& value) const;
		void setUniform(const std::string& uniformName, const glm::mat3& value) const;
		void setUniform(const std::string& uniformName, const glm::mat4& value) const;
		void setUniform(const std::string& uniformName, int value) const;
		void setUniform(const std::string& uniformName, const glm::ivec2& value) const;
		void setUniform(const std::string& uniformName, const glm::ivec3& value) const;
		void setUniform(const std::string& uniformName, const glm::ivec4& value) const;

		bool isBound() const noexcept;

		void bind() const noexcept;

		static void unbind() noexcept;
	};

}
