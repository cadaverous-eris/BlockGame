#pragma once

#include <array>
#include <vector>
#include <type_traits>

#include <glm/vec2.hpp>

#include "RenderLayer.h"
#include "render/Renderer.h"
#include "render/VertexArray.h"
#include "render/VertexFormat.h"
#include "render/VertexBuffer.h"
#include "render/ShaderProgram.h"
#include "render/texture/Texture.h"
#include "render/Camera.h"
#include "world/World.h"
#include "model/block/BlockQuad.h"
#include "util/direction.h"
#include "util/math/Frustum.h"
#include "render/world/chunk/ChunkBakery.h"
#include "render/FrameBuffer.h"
#include "render/RenderBuffer.h"
#include "util/resources/Image.h"

namespace eng {

	class PlayState;
	
	class WorldRenderer {
	public:

	private:
		Renderer* renderer;

		ChunkBakery chunkBakery;

		bool useFallbackTransparency;

		std::array<VertexArray, render_layer::layers.size()> blockVAOs;
		std::array<VertexBuffer, render_layer::layers.size()> blockVBOs;

		std::array<ShaderProgram, render_layer::layers.size()> blockShaders;

		VertexArray blockSelectionVAO;
		VertexBuffer blockSelectionVBO;
		ShaderProgram blockSelectionShader = ShaderProgram::load("block_selection.vert", "block_selection.frag");

		std::array<std::vector<BlockQuad>, render_layer::layers.size()> blockQuads;


		FrameBuffer worldFBO;
		Texture worldFBOColorAttachment;
		RenderBuffer worldFBODepthStencilAttachment { TextureInternalFormat::DEPTH_U24_STENCIL_U8 };
		VertexArray worldFrameBufferVAO;
		VertexBuffer worldFrameBufferVBO;
		ShaderProgram worldFBOShader = ShaderProgram::load("world/world_fbo.vert", "world/world_fbo.frag");

		struct WorldFBOVertex {
			static inline const VertexFormat format {
				{ "pos",      VertexAttribute::Type::FLOAT, 2, },
				{ "texCoord", VertexAttribute::Type::FLOAT, 2, },
			};
			glm::vec2 pos;
			glm::vec2 texCoord;
			WorldFBOVertex(const glm::vec2& pos, const glm::vec2& texCoord) noexcept : pos(pos), texCoord(texCoord) {}
		};
		static inline const std::array<WorldFBOVertex, 4> worldFBOQuadData = {
			WorldFBOVertex({ -1.0f,  1.0f }, { 0.0f, 1.0f }),
			WorldFBOVertex({ -1.0f, -1.0f }, { 0.0f, 0.0f }),
			WorldFBOVertex({  1.0f,  1.0f }, { 1.0f, 1.0f }),
			WorldFBOVertex({  1.0f, -1.0f }, { 1.0f, 0.0f }),
		};


		FrameBuffer transparentsFBO;
		Texture transparentsAccumTex;
		Texture transparentsRevealageTex;
		ShaderProgram transparentsFallbackRevealageShader = ShaderProgram::load("world/blocks.vert", "world/blocks_transparent_fallback_revealage.frag", "world/blocks.geom");
		ShaderProgram transparentsFBOShader = ShaderProgram::load("world/world_fbo.vert", "world/transparents_fbo.frag");


	public:
		WorldRenderer(Renderer* const);

		void render(const float partialTicks, const PlayState& gameState, const World* world, const Camera* camera);

		void resize(size_t width, size_t height);

		ImageRGBA captureFrame() const;

		inline ChunkBakery& getChunkBakery() noexcept { return chunkBakery; }

	private:

		void renderChunks(const float partialTicks, const PlayState& gameState, const World* world, const Camera* camera);

		void renderBlockSelectionOutline(const float partialTicks, const glm::mat4& mvpMatrix, const Direction selectionFace, AxisAlignedBox<float> selectionBox);


	};

}