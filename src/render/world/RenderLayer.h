#pragma once

#include <array>

namespace eng {

	namespace render_layer {

		enum class RenderLayer { Opaque = 0, Cutout = 1, Transparent = 2, };

		constexpr RenderLayer Opaque = RenderLayer::Opaque;
		constexpr RenderLayer Cutout = RenderLayer::Cutout;
		constexpr RenderLayer Transparent = RenderLayer::Transparent;

		constexpr std::array<RenderLayer, 3> layers = { Opaque, Cutout, Transparent };

		constexpr typename std::underlying_type_t<RenderLayer> getIndex(RenderLayer layer) {
			return static_cast<std::underlying_type_t<RenderLayer>>(layer);
		}

	}

	using RenderLayer = render_layer::RenderLayer;


	constexpr bool operator <(const RenderLayer& lhs, const RenderLayer& rhs) {
		return render_layer::getIndex(lhs) < render_layer::getIndex(rhs);
	}
	constexpr bool operator >(const RenderLayer& lhs, const RenderLayer& rhs) {
		return render_layer::getIndex(lhs) > render_layer::getIndex(rhs);
	}
	constexpr bool operator <=(const RenderLayer& lhs, const RenderLayer& rhs) {
		return render_layer::getIndex(lhs) <= render_layer::getIndex(rhs);
	}
	constexpr bool operator >=(const RenderLayer& lhs, const RenderLayer& rhs) {
		return render_layer::getIndex(lhs) >= render_layer::getIndex(rhs);
	}

}