#include "WorldRenderer.h"

#include <utility>
#include <algorithm>
#include <stdexcept>
#include <iostream>

#include <glm/gtx/norm.hpp>

#include "util/math/AxisAlignedBox.h"
#include "game_states/PlayState.h"
#include "world/chunk/Chunk.h"
#include "render/world/chunk/RenderChunk.h"
#include "util/resources/ResourceManager.h"

namespace eng {

	static std::string_view getTransparentBlockShaderSource(const bool fallback) noexcept {
		return fallback ? "world/blocks_transparent_fallback_accum.frag" : "world/blocks_transparent.frag";
	}

	WorldRenderer::WorldRenderer(Renderer* const renderer) :
			renderer(renderer),
			useFallbackTransparency(!Renderer::hasExtension<GLExtension::ARB_draw_buffers_blend>()),
			blockShaders{
				ShaderProgram::load("world/blocks.vert", "world/blocks.frag", "world/blocks.geom"),
				ShaderProgram::load("world/blocks.vert", "world/blocks_cutout.frag", "world/blocks.geom"),
				ShaderProgram::load("world/blocks.vert", getTransparentBlockShaderSource(useFallbackTransparency), "world/blocks.geom"),
			} {
		using svec2 = glm::vec<2, size_t>;

		// setup framebuffer
		worldFBO.bind(FrameBuffer::Target::DRAW_FRAMEBUFFER);
		const svec2 windowSize = renderer->getWindowSize();
		worldFBOColorAttachment.setScaleMode(TextureScaleMode::LINEAR);
		worldFBOColorAttachment.setData(nullptr, windowSize.x, windowSize.y, TextureInternalFormat::U8x4, TextureFormat::RGBA, TextureDataType::UINT8);
		worldFBODepthStencilAttachment.setSize(windowSize);
		worldFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_0, &worldFBOColorAttachment);
		worldFBO.attachRenderBuffer(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::DEPTH_STENCIL, &worldFBODepthStencilAttachment);
		if (!worldFBO.isComplete(FrameBuffer::Target::DRAW_FRAMEBUFFER)) {
			const auto status = worldFBO.getStatus(FrameBuffer::Target::DRAW_FRAMEBUFFER);
			std::cerr << "World framebuffer object is incomplete with status " << status << '\n';
			throw std::runtime_error("World framebuffer object is incomplete with status " + to_string(status));
		}
		worldFBOShader.createUniform("textureSampler");
		worldFrameBufferVAO.bind();
		worldFrameBufferVBO.setData(std::span(worldFBOQuadData.data(), worldFBOQuadData.size()), VertexBuffer::DrawHint::STATIC);
		worldFrameBufferVAO.setVertexFormat(WorldFBOVertex::format);

