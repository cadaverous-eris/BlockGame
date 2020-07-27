#include "FluidFinite.h"

#include <limits>
#include <cmath>

#include "world/World.h"
#include "fluid/FluidRegistry.h"
#include "block/BlockRegistry.h"
#include "util/resources/ResourceManager.h"
#include "util/resources/AtlasSprite.h"
#include "util/math/math.h"
#include "model/block/BlockModel.h"

#include <iostream>

namespace eng {

	inline static float getFillAmount(FluidStateRef fluidState, BlockStateRef blockState) noexcept {
		const FiniteFluidState state = fluidState;
		const auto capacity = (blockState.getFluidCapactity() * (FiniteFluidState::base_capacity + 1.0f));
		if (capacity <= 0) return 0;
		return vmin((state.getAmount() + 1.0f) / (capacity), 1.0f);
	}
	inline static float getFillAmount(FluidRef fluid, FluidStateRef fluidState, BlockStateRef blockState) {
		return (fluidState.getFluid() == fluid) ? getFillAmount(fluidState, blockState) : 0;
	}
	inline static float getFillAmount(FluidRef fluid, const World& world, const glm::ivec3& blockPos) {
		return getFillAmount(fluid, world.getFluidState(blockPos), world.getBlockState(blockPos));
	}


	AxisAlignedBoxF FluidFinite::getBoundingBox(const World& world, FluidStateRef fluidState, const glm::ivec3& blockPos) const {
		const auto height = getFillAmount(fluidState, world.getBlockState(blockPos));
		return { { 0.0f, 0.0f, 0.0f }, { 1.0f, height, 1.0f } };
	}

	uint8_t FluidFinite::getAmount(FluidStateRef fluidState) const {
		return FiniteFluidState(fluidState).getAmount() + 1;
	}

	
	static inline int getFluidCapacity(BlockStateRef blockState) {
		return static_cast<int>(blockState.getFluidCapactity() * (FiniteFluidState::base_capacity + 1.0f));
	}
	static int getFluidCapacity(FluidRef fluid, FluidStateRef fluidState, BlockStateRef blockState, const Direction flowFace) {
		if ((fluidState.isEmpty() || (fluidState.getFluid() == fluid)) && blockState.canFluidFlowThroughFace(flowFace, fluid)) {
			return getFluidCapacity(blockState);
		}
		return 0;
	}
	static inline int getFluidAmount(FluidRef fluid, FluidStateRef fluidState) {
		return (fluidState.getFluid() == fluid) ? fluid.getAmount(fluidState) : 0;
	}
	struct FluidCell {
		int capacity;
		int amount;
		FluidCell(const int cap, const int amt) noexcept : capacity(cap), amount(amt) {}
		FluidCell(FluidRef fluid, FluidStateRef fluidState, BlockStateRef blockState, const Direction flowFace) :
			FluidCell(getFluidCapacity(fluid, fluidState, blockState, flowFace), getFluidAmount(fluid, fluidState)) {}
		inline int getMaxFlow() const noexcept { return capacity - amount; }
		inline FluidState toFluidState(FluidRef fluid) const {
			return (amount > 0) ? FluidState(FiniteFluidState(fluid, amount - 1, FluidFlow::Still)) : fluids::empty_fluidstate;
		}
	};

