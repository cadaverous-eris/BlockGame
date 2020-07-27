#pragma once

#include "util/Registry.h"
#include "Block.h"
#include "blocks/BlockAir.h"
#include "blocks/BlockBasicPlant.h"
#include "blocks/BlockLog.h"
#include "blocks/BlockLeaves.h"
#include "blocks/BlockGlass.h"
#include "blocks/BlockGrass.h"
#include "blocks/BlockSlab.h"


namespace eng {

	inline Registry<Block> block_registry {};

	namespace blocks {
		// TODO: more block definitions

		inline const BlockAir air("air");
		inline const BlockState empty_blockstate { air };

		inline const BasicBlock stone("stone");
		inline const BasicBlock cobblestone("cobblestone");
		inline const BasicBlock dirt("dirt");
		inline const BlockGrass grass("grass");
		inline const BasicBlock gravel("gravel");
		inline const BasicBlock sand("sand");
		inline const BasicBlock stone_brick("stone_brick");
		inline const BasicBlock stone_brick_carved("stone_brick_carved");
		inline const BasicBlock obsidian("obsidian");
		inline const BasicBlock wood_planks("wood_planks");
		inline const BlockLog log("log");
		inline const BlockLeaves leaves("leaves");
		inline const BlockTallGrass tall_grass("tall_grass");
		inline const BlockGlass glass("glass");

		inline const BlockGlass glass_red("glass_red");
		inline const BlockGlass glass_white("glass_white");
		inline const BlockGlass glass_blue("glass_blue");
		inline const BlockGlass glass_green("glass_green");

		inline const BlockSlab cobblestone_slab(cobblestone);
		inline const BlockSlab stone_brick_slab(stone_brick);

	}

}