		// setup transparency framebuffer
		transparentsFBO.bind(FrameBuffer::Target::DRAW_FRAMEBUFFER);
		transparentsAccumTex.setScaleMode(TextureScaleMode::LINEAR);
		transparentsAccumTex.setData(nullptr, windowSize.x, windowSize.y, TextureInternalFormat::FLOAT16x4, TextureFormat::RGBA, TextureDataType::FLOAT32);
		transparentsRevealageTex.setScaleMode(TextureScaleMode::LINEAR);
		transparentsRevealageTex.setData(nullptr, windowSize.x, windowSize.y, TextureInternalFormat::U8x1, TextureFormat::RED, TextureDataType::UINT8);
		transparentsFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_0, &transparentsAccumTex);
		transparentsFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_1, &transparentsRevealageTex);
		transparentsFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_2, &worldFBOColorAttachment);
		transparentsFBO.attachRenderBuffer(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::DEPTH_STENCIL, &worldFBODepthStencilAttachment);
		if (!transparentsFBO.isComplete(FrameBuffer::Target::DRAW_FRAMEBUFFER)) {
			const auto status = transparentsFBO.getStatus(FrameBuffer::Target::DRAW_FRAMEBUFFER);
			std::cerr << "Transparents framebuffer object is incomplete with status " << status << '\n';
			throw std::runtime_error("Transparents framebuffer object is incomplete with status " + to_string(status));
		}
		transparentsFBOShader.createUniform("accumTexSampler");
		transparentsFBOShader.createUniform("revealageTexSampler");
		transparentsFBOShader.createUniform("colorTexSampler");

		FrameBuffer::unbind(FrameBuffer::Target::DRAW_FRAMEBUFFER);

		// lambda function to create uniforms for block shaders
		constexpr auto createBlockShaderUniforms = [](ShaderProgram& shader, const bool transparent) {
			shader.createUniform("textureSampler");
			shader.createUniform("modelMatrix");
			shader.createUniform("viewMatrix");
			shader.createUniform("projectionMatrix");
			if (transparent) {
				shader.createUniform("opaqueColorTexSampler");
			}
		};

		for (auto layer : render_layer::layers) {
			const size_t layerIndex = render_layer::getIndex(layer);

			// Block shader setup
			createBlockShaderUniforms(blockShaders[layerIndex], layer == render_layer::Transparent);

			// Block VAO and VBO setup
			blockVAOs[layerIndex].bind(); // bind the block VAO
			blockVBOs[layerIndex].setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
			blockVBOs[layerIndex].bind();
			blockVAOs[layerIndex].setVertexFormat(BlockQuad::format);
		}

		if (useFallbackTransparency) // Transparency fallback block revealage shader setup
			createBlockShaderUniforms(transparentsFallbackRevealageShader, true);

		blockSelectionShader.createUniform("mvpMatrix");
		blockSelectionShader.createUniform("color");
		blockSelectionVAO.bind();
		blockSelectionVBO.setData(nullptr, 0, VertexBuffer::DrawHint::STREAM);
		blockSelectionVBO.bind();
		blockSelectionVAO.setVertexFormat(VertexFormat({ { "position", VertexAttribute::Type::FLOAT, 3 } }));

		/*auto testImg = loadImage("scale_test.png");
		std::visit([](const auto img) {
			img.scaleNearest(0.5).writeImage("scale_test_nearest_half.png");
			img.scaleNearest(2).writeImage("scale_test_nearest_2.png");
			img.scaleLinear(0.5).writeImage("scale_test_bilinear_half.png");
			img.scaleLinear(2).writeImage("scale_test_bilinear_2.png");
		}, testImg);*/
		/*auto testImg = loadImage("colored.png");
		std::visit([](const auto img) {
			img.whiten().writeImage("whitened.png");
		}, testImg);*/

		/*Texture::GetDataResult<TextureFormat::RGBA, TextureDataType::UINT8> blockTexturesData = blockTextures.getData<TextureFormat::RGBA, TextureDataType::UINT8>();
		ImageRGBA blockTexturesDataImg { blockTexturesData.data.get(), blockTexturesData.width, blockTexturesData.height };
		blockTexturesDataImg.writeImage("test_texture_get.png");*/
	}

	inline static std::vector<const Chunk*> renderableChunks {};
	void WorldRenderer::render(const float partialTicks, const PlayState& gameState, const World* world, const Camera* camera) {
		Renderer::setClearColor(0x00000000_c);
		Renderer::clear(Renderer::ClearBit::COLOR | Renderer::ClearBit::DEPTH | Renderer::ClearBit::STENCIL);

		// create a vector of pointers to all renderable chunks
		const FrustumF viewFrustum(renderer->getProjectionMatrix() * camera->getViewMatrix(partialTicks), false);
		for (const auto& [chunkCoord, chunk] : world->getLoadedChunks()) {
			if (viewFrustum.contains(chunk.getBoundingBox())) { // frustum culling
				renderableChunks.push_back(&chunk);
				if (const RenderChunk* renderChunk = chunk.getRawRenderChunk(); renderChunk) {
					renderChunk->preRender(); // keep chunk meshes synchronized
				}
			}
		}

		// render the world
		renderChunks(partialTicks, gameState, world, camera);

		renderableChunks.clear();


		// render worldFBO to a fullscreen quad
		worldFBOColorAttachment.bind();
		worldFBOShader.bind();
		worldFBOShader.setUniform("textureSampler", 0);
		//glEnable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		worldFrameBufferVAO.draw(DrawMode::TRIANGLE_STRIP, 0, 4);
		//glDisable(GL_BLEND);
		//glDisable(GL_CULL_FACE);
	}

	void WorldRenderer::resize(const size_t width, const size_t height) {
		// resize framebuffer attachments
		worldFBO.bind(FrameBuffer::Target::DRAW_FRAMEBUFFER);
		worldFBOColorAttachment.setData(nullptr, width, height, TextureInternalFormat::U8x4, TextureFormat::RGBA, TextureDataType::UINT8);
		worldFBODepthStencilAttachment = RenderBuffer(TextureInternalFormat::DEPTH_U24_STENCIL_U8, width, height);
		//worldFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_0, &worldFBOColorAttachment);
		worldFBO.attachRenderBuffer(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::DEPTH_STENCIL, &worldFBODepthStencilAttachment);
		
		// setup transparency framebuffer
		transparentsFBO.bind(FrameBuffer::Target::DRAW_FRAMEBUFFER);
		transparentsAccumTex.setData(nullptr, width, height, TextureInternalFormat::FLOAT16x4, TextureFormat::RGBA, TextureDataType::FLOAT32);
		transparentsRevealageTex.setData(nullptr, width, height, TextureInternalFormat::U8x1, TextureFormat::RED, TextureDataType::UINT8);
		//transparentsFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_0, &transparentsAccumTex);
		//transparentsFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_1, &transparentsRevealageTex);
		//transparentsFBO.attachTexture(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::COLOR_2, &worldFBOColorAttachment);
		transparentsFBO.attachRenderBuffer(FrameBuffer::Target::DRAW_FRAMEBUFFER, FrameBuffer::Attachment::DEPTH_STENCIL, &worldFBODepthStencilAttachment);

		FrameBuffer::unbind(FrameBuffer::Target::DRAW_FRAMEBUFFER);
	}

	void WorldRenderer::renderChunks(const float partialTicks, const PlayState& gameState, const World* world, const Camera* camera) {
		const auto preSolidLayerRender = []() {
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		};
		const auto postSolidLayerRender = []() {
			glDisable(GL_CULL_FACE);
		};
		const auto preTransparentLayerRender = []() {
			glEnable(GL_CULL_FACE);
			glDepthMask(GL_FALSE);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
		};
		const auto postTransparentLayerRender = []() {
			glDepthMask(GL_TRUE);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
		};
		
		// clear the buffers
		for (auto layer : render_layer::layers) {
			const size_t layerIndex = render_layer::getIndex(layer);
			blockQuads[layerIndex].clear();
		}

		const glm::mat4 worldMatrix = glm::mat4(1.0f);//glm::translate(worldMatrix, glm::vec3(-0.5f, -0.5f, -0.5f));
		const auto& viewMatrix = camera->getViewMatrix(partialTicks);
		const auto& projectionMatrix = renderer->getProjectionMatrix();

		const auto mvpMatrix = projectionMatrix * viewMatrix * worldMatrix;

		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		constexpr auto opaqueIndex = render_layer::getIndex(render_layer::Opaque);
		constexpr auto cutoutIndex = render_layer::getIndex(render_layer::Cutout);
		constexpr auto transparentIndex = render_layer::getIndex(render_layer::Transparent);

		Renderer::setActiveTextureUnit(0);
		ResourceManager::instance().getBlockTextures().bindTexture();
		

		Renderer::setActiveTextureUnit(1);
		Texture::bind(worldFBOColorAttachment); // bind the main fbo color attachment texture

		worldFBO.bind(FrameBufferTarget::DRAW_FRAMEBUFFER);
		//Renderer::setClearColor(0x81EBE7_c); // blue
		Renderer::setClearColor(0xF4BCEE_c); // pink 1
		//Renderer::setClearColor(0xFF88B5_c); // pink 2
		Renderer::clear(Renderer::ClearBit::COLOR | Renderer::ClearBit::DEPTH | Renderer::ClearBit::STENCIL);

		preSolidLayerRender();

		blockShaders[opaqueIndex].bind();
		blockShaders[opaqueIndex].setUniform("textureSampler", 0);
		blockShaders[opaqueIndex].setUniform("viewMatrix", viewMatrix);
		blockShaders[opaqueIndex].setUniform("projectionMatrix", projectionMatrix);

		blockShaders[cutoutIndex].bind();
		blockShaders[cutoutIndex].setUniform("textureSampler", 0);
		blockShaders[cutoutIndex].setUniform("viewMatrix", viewMatrix);
		blockShaders[cutoutIndex].setUniform("projectionMatrix", projectionMatrix);

		// render chunks
		for (const auto chunk : renderableChunks) {
			const RenderChunk* renderChunk = chunk->getRawRenderChunk();
			if (!renderChunk) continue;

			const auto chunkMatrix = glm::translate(worldMatrix, glm::vec3(chunk->getBlockPos()));

			if (renderChunk->shouldDrawLayer(render_layer::Opaque)) {
				blockShaders[opaqueIndex].bind();
				blockShaders[opaqueIndex].setUniform("modelMatrix", chunkMatrix);
				renderChunk->drawLayer(render_layer::Opaque);
			}
			if (renderChunk->shouldDrawLayer(render_layer::Cutout)) {
				glDepthFunc(GL_LEQUAL);
				blockShaders[cutoutIndex].bind();
				blockShaders[cutoutIndex].setUniform("modelMatrix", chunkMatrix);
				renderChunk->drawLayer(render_layer::Cutout);
				glDepthFunc(GL_LESS);
			}

			// TODO: render entities
		}
		// render from vbo members
		if (!blockQuads[opaqueIndex].empty()) {
			blockShaders[opaqueIndex].bind();
			blockShaders[opaqueIndex].setUniform("modelMatrix", worldMatrix);
			blockVAOs[opaqueIndex].bind();
			blockVBOs[opaqueIndex].setData(blockQuads[opaqueIndex].data(), sizeof(BlockQuad) * blockQuads[opaqueIndex].size());
			blockVAOs[opaqueIndex].draw(DrawMode::POINTS, 0, blockQuads[opaqueIndex].size());
		}
		if (!blockQuads[cutoutIndex].empty()) {
			glDepthFunc(GL_LEQUAL);
			blockShaders[cutoutIndex].bind();
			blockShaders[cutoutIndex].setUniform("modelMatrix", worldMatrix);
			blockVAOs[cutoutIndex].bind();
			blockVBOs[cutoutIndex].setData(blockQuads[cutoutIndex].data(), sizeof(BlockQuad) * blockQuads[cutoutIndex].size());
			blockVAOs[cutoutIndex].draw(DrawMode::POINTS, 0, blockQuads[cutoutIndex].size());
			glDepthFunc(GL_LESS);
		}
		postSolidLayerRender();

		// render block selection outline
		const auto& rayCastResult = gameState.getPlayerRayCastResult();
		if (rayCastResult.hitBlock()) {
			const auto& blockState = world->getBlockState(rayCastResult.blockPos);
			const auto box = blockState.getBlock().getSelectionBox(*world, blockState, rayCastResult.blockPos) + rayCastResult.blockPos;
			renderBlockSelectionOutline(partialTicks, mvpMatrix, rayCastResult.face, box);
		}


		transparentsFBO.bind(FrameBufferTarget::DRAW_FRAMEBUFFER);
		Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_0, DrawBuffer::COLOR_ATTACHMENT_1, DrawBuffer::COLOR_ATTACHMENT_2 });
		transparentsFBO.clearColorBuffer(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), 0);
		transparentsFBO.clearColorBuffer(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), 1);

		preTransparentLayerRender();

		blockShaders[transparentIndex].bind();
		blockShaders[transparentIndex].setUniform("textureSampler", 0);
		blockShaders[transparentIndex].setUniform("opaqueColorTexSampler", 1);
		blockShaders[transparentIndex].setUniform("viewMatrix", viewMatrix);
		blockShaders[transparentIndex].setUniform("projectionMatrix", projectionMatrix);

		if (useFallbackTransparency) {
			transparentsFallbackRevealageShader.bind();
			transparentsFallbackRevealageShader.setUniform("textureSampler", 0);
			transparentsFallbackRevealageShader.setUniform("opaqueColorTexSampler", 1);
			transparentsFallbackRevealageShader.setUniform("viewMatrix", viewMatrix);
			transparentsFallbackRevealageShader.setUniform("projectionMatrix", projectionMatrix);
		}

		// render chunks
		for (const auto chunk : renderableChunks) {
			const RenderChunk* renderChunk = chunk->getRawRenderChunk();
			if (!renderChunk) continue;

			const bool drawTransparent = renderChunk->shouldDrawLayer(render_layer::Transparent);
			if (drawTransparent) {
				const auto chunkMatrix = glm::translate(worldMatrix, glm::vec3(chunk->getBlockPos()));

				if (useFallbackTransparency) {
					// render to accum texture
					Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_0 });
					glBlendFunc(GL_ONE, GL_ONE);
					blockShaders[transparentIndex].bind();
					blockShaders[transparentIndex].setUniform("modelMatrix", chunkMatrix);
					renderChunk->drawLayer(render_layer::Transparent);

					// render to revealage texture
					Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_1, DrawBuffer::COLOR_ATTACHMENT_2 });
					glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
					transparentsFallbackRevealageShader.bind();
					transparentsFallbackRevealageShader.setUniform("modelMatrix", chunkMatrix);
					renderChunk->drawLayer(render_layer::Transparent);
				} else {
					Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_0, DrawBuffer::COLOR_ATTACHMENT_1, DrawBuffer::COLOR_ATTACHMENT_2 });
					// TODO: support AMD extension
					glBlendFunciARB(0, GL_ONE, GL_ONE);
					glBlendFunciARB(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
					glBlendFunciARB(2, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
					blockShaders[transparentIndex].bind();
					blockShaders[transparentIndex].setUniform("modelMatrix", chunkMatrix);
					renderChunk->drawLayer(render_layer::Transparent);
				}

				// TODO: render entities
			}
		}

		// sort transparent quads
		//auto& transparentQuads = blockQuads[render_layer::getIndex(RenderLayer::Transparent)];
		//const auto& cameraPos = camera->getPosition();
		//std::sort(transparentQuads.begin(), transparentQuads.end(),
		//		  [&cameraPos](const BlockQuad& a, const BlockQuad& b) -> bool {
		//			  return glm::distance2(a.getCenter(), cameraPos) > glm::distance2(b.getCenter(), cameraPos);
		//		  });

		// render from vbo members
		if (!blockQuads[transparentIndex].empty()) {
			if (useFallbackTransparency) {
				// render to accum texture
				Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_0 });
				glBlendFunc(GL_ONE, GL_ONE);
				blockShaders[transparentIndex].bind();
				blockShaders[transparentIndex].setUniform("modelMatrix", worldMatrix);
				blockVAOs[transparentIndex].bind();
				blockVBOs[transparentIndex].setData(blockQuads[transparentIndex].data(), sizeof(BlockQuad)* blockQuads[transparentIndex].size());
				blockVAOs[transparentIndex].draw(DrawMode::POINTS, 0, blockQuads[transparentIndex].size());

				// render to revealage texture
				Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_1, DrawBuffer::COLOR_ATTACHMENT_2 });
				glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
				transparentsFallbackRevealageShader.bind();
				transparentsFallbackRevealageShader.setUniform("modelMatrix", worldMatrix);
				blockVAOs[transparentIndex].draw(DrawMode::POINTS, 0, blockQuads[transparentIndex].size());
			} else {
				Renderer::setActiveDrawBuffers({ DrawBuffer::COLOR_ATTACHMENT_0, DrawBuffer::COLOR_ATTACHMENT_1, DrawBuffer::COLOR_ATTACHMENT_2 });
				// TODO: support AMD extension
				glBlendFunciARB(0, GL_ONE, GL_ONE);
				glBlendFunciARB(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
				glBlendFunciARB(2, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
				blockShaders[transparentIndex].bind();
				blockShaders[transparentIndex].setUniform("modelMatrix", worldMatrix);
				blockVAOs[transparentIndex].bind();
				blockVBOs[transparentIndex].setData(blockQuads[transparentIndex].data(), sizeof(BlockQuad)* blockQuads[transparentIndex].size());
				blockVAOs[transparentIndex].draw(DrawMode::POINTS, 0, blockQuads[transparentIndex].size());
			}
		}
		postTransparentLayerRender();

		worldFBO.bind(FrameBufferTarget::DRAW_FRAMEBUFFER);

		// composite transparents FBO onto main world FBO
		Renderer::setActiveTextureUnit(0);
		Texture::bind(transparentsAccumTex);
		Renderer::setActiveTextureUnit(1);
		Texture::bind(transparentsRevealageTex);
		Renderer::setActiveTextureUnit(2);
		Texture::bind(worldFBOColorAttachment);
		transparentsFBOShader.bind();
		transparentsFBOShader.setUniform("accumTexSampler", 0);
		transparentsFBOShader.setUniform("revealageTexSampler", 1);
		transparentsFBOShader.setUniform("colorTexSampler", 2);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_ONE);
		worldFrameBufferVAO.draw(DrawMode::TRIANGLE_STRIP, 0, 4);
		glDisable(GL_BLEND);

		FrameBuffer::unbind(FrameBufferTarget::DRAW_FRAMEBUFFER);

		Renderer::setActiveTextureUnit(0);
	}

	void WorldRenderer::renderBlockSelectionOutline(const float partialTicks, const glm::mat4& mvpMatrix, const Direction selectionFace, AxisAlignedBox<float> box) {
		if (!box) return;
		if (selectionFace == Direction::UNDEFINED) return;

		box = box.getExpanded(0.0025f);

		glm::vec3 faceP0 = box.min, faceP1 = box.max;
		glm::vec3 faceP2 = box.min, faceP3 = box.max;
		switch (selectionFace) {
		case Direction::DOWN:
			faceP1.y = faceP0.y;
			faceP3.y = faceP2.y;
			std::swap(faceP2.x, faceP3.x);
			break;
		case Direction::UP:
			faceP0.y = faceP1.y;
			faceP2.y = faceP3.y;
			std::swap(faceP2.x, faceP3.x);
			break;
		case Direction::NORTH:
			faceP1.z = faceP0.z;
			faceP3.z = faceP2.z;
			std::swap(faceP2.x, faceP3.x);
			break;
		case Direction::SOUTH:
			faceP0.z = faceP1.z;
			faceP2.z = faceP3.z;
			std::swap(faceP2.x, faceP3.x);
			break;
		case Direction::WEST:
			faceP1.x = faceP0.x;
			faceP3.x = faceP2.x;
			std::swap(faceP2.z, faceP3.z);
			break;
		case Direction::EAST:
			faceP0.x = faceP1.x;
			faceP2.x = faceP3.x;
			std::swap(faceP2.z, faceP3.z);
			break;
		case Direction::UNDEFINED:
			break;
		}

		const glm::vec3 lines[] {
			// face cross
			faceP0, faceP1,
			faceP2, faceP3,
			// bottom edges
			{ box.min.x, box.min.y, box.min.z }, { box.max.x, box.min.y, box.min.z },
			{ box.min.x, box.min.y, box.min.z }, { box.min.x, box.min.y, box.max.z },
			{ box.min.x, box.min.y, box.max.z }, { box.max.x, box.min.y, box.max.z },
			{ box.max.x, box.min.y, box.min.z }, { box.max.x, box.min.y, box.max.z },
			// top edges
			{ box.min.x, box.max.y, box.min.z }, { box.max.x, box.max.y, box.min.z },
			{ box.min.x, box.max.y, box.min.z }, { box.min.x, box.max.y, box.max.z },
			{ box.min.x, box.max.y, box.max.z }, { box.max.x, box.max.y, box.max.z },
			{ box.max.x, box.max.y, box.min.z }, { box.max.x, box.max.y, box.max.z },
			// side edges
			{ box.min.x, box.min.y, box.min.z }, { box.min.x, box.max.y, box.min.z },
			{ box.max.x, box.min.y, box.min.z }, { box.max.x, box.max.y, box.min.z },
			{ box.max.x, box.min.y, box.max.z }, { box.max.x, box.max.y, box.max.z },
			{ box.min.x, box.min.y, box.max.z }, { box.min.x, box.max.y, box.max.z },
		};

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);

		blockSelectionShader.bind();
		blockSelectionShader.setUniform("mvpMatrix", mvpMatrix);
		blockSelectionShader.setUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));

		blockSelectionVAO.bind(); // bind the VAO
		blockSelectionVBO.setData(&lines, sizeof(lines)); // upload the vertex data to the VBO
		blockSelectionVAO.draw(DrawMode::LINES, 0, (sizeof(lines) / sizeof(glm::vec3))); // draw

		glDepthFunc(GL_LESS);
	}

	ImageRGBA WorldRenderer::captureFrame() const {
		const auto frameData = worldFBOColorAttachment.getData<TextureFormat::RGBA, TextureDataType::UINT8>();
		return ImageRGBA(frameData.data.get(), frameData.width, frameData.height).flipY();
	}

}