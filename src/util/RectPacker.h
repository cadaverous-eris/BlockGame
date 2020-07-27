#pragma once

// https://github.com/juj/RectangleBinPack/

#include <vector>
#include <cassert>

#include <glm/vec2.hpp>

#include "math/math.h"

namespace eng {

	template<typename T>
	class RectPacker {
	public:
		using tvec2 = glm::vec<2, T>;

		// Represents a single level (a horizontal line) of the skyline/horizon/envelope.
		struct SkylineNode {
			tvec2 pos; // The starting coordinate (leftmost)
			T width; // The line width (the ending coordinate (inclusive) will be pos.x + width - 1)
			SkylineNode(const tvec2& pos, T width) : pos(pos), width(width) {}
		};

	private:
		tvec2 binSize;
		size_t usedArea = 0;
		std::vector<SkylineNode> skyline;

	public:

		RectPacker(const tvec2& binSize) :
			binSize(binSize),
			skyline { SkylineNode{ tvec2{ 0, 0 }, binSize.x } } {}

		RectPacker(const RectPacker&) = delete;
		RectPacker(RectPacker&&) = default;
		RectPacker& operator =(const RectPacker&) = delete;
		RectPacker& operator =(RectPacker&&) = default;

		// returns the position of the packed rect
		tvec2 packRect(const tvec2& rectSize) {
			tvec2 bestSize = { std::numeric_limits<T>::max(), std::numeric_limits<T>::max() };
			size_t bestIndex;
			bool foundPos = false;
			tvec2 bestPos;

			for (size_t i = 0; i < skyline.size(); i++) {
				const SkylineNode& node = skyline[i];
				T y;
				if (rectFits(i, rectSize, y)) {
					if (y + rectSize.y < bestSize.y || (y + rectSize.y == bestSize.y && node.width < bestSize.x)) {
						bestIndex = i;
						foundPos = true;
						bestSize = { node.width, y + rectSize.y };
						bestPos = { node.pos.x, y };
					}
				}
			}

			if (foundPos) {
				// Perform the actual packing.
				addSkylineLevel(bestIndex, bestPos, rectSize);

				usedArea += getArea(rectSize);
				return bestPos;
			}
			expandBin(rectSize);
			return packRect(rectSize);
		}

		const tvec2& getBinSize() const { return binSize; }

		float getOccupancy() const {
			return static_cast<float>(usedArea) / getArea(static_cast<glm::vec2>(binSize));
		}

		// returns the minimum size the bin could be to fit the contents
		tvec2 getMinPackedBinSize() const {
			tvec2 minSize { 0, 0 };
			for (const auto& node : skyline) {
				if (node.pos.y > 0 && (node.pos.x + node.width > minSize.x))
					minSize.x = node.pos.x + node.width;
				if (node.pos.y > minSize.y)
					minSize.y = node.pos.y;
			}
			return minSize;
		}

	private:

		void expandBin(const tvec2& rectSize) {
			//tvec2 expandedBinSize = binSize * T { 2 };
			tvec2 expandedBinSize = binSize + static_cast<T>(std::ceil(static_cast<float>(vmax(rectSize.x, rectSize.y)) * 1.5f));
			if (expandedBinSize.x > binSize.x) {
				skyline.emplace_back(tvec2{ binSize.x, 0 }, expandedBinSize.x - binSize.x);
				mergeSkylines();
			}
			binSize = expandedBinSize;
		}

		bool rectFits(size_t skylineNodeIndex, const tvec2& rectSize, T& y) const {
			auto x = skyline[skylineNodeIndex].pos.x;
			if (x + rectSize.x > binSize.x)
				return false;
			auto widthRemaining = rectSize.x;
			y = skyline[skylineNodeIndex].pos.y;
			for (size_t i = skylineNodeIndex; i < skyline.size(); i++) {
				y = vmax(y, skyline[i].pos.y);
				if (y + rectSize.y > binSize.y)
					return false;
				if (skyline[i].width >= widthRemaining)
					return true;
				widthRemaining -= skyline[i].width;
			}
			return false;
		}

		void addSkylineLevel(size_t skylineNodeIndex, const tvec2& rectPos, const tvec2& rectSize) {
			SkylineNode& newNode = *skyline.emplace(skyline.begin() + skylineNodeIndex,
													tvec2{ rectPos.x, rectPos.y + rectSize.y }, rectSize.x);
			assert(newNode.pos.x + newNode.width <= binSize.x);
			assert(newNode.pos.y <= binSize.y);

			for (size_t i = skylineNodeIndex + 1; i < skyline.size(); i++) {
				const SkylineNode& prevNode = skyline[i - 1];
				SkylineNode& node = skyline[i];
				assert(prevNode.pos.x <= node.pos.x);

				if (node.pos.x < prevNode.pos.x + prevNode.width) { // if node.pos is within prevNode
					auto shrink = prevNode.pos.x + prevNode.width - node.pos.x;

					node.pos.x += shrink;

					if (node.width <= shrink) {
						skyline.erase(skyline.begin() + i);
						--i;
					} else {
						node.width -= shrink;
						break;
					}
				} else {
					break;
				}
			}
			mergeSkylines();
		}

		void mergeSkylines() {
			for (size_t i = 0; i + 1 < skyline.size(); i++) {
				const SkylineNode& nextNode = skyline[i + 1];
				SkylineNode& node = skyline[i];
				if (node.pos.y == nextNode.pos.y) {
					node.width += nextNode.width;
					skyline.erase(skyline.begin() + (i + 1));
					i--;
				}
			}
		}

	};

}
