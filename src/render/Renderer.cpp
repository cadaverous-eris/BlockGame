#include "Renderer.h"

#include <stdexcept>
#include <string>
#include <iostream>

#include "render/ui/UIRenderer.h"
#include "render/font/FontRenderer.h"

namespace eng {

	bool Renderer::doubleBuffer = false;
	int32_t Renderer::maxDrawBuffers = 0;
	int32_t Renderer::maxColorAttachments = 0;
	int32_t Renderer::maxTextureSize = 0;
	int32_t Renderer::max3dTextureSize = 0;
	int32_t Renderer::maxCubeMapTextureSize = 0;
	int32_t Renderer::maxBufferTextureSize = 0;
	int32_t Renderer::maxArrayTextureLayers = 0;
	int32_t Renderer::maxFragTextureUnits = 0;
	int32_t Renderer::maxVertTextureUnits = 0;
	int32_t Renderer::maxGeomTextureUnits = 0;
	int32_t Renderer::maxCombinedTextureUnits = 0;
	int32_t Renderer::maxVertexAttribs = 0;

	Renderer::Renderer(Window* const window) :
			window(window),
			windowSize(window->getSize()),
			windowContentScale(window->getContentScale()),
			windowSizeScaled(static_cast<glm::vec2>(window->getSize()) / window->getContentScale()),
			uiRenderer(),
			fontRenderer() {
		updateProjectionMatrices();
	}

	Renderer::~Renderer() {}

	void Renderer::init() {
		// initialize glad
		if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
			throw std::runtime_error("Failed to initialize OpenGL context");
		}

