#pragma once

#include <glad/glad.h>

#include <cstdint>
#include <memory>
#include <iostream>
#include <initializer_list>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "Window.h"
#include "util/type_traits.h"
#include "util/Color.h"

namespace eng {

	enum class GLExtension {
		AMD_draw_buffers_blend,
		ARB_conservative_depth,
		ARB_draw_buffers_blend,
		ARB_internalformat_query,
		ARB_separate_shader_objects,
		ARB_shading_language_include,
		ARB_texture_filter_anisotropic,
		ARB_texture_storage,
		EXT_texture_filter_anisotropic,
	};

	enum class BufferUsage : GLenum {
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY,
	};
	enum class BufferDrawHint : GLenum {
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW,
		STREAM = GL_STREAM_DRAW,
	};
	enum class BufferReadHint : GLenum {
		STATIC = GL_STATIC_READ,
		DYNAMIC = GL_DYNAMIC_READ,
		STREAM = GL_STREAM_READ,
	};
	enum class BufferCopyHint : GLenum {
		STATIC = GL_STATIC_COPY,
		DYNAMIC = GL_DYNAMIC_COPY,
		STREAM = GL_STREAM_COPY,
	};
	enum class DrawMode : GLenum {
		POINTS = GL_POINTS,
		LINES = GL_LINES,
		LINE_LOOP = GL_LINE_LOOP,
		LINE_STRIP = GL_LINE_STRIP,
		TRIANGLES = GL_TRIANGLES,
		TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
		TRIANGLE_FAN = GL_TRIANGLE_FAN,
	};
	enum class BufferMask : GLbitfield {
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT,
	};
	enum class DrawBuffer : GLenum {
		NONE = GL_NONE,
		FRONT = GL_FRONT, // default draw buffer for single buffered contexts
		BACK = GL_BACK, // default draw buffer for double buffered contexts
		FRONT_AND_BACK = GL_FRONT_AND_BACK,
		FRONT_LEFT = GL_FRONT_LEFT, // only front buffer for monoscopic contexts
		FRONT_RIGHT = GL_FRONT_RIGHT,
		BACK_LEFT = GL_BACK_LEFT, // only back buffer for monoscopic contexts
		BACK_RIGHT = GL_BACK_RIGHT,
		COLOR_ATTACHMENT_0  = GL_COLOR_ATTACHMENT0,
		COLOR_ATTACHMENT_1  = GL_COLOR_ATTACHMENT1,
		COLOR_ATTACHMENT_2  = GL_COLOR_ATTACHMENT2,
		COLOR_ATTACHMENT_3  = GL_COLOR_ATTACHMENT3,
		COLOR_ATTACHMENT_4  = GL_COLOR_ATTACHMENT4,
		COLOR_ATTACHMENT_5  = GL_COLOR_ATTACHMENT5,
		COLOR_ATTACHMENT_6  = GL_COLOR_ATTACHMENT6,
		COLOR_ATTACHMENT_7  = GL_COLOR_ATTACHMENT7,
		COLOR_ATTACHMENT_8  = GL_COLOR_ATTACHMENT8,
		COLOR_ATTACHMENT_9  = GL_COLOR_ATTACHMENT9,
		COLOR_ATTACHMENT_10 = GL_COLOR_ATTACHMENT10,
		COLOR_ATTACHMENT_11 = GL_COLOR_ATTACHMENT11,
		COLOR_ATTACHMENT_12 = GL_COLOR_ATTACHMENT12,
		COLOR_ATTACHMENT_13 = GL_COLOR_ATTACHMENT13,
		COLOR_ATTACHMENT_14 = GL_COLOR_ATTACHMENT14,
		COLOR_ATTACHMENT_15 = GL_COLOR_ATTACHMENT15,
		COLOR_ATTACHMENT_16 = GL_COLOR_ATTACHMENT16,
		COLOR_ATTACHMENT_17 = GL_COLOR_ATTACHMENT17,
		COLOR_ATTACHMENT_18 = GL_COLOR_ATTACHMENT18,
		COLOR_ATTACHMENT_19 = GL_COLOR_ATTACHMENT19,
		COLOR_ATTACHMENT_20 = GL_COLOR_ATTACHMENT20,
		COLOR_ATTACHMENT_21 = GL_COLOR_ATTACHMENT21,
		COLOR_ATTACHMENT_22 = GL_COLOR_ATTACHMENT22,
		COLOR_ATTACHMENT_23 = GL_COLOR_ATTACHMENT23,
		COLOR_ATTACHMENT_24 = GL_COLOR_ATTACHMENT24,
		COLOR_ATTACHMENT_25 = GL_COLOR_ATTACHMENT25,
		COLOR_ATTACHMENT_26 = GL_COLOR_ATTACHMENT26,
		COLOR_ATTACHMENT_27 = GL_COLOR_ATTACHMENT27,
		COLOR_ATTACHMENT_28 = GL_COLOR_ATTACHMENT28,
		COLOR_ATTACHMENT_29 = GL_COLOR_ATTACHMENT29,
		COLOR_ATTACHMENT_30 = GL_COLOR_ATTACHMENT30,
		COLOR_ATTACHMENT_31 = GL_COLOR_ATTACHMENT31,
	};

