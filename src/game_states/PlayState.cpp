#include "PlayState.h"

#include <sstream>
#include <string>

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include "Game.h"
#include "render/Renderer.h"
#include "model/block/BlockQuad.h"
#include "util/math/math.h"
#include "util/resources/Image.h"
#include "util/resources/ResourceManager.h"
#include "gui/inventory/GuiPlayerInventory.h"
#include "gui/menu/GuiPauseMenu.h"
#include "util/nbt/NBT.h" // TODO: remove
#include "util/IOUtils.h" // TODO: remove

#include <iostream> // TODO: remove

namespace eng {

	PlayState::PlayState(Game* const game) :
			GameState(game),
			world(), // TODO: take arguments
			worldRenderer(&game->renderer),
			hudRenderer(&game->renderer),
			pauseKeyHandler(*input::keybinds::PAUSE, [this](const input::KeyInput&) { this->openPauseMenu(); }),
			inventoryKeyHandler(*input::keybinds::INVENTORY, [this](const input::KeyInput&) { this->openInventory(); }),
			printCameraKeyHandler(*input::keybinds::PRINT_CAMERA, [this](const input::KeyInput&) { this->camera.log(); }),
			takeScreenshotKeyHandler(*input::keybinds::TAKE_SCREENSHOT, [this](const input::KeyInput&) { this->takeScreenshot(); }) {
		world.setPlayer(&camera);
		world.setWorldRenderer(&worldRenderer);

		//bool parsedSNBT = false;
		//nbt::NBT nbt {};
		//const std::string inputSNBT =
		//	R"({ "awdd": 13b, "aw": 16d, a: 198f,'BA':[B; 32, 16, 127, -4, 255, 3], "B": -1.34, rstsr: [ "awdwd", 'adwa', "awawd" ], empty_list: [], ll: [[1], [0.5f, -inff, nanf, 0.002f]], elll: [[[[[]]]]],"c": { "awd": 23, "adw": 35L, "ls": [ { str: "adawafaf" }, { s: -12s, i: 2, }, { ia: [I; 23, -344, 232, 45, ], f: 0.04 } ] }, })";
		//try {
		//	nbt = nbt::parseSNBT(inputSNBT);
		//	parsedSNBT = true;
		//} catch(const nbt::ParseError& parseError) {
		//	std::cerr << '\n' << "Error parsing SNBT:" << '\n';
		//	std::cerr << parseError.what() << '\n' << '\n';
		//}
		//if (parsedSNBT) {
		//	nbt.asCompound().at<nbt::TagByteArray>("BA").emplace_back(0xFF);
		//	std::cout << '\n' << nbt.toSNBT(2) << '\n' << '\n';
		//}

		//std::cout << '\n';
		//const std::string nbtFilePath = "bigtest.nbt";//"test.nbt";
		//const std::string nbtFileName = [&]() -> std::string {
		//	std::string_view fileName = std::string_view(nbtFilePath);
		//	const auto lastDotIndex = fileName.find_last_of('.');
		//	const auto lastSlashIndex = fileName.find_last_of('/', lastDotIndex);
		//	if (lastDotIndex != std::string_view::npos)
		//		fileName = fileName.substr(0, lastDotIndex);
		//	if ((lastSlashIndex != std::string_view::npos) && ((lastSlashIndex + 1) < fileName.size()))
		//		fileName = fileName.substr(lastSlashIndex + 1);
		//	return std::string(fileName);
		//}();
		//const bool isFileCompressed = true;
		//const auto fileData = isFileCompressed ?
		//		readCompressedBinaryFile("../nbt_testing/" + nbtFilePath) :
		//		readBinaryFile("../nbt_testing/" + nbtFilePath);
		//std::cout << "Uncompressed size of '" << nbtFilePath << "': " << fileData.size() << '\n';
		//bool parsedNBT = false;
		//nbt::NBT nbt {};
		//try {
		//	nbt = nbt::parseNBT(std::span<const unsigned char>(fileData.data(), fileData.size()));
		//	parsedNBT = true;
		//} catch(const nbt::ParseError& parseError) {
		//	std::cerr << '\n' << "Error parsing NBT:" << '\n';
		//	std::cerr << parseError.what() << '\n' << '\n';
		//}
		//const std::string snbtFilePath = "../nbt_testing/" + (nbtFileName.empty() ? std::string("test") : nbtFileName) + ".snbt";
		//const std::string nbtCopyFilePath = "../nbt_testing/" + (nbtFileName.empty() ? std::string("test") : nbtFileName) + "_reserialized.nbt";
		//const std::string snbtCopyFilePath = "../nbt_testing/" + (nbtFileName.empty() ? std::string("test") : nbtFileName) + "_reserialized.snbt";
		//if (parsedNBT) {
		//	writeTextFile(snbtFilePath, nbt.toSNBT(2));
		//	std::cout << '\n' << "Successfully parsed " << nbtFilePath << ", SNBT written to " << snbtFilePath << '\n' << '\n';
		//	//std::cout << '\n' << nbt.toSNBT(2) << '\n' << '\n';
		//	const auto reserializedBytes = [&]() {
		//		const auto it = nbt.asCompound().cbegin();
		//		return it->second.toBinary(it->first);
		//	}();
		//	if (isFileCompressed)
		//		writeCompressedBinaryFile(nbtCopyFilePath, std::span<const unsigned char>(reserializedBytes.begin(), reserializedBytes.end()));
		//	else
		//		writeBinaryFile(nbtCopyFilePath, std::span<const unsigned char>(reserializedBytes.begin(), reserializedBytes.end()));
		//	const auto reserializedNBT = nbt::parseNBT(std::span<const unsigned char>(reserializedBytes.begin(), reserializedBytes.end()));
		//	writeTextFile(snbtCopyFilePath, reserializedNBT.toSNBT(2));
		//}

		//std::vector<Color> testByteVec { {}, color::aquamarine, color::black, };
		//std::vector<uint64_t> testByteVec { 0x00, 0x01, 0xFF, 0x7D, 0x7F, };
		//nbt::NBT testNbt = testByteVec;
		//std::cout << '\n' << testNbt.getTagType() << '\n' << /*testNbt.is<std::vector<Color>>() <<*/ '\n' << '\n';

		// Vertex Array setup
		//vao.bind(); // bind the VAO
		//vbo.setData(vertices, sizeof(vertices));
		//ebo.setData(indices, sizeof(indices) / sizeof(uint32_t));
		/*{ // Cube model manipulation
			const glm::vec3 cubeModelOrigin{ 0.5f, 0.5f, 0.5f };
			const glm::mat4 quadTransformationMatrix = glm::translate(glm::rotate(glm::translate(cubeModelOrigin), glm::radians(45.0f), { 0, 0, -1 }), -cubeModelOrigin);
			for (BlockQuad& quad : quads) {
				quad.transform(quadTransformationMatrix);
			}
		}*/
		//vbo.setData(quads, sizeof(quads));
		//vbo.setData(std::span<BlockQuad>(std::begin(quads), std::end(quads)));
		// make sure the vbo is bound when setting up vertex attributes
		//vbo.bind();
		/*VertexFormat vertexFormat {
			{ "position", VertexAttribute::Type::FLOAT, 3 },
			{ "normal", VertexAttribute::Type::FLOAT, 3 },
			{ "texCoord", VertexAttribute::Type::FLOAT, 2 },
		};*/

		// print the gl version for debugging
		std::cout << glGetString(GL_VERSION) << '\n';
		std::cout << glGetString(GL_RENDERER) << '\n';

		// Camera setup
		const auto terrainHeight = world.getTerrainHeight(0, 0);
		camera.setPosition(0.5f, terrainHeight + 2.8f, 0.5f);
		//camera.lookAt(0.0f, 0.0f, 0.0f);

		game->inputManager.setCursorMode(input::CursorMode::DISABLED);
		game->inputManager.input();

		if (!game->window.hasFocus()) {
			openPauseMenu();
			game->window.requestAttention();
		}
	}

