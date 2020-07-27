#pragma once

#include <type_traits>

namespace eng {

	enum class MeshingPriority {
		PlayerInteract = 0, // blocks placed or broken by player
		FluidUpdate, // fluid updates
		BlockUpdate, // misc. block updates
		ChunkUnload, // neighbor unloaded
		ChunkLoad, // chunk loading and generation
	};

	constexpr bool operator <(MeshingPriority lhs, MeshingPriority rhs) noexcept {
		return static_cast<std::underlying_type_t<MeshingPriority>>(lhs) < static_cast<std::underlying_type_t<MeshingPriority>>(rhs);
	}
	constexpr bool operator >(MeshingPriority lhs, MeshingPriority rhs) noexcept {
		return static_cast<std::underlying_type_t<MeshingPriority>>(lhs) > static_cast<std::underlying_type_t<MeshingPriority>>(rhs);
	}
	constexpr bool operator <=(MeshingPriority lhs, MeshingPriority rhs) noexcept {
		return static_cast<std::underlying_type_t<MeshingPriority>>(lhs) <= static_cast<std::underlying_type_t<MeshingPriority>>(rhs);
	}
	constexpr bool operator >=(MeshingPriority lhs, MeshingPriority rhs) noexcept {
		return static_cast<std::underlying_type_t<MeshingPriority>>(lhs) >= static_cast<std::underlying_type_t<MeshingPriority>>(rhs);
	}

}