	class UIRenderer;
	class FontRenderer;

	class Renderer {
	public:
		using ClearBit = BufferMask;

		static constexpr float perspectiveNearPlane = 0.05f;
		static constexpr float perspectiveFarPlane = 1000.0f;

		Window* const window;
	private:
		int uiScale = 2;
		float fov = 60.0f;

		glm::mat4 projectionMatrix = glm::mat4(1.0);
		glm::mat4 orthoMatrix = glm::mat4(1.0);

		glm::ivec2 windowSize;
		glm::vec2 windowContentScale;
		glm::ivec2 windowSizeScaled;
		glm::vec2 uiSize;

		std::unique_ptr<UIRenderer> uiRenderer;
		std::unique_ptr<FontRenderer> fontRenderer;

		static bool doubleBuffer;
		static int32_t maxDrawBuffers, maxColorAttachments, maxTextureSize, max3dTextureSize, maxCubeMapTextureSize, maxBufferTextureSize, maxArrayTextureLayers, maxFragTextureUnits, maxVertTextureUnits, maxGeomTextureUnits, maxCombinedTextureUnits, maxVertexAttribs;
	public:

		Renderer(Window* const window);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;

		Renderer& operator =(const Renderer&) = delete;
		Renderer& operator =(Renderer&&) = delete;

		// called after the gl context has been created
		void init();

		static inline void clear(ClearBit clearBits) noexcept { glClear(to_underlying(clearBits)); }

		static inline void setClearColor(const Color& color) noexcept { glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f); }

		static inline void setClearDepth(const double depth) noexcept { glClearDepth(depth); }
		static inline void setClearDepth(const float depth) noexcept { glClearDepth(static_cast<double>(depth)); }

		static inline void setClearStencil(const int32_t stencil) noexcept { glClearStencil(stencil); }

		static inline void setActiveDrawBuffer(const DrawBuffer buffer) noexcept { glDrawBuffer(to_underlying(buffer)); }
		static void setActiveDrawBuffers(const std::initializer_list<DrawBuffer> buffers) noexcept;
		static inline void setActiveReadBuffer(const DrawBuffer buffer) noexcept { glReadBuffer(to_underlying(buffer)); }

		static inline void setViewport(int width, int height) noexcept { glViewport(0, 0, width, height); }
		static inline void setViewport(int minX, int minY, int maxX, int maxY) noexcept { glViewport(minX, minY, maxX, maxY); }

		static void setActiveTextureUnit(unsigned char textureUnit);

		inline const glm::mat4& getProjectionMatrix() const noexcept { return projectionMatrix; }
		inline const glm::mat4& getOrthoMatrix() const noexcept { return orthoMatrix; }

		void updateProjectionMatrices();

		inline float getFOV() const noexcept { return fov; }
		void setFOV(float fov) {
			this->fov = fov;
			updateProjectionMatrices();
		}
		inline int getUIScale() const noexcept { return uiScale; }
		void setUIScale(int uiScale) {
			this->uiScale = uiScale;
			handleWindowResize();
		}

		inline const glm::ivec2& getWindowSize() const noexcept { return windowSize; }
		inline const glm::vec2& getWindowContentScale() const noexcept { return windowContentScale; }
		inline const glm::ivec2& getWindowSizeScaled() const noexcept { return windowSizeScaled; }
		inline const glm::vec2& getUISize() const noexcept { return uiSize; }