	/*
	PlayState::~PlayState() {
		// TODO: implement?
	}
	*/

	void PlayState::input() {
		const bool enableCamera = (game->inputManager.getCursorMode() == input::CursorMode::DISABLED);

		if (!game->inputManager.cursorModeChanged()) {
			const auto& cursorPos = game->inputManager.getCursorPos();
			const auto& prevCursorPos = game->inputManager.getPrevCursorPos();
			if ((cursorPos != prevCursorPos)) {
				// rotate the camera
				glm::dvec2 cameraRotation = glm::vec2(cursorPos - prevCursorPos) * cameraSensitivity;
				if (enableCamera) camera.rotate(cameraRotation.x, cameraRotation.y);
			}
		}

		// switch the selected block with the scroll wheel
		const int scrollY = static_cast<int>(game->inputManager.getMouseScroll().y);
		if (enableCamera && scrollY != 0) {
			const int prevSelectedBlockId = selectedBlock->getId();
			const int selectedBlockId = 1 + pmod(prevSelectedBlockId + scrollY - 1, static_cast<int>(block_registry.size() - 1));
			selectedBlock = &block_registry[selectedBlockId];
		}
	}

	void PlayState::update() {
		if (game->isPaused())
			return; // TODO: continue loading/generating chunks while game is paused

		camera.update();

		glm::vec3 cameraMovement(0.0f, 0.0f, 0.0f);
		if (input::keybinds::MOVE_FORWARD->isPressed()) cameraMovement.z += 1.0f;
		if (input::keybinds::MOVE_BACKWARD->isPressed()) cameraMovement.z -= 1.0f;
		if (input::keybinds::MOVE_LEFT->isPressed()) cameraMovement.x -= 1.0f;
		if (input::keybinds::MOVE_RIGHT->isPressed()) cameraMovement.x += 1.0f;
		if (cameraMovement.x != 0 && cameraMovement.z != 0) {
			cameraMovement = glm::normalize(cameraMovement);
		}
		if (input::keybinds::SNEAK->isPressed()) cameraMovement.y -= 1.0f;
		if (input::keybinds::JUMP->isPressed()) cameraMovement.y += 1.0f;
		cameraMovement *= cameraSpeed;
		if (game->inputManager.getCursorMode() == input::CursorMode::DISABLED) {
			camera.move(cameraMovement);
		}

		constexpr float range = 20.0f;
		const auto ray = RayF::fromDir(camera.getPosition(), camera.getLookVec(), range);
		playerRayCastResult = world.rayCast(ray, RayCastMask::Blocks | RayCastMask::Entities);

		// decrement placement and destruction delays
		if (blockBreakDelay > 0) blockBreakDelay--;
		if (blockPlaceDelay > 0) blockPlaceDelay--;

		// decrement fluid placement and destruction delays // TODO: remove
		if (fluidBreakDelay > 0) fluidBreakDelay--;
		if (fluidPlaceDelay > 0) fluidPlaceDelay--;

		if (game->inputManager.getCursorMode() == input::CursorMode::DISABLED) {
			// try block destruction
			if (input::keybinds::BREAK_BLOCK->isPressed() && playerRayCastResult.hitBlock() && (blockBreakDelay <= 0)) {
				world.setBlockState(playerRayCastResult.blockPos, blocks::air, true, true, true, MeshingPriority::PlayerInteract);
				blockBreakDelay = 4;
			}

			// try block placement
			if (input::keybinds::PLACE_BLOCK->isPressed() && playerRayCastResult.hitBlock() &&
					(playerRayCastResult.face != Direction::UNDEFINED) && (blockPlaceDelay <= 0)) {
				const auto hitBlockState = world.getBlockState(playerRayCastResult.blockPos);
				if (hitBlockState.getBlock().isReplaceable(hitBlockState)) {
					if (selectedBlock->canPlaceBlock(world, playerRayCastResult.blockPos, playerRayCastResult.face)) {
						const auto placementState = selectedBlock->getStateForPlacement(world, playerRayCastResult.blockPos, playerRayCastResult); // TODO: add a player argment
						world.setBlockState(playerRayCastResult.blockPos, placementState, true, true, true, MeshingPriority::PlayerInteract);
						blockPlaceDelay = 4;
					}
				} else {
					const auto placementPos = offsetVector(playerRayCastResult.blockPos, playerRayCastResult.face);
					const auto existingBlockState = world.getBlockState(placementPos);
					if (existingBlockState.getBlock().isReplaceable(existingBlockState) &&
							selectedBlock->canPlaceBlock(world, placementPos, playerRayCastResult.face)) {
						const auto placementState = selectedBlock->getStateForPlacement(world, placementPos, playerRayCastResult); // TODO: add a player argment
						world.setBlockState(placementPos, placementState, true, true, true, MeshingPriority::PlayerInteract);
						blockPlaceDelay = 4;
					}
				}
			}

			const bool tryPlaceFluid = input::keybinds::DEBUG_PLACE_FLUID->isPressed() && (fluidPlaceDelay <= 0);
			const bool tryBreakFluid = input::keybinds::DEBUG_BREAK_FLUID->isPressed() && (fluidBreakDelay <= 0);
			if (tryPlaceFluid || tryBreakFluid) {
				const RayCastResultF fluidRayCast = world.rayCast(ray, RayCastMask::Blocks | RayCastMask::Fluids | RayCastMask::Entities);

				if (tryBreakFluid && fluidRayCast.hitFluid()) {
					world.setFluidState(fluidRayCast.blockPos, fluids::empty_fluidstate, true, true, false, MeshingPriority::PlayerInteract);
					fluidBreakDelay = 4;
				}
				if (tryPlaceFluid && (fluidRayCast.face != Direction::UNDEFINED)) {
					const FluidState placementState = FiniteFluidState(fluids::water, 23);
					if (fluidRayCast.hitBlock()) {
						const auto hitBlockState = world.getBlockState(fluidRayCast.blockPos);
						const auto hitFluidState = world.getFluidState(fluidRayCast.blockPos);
						if (!hitBlockState.getBlock().isFullCube(hitBlockState) && hitFluidState.getFluid().isNullFluid(hitFluidState)) {
							world.setFluidState(fluidRayCast.blockPos, placementState, true, false, true, MeshingPriority::PlayerInteract);
							fluidPlaceDelay = 4;
						} else {
							const auto placementPos = offsetVector(fluidRayCast.blockPos, fluidRayCast.face);
							const auto existingBlockState = world.getBlockState(placementPos);
							const auto existingFluidState = world.getFluidState(placementPos);
							if (!existingBlockState.getBlock().isFullCube(existingBlockState) &&
									existingFluidState.getFluid().isNullFluid(existingFluidState)) {
								world.setFluidState(placementPos, placementState, true, false, true, MeshingPriority::PlayerInteract);
								fluidPlaceDelay = 4;
							}
						}
					} else if (fluidRayCast.hitFluid()) {
						// TODO: place fluid against the hit fluid

					}
				}
			}
		}

		world.update();

		ResourceManager::instance().updateBlockAnimations();

	}