	void FluidFinite::onFluidUpdate(World& world, FluidStateRef fluidState, const glm::ivec3& blockPos, const Direction srcDirection) const {
		const FiniteFluidState state = fluidState;
		BlockStateRef blockState = world.getBlockState(blockPos);
		
		const int amount = getAmount(fluidState);

		FluidCell cell(getFluidCapacity(blockState), amount);

		// TODO: pressure
		
		const auto dPos = direction::offsetVector(blockPos, Direction::DOWN);
		FluidStateRef dFluidState = world.getFluidState(dPos);
		BlockStateRef dBlockState = world.getBlockState(dPos);
		FluidCell dCell(*this, dFluidState, dBlockState, Direction::UP);
		if (cell.amount > 0) { // flow down
			const auto flowD = vmin(cell.amount, dCell.capacity - dCell.amount);
			if (flowD > 0) {
				dCell.amount += flowD;
				world.setFluidState(dPos, dCell.toFluidState(*this), true, false, true);
				cell.amount -= flowD;
			}
		}
		if (cell.amount > 0) { // flow sideways
			const auto nPos = direction::offsetVector(blockPos, Direction::NORTH);
			const auto sPos = direction::offsetVector(blockPos, Direction::SOUTH);
			const auto wPos = direction::offsetVector(blockPos, Direction::WEST);
			const auto ePos = direction::offsetVector(blockPos, Direction::EAST);
			FluidStateRef nFluidState = world.getFluidState(nPos);
			BlockStateRef nBlockState = world.getBlockState(nPos);
			FluidStateRef sFluidState = world.getFluidState(sPos);
			BlockStateRef sBlockState = world.getBlockState(sPos);
			FluidStateRef wFluidState = world.getFluidState(wPos);
			BlockStateRef wBlockState = world.getBlockState(wPos);
			FluidStateRef eFluidState = world.getFluidState(ePos);
			BlockStateRef eBlockState = world.getBlockState(ePos);
			const FluidCell nCellOg(*this, nFluidState, nBlockState, Direction::SOUTH);
			const FluidCell sCellOg(*this, sFluidState, sBlockState, Direction::NORTH);
			const FluidCell wCellOg(*this, wFluidState, wBlockState, Direction::EAST);
			const FluidCell eCellOg(*this, eFluidState, eBlockState, Direction::WEST);

			const bool overFilled = (cell.amount > cell.capacity);

			bool doFlowN = (nCellOg.capacity > 0);// && ((nCellOg.amount < cell.amount) || overFilled) && (nCellOg.getMaxFlow() > 0);
			bool doFlowS = (sCellOg.capacity > 0);// && ((sCellOg.amount < cell.amount) || overFilled) && (sCellOg.getMaxFlow() > 0);
			bool doFlowW = (wCellOg.capacity > 0);// && ((wCellOg.amount < cell.amount) || overFilled) && (wCellOg.getMaxFlow() > 0);
			bool doFlowE = (eCellOg.capacity > 0);// && ((eCellOg.amount < cell.amount) || overFilled) && (eCellOg.getMaxFlow() > 0);
			int numFlowSides = static_cast<int>(doFlowN) + static_cast<int>(doFlowS) + static_cast<int>(doFlowW) + static_cast<int>(doFlowE);

			if (numFlowSides > 0) {
				FluidCell nCell = nCellOg;
				FluidCell sCell = sCellOg;
				FluidCell wCell = wCellOg;
				FluidCell eCell = eCellOg;
				
				const auto ndPos = direction::offsetVector(nPos, Direction::DOWN);
				const auto sdPos = direction::offsetVector(sPos, Direction::DOWN);
				const auto wdPos = direction::offsetVector(wPos, Direction::DOWN);
				const auto edPos = direction::offsetVector(ePos, Direction::DOWN);
				FluidStateRef ndFluidState = world.getFluidState(ndPos);
				BlockStateRef ndBlockState = world.getBlockState(ndPos);
				FluidStateRef sdFluidState = world.getFluidState(sdPos);
				BlockStateRef sdBlockState = world.getBlockState(sdPos);
				FluidStateRef wdFluidState = world.getFluidState(wdPos);
				BlockStateRef wdBlockState = world.getBlockState(wdPos);
				FluidStateRef edFluidState = world.getFluidState(edPos);
				BlockStateRef edBlockState = world.getBlockState(edPos);
				const FluidCell ndCell(*this, ndFluidState, ndBlockState, Direction::UP);
				const FluidCell sdCell(*this, sdFluidState, sdBlockState, Direction::UP);
				const FluidCell wdCell(*this, wdFluidState, wdBlockState, Direction::UP);
				const FluidCell edCell(*this, edFluidState, edBlockState, Direction::UP);
				const bool canFlowND = ndCell.getMaxFlow() > 0;
				const bool canFlowSD = sdCell.getMaxFlow() > 0;
				const bool canFlowWD = wdCell.getMaxFlow() > 0;
				const bool canFlowED = edCell.getMaxFlow() > 0;
				const int numFlowDownSides = static_cast<int>(canFlowND) + static_cast<int>(canFlowSD) + static_cast<int>(canFlowWD) + static_cast<int>(canFlowED);

				if (numFlowDownSides > 0) {
					//numFlowSides = numFlowDownSides;
					doFlowN &= (canFlowND || (nCellOg.amount >= cell.amount));
					doFlowS &= (canFlowSD || (sCellOg.amount >= cell.amount));
					doFlowW &= (canFlowWD || (wCellOg.amount >= cell.amount));
					doFlowE &= (canFlowED || (eCellOg.amount >= cell.amount));
					numFlowSides = static_cast<int>(doFlowN) + static_cast<int>(doFlowS) + static_cast<int>(doFlowW) + static_cast<int>(doFlowE);
				}

				int flowingTotal = cell.amount;
				if (doFlowN) flowingTotal += nCellOg.amount;
				if (doFlowS) flowingTotal += sCellOg.amount;
				if (doFlowW) flowingTotal += wCellOg.amount;
				if (doFlowE) flowingTotal += eCellOg.amount;

				const auto maxAmount = vmax(cell.amount, nCell.amount, sCell.amount, wCell.amount, eCell.amount);

				int maxDiff = std::numeric_limits<int>::min();
				int minDiff = std::numeric_limits<int>::max();
				if (doFlowN) {
					maxDiff = (cell.amount - nCell.amount);
					minDiff = (cell.amount - nCell.amount);
				}
				if (doFlowS) {
					maxDiff = vmax(maxDiff, cell.amount - sCell.amount);
					minDiff = vmin(minDiff, cell.amount - sCell.amount);
				}
				if (doFlowW) {
					maxDiff = vmax(maxDiff, cell.amount - wCell.amount);
					minDiff = vmin(minDiff, cell.amount - wCell.amount);
				}
				if (doFlowE) {
					maxDiff = vmax(maxDiff, cell.amount - eCell.amount);
					minDiff = vmin(minDiff, cell.amount - eCell.amount);
				}

				if ((maxAmount > 1) || (numFlowDownSides > 0) || (cell.capacity <= 0)) {
					const auto flowCells = (static_cast<int>((cell.capacity > 0) && (numFlowDownSides <= 0)) + numFlowSides);
					if ((numFlowDownSides <= 0) && (flowCells > 1) && (std::abs(maxDiff) <= 1) && (std::abs(minDiff) <= 1) && ((maxDiff - minDiff) <= 1)) {
						/*const auto m = (flowingTotal % flowCells);
						if (m > 0) {
							if (m == 1) flowingTotal -= 1;
							else if (m >= (flowCells - 2)) flowingTotal += flowCells - m;
						}*/
						doFlowN = doFlowS = doFlowW = doFlowE = false;
					}

					if (doFlowN || doFlowS || doFlowW || doFlowE) {
						const float rawAvg = static_cast<float>(flowingTotal) / flowCells;
						const int avgAmountFloor = static_cast<int>(rawAvg);
						const int avgAmount = static_cast<int>(std::ceil(rawAvg));

						const int cellReserved = (numFlowDownSides > 0) ? 0 : vmin(cell.capacity, avgAmountFloor, flowingTotal);
						flowingTotal -= cellReserved;

						if (doFlowN) {
							nCell.amount = vmin(nCell.capacity, avgAmount, flowingTotal);
							flowingTotal -= nCell.amount;
							if (nCell.amount != nCellOg.amount)
								world.setFluidState(nPos, nCell.toFluidState(*this), true, false, true);
						}
						if (doFlowS) {
							sCell.amount = vmin(sCell.capacity, avgAmount, flowingTotal);
							flowingTotal -= sCell.amount;
							if (sCell.amount != sCellOg.amount)
								world.setFluidState(sPos, sCell.toFluidState(*this), true, false, true);
						}
						if (doFlowW) {
							wCell.amount = vmin(wCell.capacity, avgAmount, flowingTotal);
							flowingTotal -= wCell.amount;
							if (wCell.amount != wCellOg.amount)
								world.setFluidState(wPos, wCell.toFluidState(*this), true, false, true);
						}
						if (doFlowE) {
							eCell.amount = vmin(eCell.capacity, avgAmount, flowingTotal);
							flowingTotal -= eCell.amount;
							if (eCell.amount != eCellOg.amount)
								world.setFluidState(ePos, eCell.toFluidState(*this), true, false, true);
						}
						cell.amount = cellReserved + flowingTotal;
					}
				}
			}
		}
		if (cell.amount > cell.capacity) { // flow up
			const auto uPos = direction::offsetVector(blockPos, Direction::UP);
			FluidStateRef uFluidState = world.getFluidState(uPos);
			BlockStateRef uBlockState = world.getBlockState(uPos);
			FluidCell uCell(*this, uFluidState, uBlockState, Direction::DOWN);

			const auto flowU = vmin(cell.amount, uCell.capacity - uCell.amount);
			if (flowU > 0) {
				uCell.amount += flowU;
				world.setFluidState(uPos, uCell.toFluidState(*this), true, false, true);
				cell.amount -= flowU;
			}
		}

		if (cell.amount < 0) cell.amount = 0;
		// TODO: update flow direction
		if (cell.amount != amount) {
			if (cell.amount == 0)
				world.setFluidState(blockPos, {}, true, true, false);
			else
				world.setFluidState(blockPos, FiniteFluidState(*this, cell.amount - 1, FluidFlow::Still, state.getMetadata()), true, true, false);
				world.scheduleFluidUpdate(blockPos, Direction::UNDEFINED, 1);
		}
	}


