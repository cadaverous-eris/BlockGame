#pragma once

#include <vector>

#include "render/Renderer.h"
#include "render/VertexArray.h"
#include "render/VertexBuffer.h"
#include "render/ShaderProgram.h"
#include "render/texture/Texture.h"
#include "UIVert.h"

namespace eng {

	class PlayState;

	class HudRenderer {
	public:

	private:
		Renderer* renderer;

		Texture hudTextures = Texture::loadTexture("hud.png"); // TODO: texture manager?

	public:
		HudRenderer(Renderer* const);

		HudRenderer(const HudRenderer&) = delete;
		HudRenderer& operator =(const HudRenderer&) = delete;
		HudRenderer(HudRenderer&&) = default;
		HudRenderer& operator =(HudRenderer&&) = default;

		void render(const float partialTicks, const PlayState& gameState);

	private:

	};

}