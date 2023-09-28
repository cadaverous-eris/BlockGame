#include "ShaderProgram.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <utility>

#include <glm/gtc/type_ptr.hpp>

#include "../util/IOUtils.h"

namespace eng {

	uint32_t ShaderProgram::boundProgram = 0;

	ShaderProgram ShaderProgram::load(std::string_view vertexShaderPath, std::string_view fragmentShaderPath, std::string_view geometryShaderPath) {
		std::string vertexShaderString = readTextFile(std::string("res/shaders/").append(vertexShaderPath));
		std::string fragmentShaderString = readTextFile(std::string("res/shaders/").append(fragmentShaderPath));
		std::string geometryShaderString = readTextFile(std::string("res/shaders/").append(geometryShaderPath));
		return ShaderProgram(vertexShaderString, fragmentShaderString, geometryShaderString);
	}
	ShaderProgram ShaderProgram::load(std::string_view vertexShaderPath, std::string_view fragmentShaderPath) {
		std::string vertexShaderString = readTextFile(std::string("res/shaders/").append(vertexShaderPath));
		std::string fragmentShaderString = readTextFile(std::string("res/shaders/").append(fragmentShaderPath));
		return ShaderProgram(vertexShaderString, fragmentShaderString);
	}

	ShaderProgram::ShaderProgram(const std::string& vertexShaderString, const std::string& fragmentShaderString, const std::string& geometryShaderString) {
		const char* vertexShaderSource = vertexShaderString.c_str();
		const char* fragmentShaderSource = fragmentShaderString.c_str();
		const char* geometryShaderSource = geometryShaderString.c_str();
		const int32_t vertexShaderSourceLength = static_cast<int32_t>(vertexShaderString.size());
		const int32_t fragmentShaderSourceLength = static_cast<int32_t>(fragmentShaderString.size());
		const int32_t geometryShaderSourceLength = static_cast<int32_t>(geometryShaderString.size());

		int success;
		char infoLog[1024];

		// vertex shader
		const int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, &vertexShaderSourceLength);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
		if (!success) {
			glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Vertex Shader Program compilation failed");
		}

		// fragment shader
		const int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, &fragmentShaderSourceLength);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Fragment Shader Program compilation failed");
		}

		// geometry shader
		const int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometryShaderSource, &geometryShaderSourceLength);
		glCompileShader(geometryShader);
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
		if (!success) {
			glGetShaderInfoLog(geometryShader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Geometry Shader Program compilation failed");
		}

		// link shaders
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glAttachShader(id, geometryShader);
		glLinkProgram(id);
		glGetProgramiv(id, GL_LINK_STATUS, &success); // check for linking errors
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Shader Program linking failed");
		}
		// clean up individual shaders
		glDetachShader(id, vertexShader);
		glDeleteShader(vertexShader);
		glDetachShader(id, fragmentShader);
		glDeleteShader(fragmentShader);
		glDetachShader(id, geometryShader);
		glDeleteShader(geometryShader);

		/*GLint i;
		GLint count;
		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)
		const GLsizei bufSize = 16; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
		std::printf("Active Uniforms: %d\n", count);
		for (i = 0; i < count; i++) {
			glGetActiveUniform(id, (GLuint) i, bufSize, &length, &size, &type, name);
			std::printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		}*/
	}
	ShaderProgram::ShaderProgram(const std::string& vertexShaderString, const std::string& fragmentShaderString) {
		const char* vertexShaderSource = vertexShaderString.c_str();
		const char* fragmentShaderSource = fragmentShaderString.c_str();
		const int32_t vertexShaderSourceLength = static_cast<int32_t>(vertexShaderString.size());
		const int32_t fragmentShaderSourceLength = static_cast<int32_t>(fragmentShaderString.size());

		int success;
		char infoLog[1024];

		// vertex shader
		const int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, &vertexShaderSourceLength);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
		if (!success) {
			glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Vertex Shader Program compilation failed");
		}

		// fragment shader
		const int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, &fragmentShaderSourceLength);
		glCompileShader(fragmentShader);
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
		if (!success) {
			glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
			std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Fragment Shader Program compilation failed");
		}

		// link shaders
		id = glCreateProgram();
		glAttachShader(id, vertexShader);
		glAttachShader(id, fragmentShader);
		glLinkProgram(id);
		glGetProgramiv(id, GL_LINK_STATUS, &success); // check for linking errors
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			throw std::runtime_error("Shader Program linking failed");
		}
		// clean up individual shaders
		glDetachShader(id, vertexShader);
		glDeleteShader(vertexShader);
		glDetachShader(id, fragmentShader);
		glDeleteShader(fragmentShader);

		/*GLint i;
		GLint count;
		GLint size; // size of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)
		const GLsizei bufSize = 16; // maximum name length
		GLchar name[bufSize]; // variable name in GLSL
		GLsizei length; // name length
		glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &count);
		std::printf("Active Uniforms: %d\n", count);
		for (i = 0; i < count; i++) {
			glGetActiveUniform(id, (GLuint) i, bufSize, &length, &size, &type, name);
			std::printf("Uniform #%d Type: %u Name: %s\n", i, type, name);
		}*/
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& b) :
			id(std::exchange(b.id, 0)), uniforms(std::move(b.uniforms)) {}
	ShaderProgram& ShaderProgram::operator =(ShaderProgram&& b) {
		if (&b != this) {
			std::swap(b.id, id);
			uniforms = std::move(b.uniforms);
		}
		return *this;
	}

	ShaderProgram::~ShaderProgram() {
		if (id != 0) {
			if (boundProgram == id) boundProgram = 0;
			glDeleteProgram(id);
		}
	}

	void ShaderProgram::createUniform(std::string uniformName) {
		// error if uniform already exists
		if (uniforms.find(uniformName) != uniforms.end()) {
			throw std::runtime_error("Uniform already exists: " + uniformName);
		}

		int uniformLocation = glGetUniformLocation(id, uniformName.c_str());
		if (uniformLocation >= 0) {
			uniforms[uniformName] = uniformLocation;
		} else {
			std::cerr << "Could not find uniform: " << uniformName << std::endl;
			//throw std::runtime_error("Could not find uniform: " + uniformName);
		}
	}

	void ShaderProgram::setUniform(const std::string& uniformName, float value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform1f(u->second, value);
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::vec2& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform2fv(u->second, 1, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::vec3& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform3fv(u->second, 1, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::vec4& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform4fv(u->second, 1, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::mat3& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniformMatrix3fv(u->second, 1, GL_FALSE, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::mat4& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniformMatrix4fv(u->second, 1, GL_FALSE, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, int value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform1i(u->second, value);
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::ivec2& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform2iv(u->second, 1, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::ivec3& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform3iv(u->second, 1, glm::value_ptr(value));
	}
	void ShaderProgram::setUniform(const std::string& uniformName, const glm::ivec4& value) const {
		bind();
		const auto u = uniforms.find(uniformName);
		if (u != uniforms.end()) glUniform4iv(u->second, 1, glm::value_ptr(value));
	}

	bool ShaderProgram::isBound() const noexcept {
		return (id != 0) && (boundProgram == id);
	}

	void ShaderProgram::bind() const noexcept {
		if ((id != 0) && (boundProgram != id)) {
			boundProgram = id;
			glUseProgram(id);
		}
	}

	void ShaderProgram::unbind() noexcept {
		if (boundProgram != 0) {
			boundProgram = 0;
			glUseProgram(0);
		}
	}

}