	void PlayState::render(float partialTicks) {
		worldRenderer.render(partialTicks, *this, &world, &camera);

		hudRenderer.render(partialTicks, *this);

		/*
		// bind the shaderProgram
		shaderProgram.bind();
		// for model matrix: do translation, then scaling, then rotation
		float interpolatedRotation = prevRotation + ((rotation - prevRotation) * partialTicks);
		modelMatrix = glm::identity<glm::mat4>();
		// TODO: translation
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, -0.5f, -0.5f));
		// TODO: scaling
		modelMatrix = glm::rotate(modelMatrix, glm::radians(interpolatedRotation), glm::vec3(0.4f, 1.0f, 0.7f));
		// use texture unit 0
		shaderProgram.setUniform("textureSampler", 0);
		// upload the matrices to the gpu
		shaderProgram.setUniform("modelMatrix", modelMatrix);
		shaderProgram.setUniform("viewMatrix", camera.getViewMatrix(partialTicks));
		shaderProgram.setUniform("projectionMatrix", game->renderer->getProjectionMatrix());
		// draw
		//vao.draw(Renderer::DrawMode::TRIANGLES, ebo);
		vao.draw(Renderer::DrawMode::POINTS, 0, 6);
		*/
	}

	void PlayState::rendererResize(const size_t width, const size_t height) {
		worldRenderer.resize(width, height);

		// TODO: implement
	}

	void PlayState::onGuiOpened() {
		if (game->inputManager.getCursorMode() == input::CursorMode::DISABLED) {
			game->inputManager.setCursorMode(input::CursorMode::NORMAL); // enable the mouse cursor
			// set the cursor position to the center of the window
			game->inputManager.setCursorPos(static_cast<glm::dvec2>(game->window.getSize()) / 2.0);
		}
	}
	void PlayState::onGuiClosed() {
		game->inputManager.setCursorMode(input::CursorMode::DISABLED); // disable the mouse cursor
	}

	void PlayState::openInventory() const {
		if (!game->getActiveGui())
			game->openGui(std::make_unique<GuiPlayerInventory>());
	}
	void PlayState::openPauseMenu() const {
		if (!game->getActiveGui())
			game->openGui(std::make_unique<GuiPauseMenu>());
	}


	void PlayState::takeScreenshot() const {
		game->renderer.takeScreenshot();
	}

}