		void handleWindowResize();
		void handleWindowResize(int windowWidth, int windowHeight);
		void handleWindowRescale(float scaleX, float scaleY);
		void handleWindowResize(int windowWidth, int windowHeight, float scaleX, float scaleY);

		inline UIRenderer* getUIRenderer() noexcept { return uiRenderer.get(); }
		inline const UIRenderer* getUIRenderer() const noexcept { return uiRenderer.get(); }

		inline FontRenderer* getFontRenderer() noexcept { return fontRenderer.get(); }
		inline const FontRenderer* getFontRenderer() const noexcept { return fontRenderer.get(); }

		void takeScreenshot() const;

		template<GLExtension Ext>
		static inline bool hasExtension() noexcept {
			if constexpr (Ext == GLExtension::AMD_draw_buffers_blend)
				return GLAD_GL_AMD_draw_buffers_blend;
			else if constexpr (Ext == GLExtension::ARB_conservative_depth)
				return GLAD_GL_ARB_conservative_depth;
			else if constexpr (Ext == GLExtension::ARB_draw_buffers_blend)
				return GLAD_GL_ARB_draw_buffers_blend;
			else if constexpr (Ext == GLExtension::ARB_internalformat_query)
				return GLAD_GL_ARB_internalformat_query;
			else if constexpr (Ext == GLExtension::ARB_separate_shader_objects)
				return GLAD_GL_ARB_separate_shader_objects;
			else if constexpr (Ext == GLExtension::ARB_shading_language_include)
				return GLAD_GL_ARB_shading_language_include;
			else if constexpr (Ext == GLExtension::ARB_texture_filter_anisotropic)
				return GLAD_GL_ARB_texture_filter_anisotropic;
			else if constexpr (Ext == GLExtension::ARB_texture_storage)
				return GLAD_GL_ARB_texture_storage;
			else if constexpr (Ext == GLExtension::EXT_texture_filter_anisotropic)
				return GLAD_GL_EXT_texture_filter_anisotropic;
			else
				return false;
		}


		static inline bool isDoubleBuffered() noexcept { return doubleBuffer; }
		static inline size_t getMaxDrawBuffers() noexcept { return static_cast<size_t>(maxDrawBuffers); }
		static inline size_t getMaxColorAttachments() noexcept { return static_cast<size_t>(maxColorAttachments); }
		static inline size_t getMax1dTextureSize() noexcept { return static_cast<size_t>(maxTextureSize); }
		static inline size_t getMax2dTextureSize() noexcept { return static_cast<size_t>(maxTextureSize); }
		static inline size_t getMax3dTextureSize() noexcept { return static_cast<size_t>(max3dTextureSize); }
		static inline size_t getMaxCubeMapTextureSize() noexcept { return static_cast<size_t>(maxCubeMapTextureSize); }
		static inline size_t getMaxBufferTextureSize() noexcept { return static_cast<size_t>(maxBufferTextureSize); }
		static inline size_t getMaxArrayTextureLayers() noexcept { return static_cast<size_t>(maxArrayTextureLayers); }
		static inline size_t getMaxFragTextureUnits() noexcept { return static_cast<size_t>(maxFragTextureUnits); }
		// could be 0, but should be at least 16
		static inline size_t getMaxVertTextureUnits() noexcept { return static_cast<size_t>(maxVertTextureUnits); }
		static inline size_t getMaxGeomTextureUnits() noexcept { return static_cast<size_t>(maxGeomTextureUnits); }
		static inline size_t getMaxCombinedTextureUnits() noexcept { return static_cast<size_t>(maxCombinedTextureUnits); }
		static inline size_t getMaxVertexAttribs() noexcept { return static_cast<size_t>(maxVertexAttribs); }
	};

	inline constexpr BufferMask operator |(BufferMask lhs, BufferMask rhs) noexcept {
		return BufferMask{ to_underlying(lhs) | to_underlying(rhs) };
	}
	inline constexpr BufferMask operator &(BufferMask lhs, BufferMask rhs) noexcept {
		return BufferMask{ to_underlying(lhs) & to_underlying(rhs) };
	}


	std::ostream& operator <<(std::ostream& os, const GLExtension& c);

}