		GLboolean doubleBufferVal {};
		glGetBooleanv(GL_DOUBLEBUFFER, &doubleBufferVal);
		doubleBuffer = doubleBufferVal;
		glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffers);
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &max3dTextureSize);
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize);
		glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &maxBufferTextureSize);
		glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragTextureUnits);
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxVertTextureUnits); // should be at least 16, but could be 0
		glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &maxGeomTextureUnits);
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombinedTextureUnits);
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);

		std::cout << "Max Draw Buffers:        " << maxDrawBuffers << "\n";
		std::cout << "Max Buffer Texture Size: " << maxBufferTextureSize << "\n";

		std::cout << "\nExtensions:\n";
		std::cout << GLExtension::AMD_draw_buffers_blend << ": " << hasExtension<GLExtension::AMD_draw_buffers_blend>() << '\n';
		std::cout << GLExtension::ARB_conservative_depth << ": " << hasExtension<GLExtension::ARB_conservative_depth>() << '\n';
		std::cout << GLExtension::ARB_draw_buffers_blend << ": " << hasExtension<GLExtension::ARB_draw_buffers_blend>() << '\n';
		std::cout << GLExtension::ARB_internalformat_query << ": " << hasExtension<GLExtension::ARB_internalformat_query>() << '\n';
		std::cout << GLExtension::ARB_separate_shader_objects << ": " << hasExtension<GLExtension::ARB_separate_shader_objects>() << '\n';
		std::cout << GLExtension::ARB_shading_language_include << ": " << hasExtension<GLExtension::ARB_shading_language_include>() << '\n';
		std::cout << GLExtension::ARB_texture_filter_anisotropic << ": " << hasExtension<GLExtension::ARB_texture_filter_anisotropic>() << '\n';
		std::cout << GLExtension::ARB_texture_storage << ": " << hasExtension<GLExtension::ARB_texture_storage>() << '\n';
		std::cout << GLExtension::EXT_texture_filter_anisotropic << ": " << hasExtension<GLExtension::EXT_texture_filter_anisotropic>() << '\n';
		std::cout << '\n';

		uiRenderer = std::make_unique<UIRenderer>(this);

		fontRenderer = std::make_unique<FontRenderer>(this);

		// TODO: more initialization?
	}

	void Renderer::updateProjectionMatrices() {
		const float ratio = (windowSize.x != windowSize.y) ?
			(static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y)) : 1.0f;
		projectionMatrix = glm::perspective(glm::radians(fov), ratio, perspectiveNearPlane, perspectiveFarPlane);
		orthoMatrix = glm::ortho(0.0f, static_cast<float>(windowSizeScaled.x), static_cast<float>(windowSizeScaled.y), 0.0f);
	}

	void Renderer::setActiveTextureUnit(unsigned char textureUnit) {
		int unitId = GL_TEXTURE0;
		switch (textureUnit) {
		case 0:  unitId = GL_TEXTURE0;  break;
		case 1:  unitId = GL_TEXTURE1;  break;
		case 2:  unitId = GL_TEXTURE2;  break;
		case 3:  unitId = GL_TEXTURE3;  break;
		case 4:  unitId = GL_TEXTURE4;  break;
		case 5:  unitId = GL_TEXTURE5;  break;
		case 6:  unitId = GL_TEXTURE6;  break;
		case 7:  unitId = GL_TEXTURE7;  break;
		case 8:  unitId = GL_TEXTURE8;  break;
		case 9:  unitId = GL_TEXTURE9;  break;
		case 10: unitId = GL_TEXTURE10; break;
		case 11: unitId = GL_TEXTURE11; break;
		case 12: unitId = GL_TEXTURE12; break;
		case 13: unitId = GL_TEXTURE13; break;
		case 14: unitId = GL_TEXTURE14; break;
		case 15: unitId = GL_TEXTURE15; break;
		case 16: unitId = GL_TEXTURE16; break;
		case 17: unitId = GL_TEXTURE17; break;
		case 18: unitId = GL_TEXTURE18; break;
		case 19: unitId = GL_TEXTURE19; break;
		case 20: unitId = GL_TEXTURE20; break;
		case 21: unitId = GL_TEXTURE21; break;
		case 22: unitId = GL_TEXTURE22; break;
		case 23: unitId = GL_TEXTURE23; break;
		case 24: unitId = GL_TEXTURE24; break;
		case 25: unitId = GL_TEXTURE25; break;
		case 26: unitId = GL_TEXTURE26; break;
		case 27: unitId = GL_TEXTURE27; break;
		case 28: unitId = GL_TEXTURE28; break;
		case 29: unitId = GL_TEXTURE29; break;
		case 30: unitId = GL_TEXTURE30; break;
		case 31: unitId = GL_TEXTURE31; break;
		//case 32: unitId = GL_TEXTURE32; break;
		//case 33: unitId = GL_TEXTURE33; break;
		//case 34: unitId = GL_TEXTURE34; break;
		//case 35: unitId = GL_TEXTURE35; break;
		//case 36: unitId = GL_TEXTURE36; break;
		//case 37: unitId = GL_TEXTURE37; break;
		//case 38: unitId = GL_TEXTURE38; break;
		//case 39: unitId = GL_TEXTURE39; break;
		//case 40: unitId = GL_TEXTURE40; break;
		//case 41: unitId = GL_TEXTURE41; break;
		//case 42: unitId = GL_TEXTURE42; break;
		//case 43: unitId = GL_TEXTURE43; break;
		//case 44: unitId = GL_TEXTURE44; break;
		//case 45: unitId = GL_TEXTURE45; break;
		//case 46: unitId = GL_TEXTURE46; break;
		//case 47: unitId = GL_TEXTURE47; break;
		default:
			throw std::runtime_error(std::string("Texture unit ") + std::to_string(textureUnit) + std::string(" is unsupported"));
		}
		glActiveTexture(unitId);
	}

	void Renderer::setActiveDrawBuffers(const std::initializer_list<DrawBuffer> buffers) noexcept {
		static_assert(sizeof(DrawBuffer) == sizeof(std::underlying_type_t<DrawBuffer>));
		using T = std::underlying_type_t<DrawBuffer>;
		glDrawBuffers(buffers.size(), reinterpret_cast<const T* const>(buffers.begin()));
	}


	void Renderer::handleWindowResize(int windowWidth, int windowHeight) {
		setViewport(windowWidth, windowHeight);
		windowSize = { windowWidth, windowHeight };
		windowSizeScaled = static_cast<glm::vec2>(windowSize) / windowContentScale;
		updateProjectionMatrices();
	}
	void Renderer::handleWindowRescale(float scaleX, float scaleY) {
		windowContentScale = { scaleX, scaleY };
		windowSizeScaled = static_cast<glm::vec2>(windowSize) / windowContentScale;
		updateProjectionMatrices();
	}
	void Renderer::handleWindowResize(int windowWidth, int windowHeight, float scaleX, float scaleY) {
		setViewport(windowWidth, windowHeight);
		windowSize = { windowWidth, windowHeight };
		windowContentScale = { scaleX, scaleY };
		windowSizeScaled = static_cast<glm::vec2>(windowSize) / windowContentScale;
		updateProjectionMatrices();
	}


	std::ostream& operator <<(std::ostream& os, const GLExtension& c) {
		switch (c) {
		case GLExtension::AMD_draw_buffers_blend:
			return os << "GL_AMD_draw_buffers_blend";
		case GLExtension::ARB_conservative_depth:
			return os << "GL_ARB_conservative_depth";
		case GLExtension::ARB_draw_buffers_blend:
			return os << "GL_ARB_draw_buffers_blend";
		case GLExtension::ARB_internalformat_query:
			return os << "GL_ARB_internalformat_query";
		case GLExtension::ARB_separate_shader_objects:
			return os << "GL_ARB_separate_shader_objects";
		case GLExtension::ARB_shading_language_include:
			return os << "GL_ARB_shading_language_include";
		case GLExtension::ARB_texture_filter_anisotropic:
			return os << "GL_ARB_texture_filter_anisotropic";
		case GLExtension::ARB_texture_storage:
			return os << "GL_ARB_texture_storage";
		case GLExtension::EXT_texture_filter_anisotropic:
			return os << "GL_EXT_texture_filter_anisotropic";
		default:
			return os << "unknown_extension";
		}
	}

}