	void FluidFinite::addQuadsToBuffer(FluidStateRef fluidState, const glm::ivec3& blockPos, RenderLayer layer, const glm::ivec3& offset, const MeshingWorldView worldView, std::vector<BlockQuad>& buffer) const {
		if (layer != getRenderLayer()) return;
		
		BlockStateRef blockState = worldView.getBlockState(blockPos);

		const auto height = getFillAmount(fluidState, blockState);
		if (height <= 0) return;

		const auto dPos = direction::offsetVector(blockPos, Direction::DOWN);
		const auto uPos = direction::offsetVector(blockPos, Direction::UP);
		const auto nPos = direction::offsetVector(blockPos, Direction::NORTH);
		const auto sPos = direction::offsetVector(blockPos, Direction::SOUTH);
		const auto wPos = direction::offsetVector(blockPos, Direction::WEST);
		const auto ePos = direction::offsetVector(blockPos, Direction::EAST);
		const auto downHeight = getFillAmount(*this, worldView.getFluidState(dPos), worldView.getBlockState(dPos));
		const auto upHeight = getFillAmount(*this, worldView.getFluidState(uPos), worldView.getBlockState(uPos));
		const auto northHeight = getFillAmount(*this, worldView.getFluidState(nPos), worldView.getBlockState(nPos));
		const auto southHeight = getFillAmount(*this, worldView.getFluidState(sPos), worldView.getBlockState(sPos));
		const auto westHeight = getFillAmount(*this, worldView.getFluidState(wPos), worldView.getBlockState(wPos));
		const auto eastHeight = getFillAmount(*this, worldView.getFluidState(ePos), worldView.getBlockState(ePos));
		
		// TODO: smoothing, use flow texture on top/bottom for flowing fluid

		constexpr float backFaceOffset = -0.001f;

		if (((upHeight <= 0) || (height < 1))) {
			const auto upQuad = createTexturedBlockQuad(
				{ { 0, height, 0 }, { 0, 0 } },
				{ { 0, height, 1 }, { 0, 1 } },
				{ { 1, height, 0 }, { 1, 0 } },
				{ { 1, height, 1 }, { 1, 1 } },
				*stillTexture);
			buffer.push_back(upQuad + offset);
			buffer.push_back(upQuad.getBackQuad() + offset + (direction::toVector<glm::vec3>(Direction::UP) * backFaceOffset));
		}
		if ((downHeight < 1) && !worldView.getBlockState(dPos).canCullAdjacentFace(Direction::UP, fluidState)) {
			const auto downQuad = createTexturedBlockQuad(
				{ { 0, 0, 1 }, { 0, 0 } },
				{ { 0, 0, 0 }, { 0, 1 } },
				{ { 1, 0, 1 }, { 1, 0 } },
				{ { 1, 0, 0 }, { 1, 1 } },
				*stillTexture);
			buffer.push_back(downQuad + offset);
			buffer.push_back(downQuad.getBackQuad() + offset + (direction::toVector<glm::vec3>(Direction::DOWN) * backFaceOffset));
		}
		if ((northHeight < height) && !worldView.getBlockState(nPos).canCullAdjacentFace(Direction::SOUTH, fluidState)) {
			const auto northQuad = createTexturedBlockQuad(
				{ { 1, height,      0 }, { 0.0f, 0.5f * (1 - height)      } },
				{ { 1, northHeight, 0 }, { 0.0f, 0.5f * (1 - northHeight) } },
				{ { 0, height,      0 }, { 0.5f, 0.5f * (1 - height)      } },
				{ { 0, northHeight, 0 }, { 0.5f, 0.5f * (1 - northHeight) } },
				*flowTexture);
			buffer.push_back(northQuad + offset);
			buffer.push_back(northQuad.getBackQuad() + offset + (direction::toVector<glm::vec3>(Direction::NORTH) * backFaceOffset));
		}
		if ((southHeight < height) && !worldView.getBlockState(sPos).canCullAdjacentFace(Direction::NORTH, fluidState)) {
			const auto southQuad = createTexturedBlockQuad(
				{ { 0, height,      1 }, { 0.0f, 0.5f * (1 - height)      } },
				{ { 0, southHeight, 1 }, { 0.0f, 0.5f * (1 - southHeight) } },
				{ { 1, height,      1 }, { 0.5f, 0.5f * (1 - height)      } },
				{ { 1, southHeight, 1 }, { 0.5f, 0.5f * (1 - southHeight) } },
				*flowTexture);
			buffer.push_back(southQuad + offset);
			buffer.push_back(southQuad.getBackQuad() + offset + (direction::toVector<glm::vec3>(Direction::SOUTH) * backFaceOffset));
		}
		if ((westHeight < height) && !worldView.getBlockState(wPos).canCullAdjacentFace(Direction::EAST, fluidState)) {
			const auto westQuad = createTexturedBlockQuad(
				{ { 0, height,     0 }, { 0.0f, 0.5f * (1 - height)     } },
				{ { 0, westHeight, 0 }, { 0.0f, 0.5f * (1 - westHeight) } },
				{ { 0, height,     1 }, { 0.5f, 0.5f * (1 - height)     } },
				{ { 0, westHeight, 1 }, { 0.5f, 0.5f * (1 - westHeight) } },
				*flowTexture);
			buffer.push_back(westQuad + offset);
			buffer.push_back(westQuad.getBackQuad() + offset + (direction::toVector<glm::vec3>(Direction::WEST) * backFaceOffset));
		}
		if ((eastHeight < height) && !worldView.getBlockState(ePos).canCullAdjacentFace(Direction::WEST, fluidState)) {
			const auto eastQuad = createTexturedBlockQuad(
				{ { 1, height,     1 }, { 0.0f, 0.5f * (1 - height)     } },
				{ { 1, eastHeight, 1 }, { 0.0f, 0.5f * (1 - eastHeight) } },
				{ { 1, height,     0 }, { 0.5f, 0.5f * (1 - height)     } },
				{ { 1, eastHeight, 0 }, { 0.5f, 0.5f * (1 - eastHeight) } },
				*flowTexture);
			buffer.push_back(eastQuad + offset);
			buffer.push_back(eastQuad.getBackQuad() + offset + (direction::toVector<glm::vec3>(Direction::EAST) * backFaceOffset));
		}
	}

	void FluidFinite::onResourceLoadComplete() const {
		stillTexture = &ResourceManager::instance().getBlockTextures().getSprite("blocks/" + getName() + "_still");
		flowTexture = &ResourceManager::instance().getBlockTextures().getSprite("blocks/" + getName() + "_flow");
	}

	void FluidFinite::addTextureNamesToSet(std::set<std::string>& textureNameSet) const {
		textureNameSet.emplace("blocks/" + getName() + "_still");
		textureNameSet.emplace("blocks/" + getName() + "_flow");
	}

}