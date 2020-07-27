#pragma once

#include <cstdint>

#include "FluidFlow.h"

namespace eng {

	class Fluid;
	using FluidRef = const Fluid&;

	class FluidState {
	public:
		using id_t = uint16_t;
		using metadata_t = uint32_t;

		using state_data_t = uint32_t;

		static constexpr auto id_bits = 10;
		static constexpr auto metadata_bits = (8 * sizeof(state_data_t)) - id_bits;
		
	private:
		state_data_t id : id_bits; // fluid id [0, 1024)
		state_data_t metadata : metadata_bits; // 22 bits of extra data that can be used differently by each fluid type
	public:
		FluidState(const id_t id, const metadata_t metadata) noexcept : id(id), metadata(metadata) {}
		FluidState(FluidRef fluid, const metadata_t metadata) noexcept;
		FluidState(FluidRef fluid);
		FluidState();

		FluidRef getFluid() const;
		inline id_t getFluidId() const noexcept { return static_cast<id_t>(id); }
		inline metadata_t getMetadata() const noexcept { return static_cast<metadata_t>(metadata); }

		inline FluidState withMetadata(const metadata_t metadata) const noexcept {
			return { getFluidId(), metadata };
		}

		inline bool operator ==(const FluidState& b) const noexcept {
			return (id == b.id) && (metadata == b.metadata);
		}
		inline bool operator !=(const FluidState& b) const noexcept {
			return (id != b.id) || (metadata != b.metadata);
		}

		bool isEmpty() const noexcept;
		inline operator bool() const noexcept { return !isEmpty(); }

		// TODO: wrapper functions for Fluid methods that take a FluidState???

	};

	using FluidStateRef = const FluidState&;


	class FiniteFluidState {
	public:
		using id_t = FluidState::id_t;
		using metadata_t = FluidState::metadata_t;
		using amount_t = uint8_t;
		using flow_t = FluidFlow;
		using flow_id_t = FluidFlowId;

		using state_data_t = FluidState::state_data_t;
		
		static constexpr amount_t base_capacity = 31;
		static constexpr amount_t max_capacity = 63;

		static constexpr auto id_bits = FluidState::id_bits;
		static constexpr auto amount_bits = 6;
		static constexpr auto flow_bits = 5;
		static constexpr auto metadata_bits = (8 * sizeof(state_data_t)) - id_bits - amount_bits - flow_bits;
		
		static constexpr metadata_t amount_mask = 0b0000000000111111;
		static constexpr metadata_t flow_mask =   0b0000011111000000;
		static constexpr metadata_t metadata_mask = ~(amount_mask | flow_mask);
	private:
		state_data_t id : FluidState::id_bits; // fluid id [0, 1024)
		state_data_t amount : amount_bits; // amount of fluid stored in this fluidstate [0, 64)
		state_data_t flow : flow_bits; // direction of flow (number of flow directions: 6 faces + 12 edges + 8 corners + 1 null = 27 (5 bits))
		state_data_t metadata : metadata_bits; // 11 bits of extra data that can be used differently by each fluid type
	
		inline static constexpr amount_t decodeAmount(const FluidState::metadata_t m) noexcept {
			return (m & amount_mask);
		}
		inline static  constexpr flow_id_t decodeFlowId(const FluidState::metadata_t m) noexcept {
			return (m & flow_mask) >> amount_bits;
		}
		inline static  constexpr metadata_t decodeMetadata(const FluidState::metadata_t m) noexcept {
			return (m & metadata_mask) >> (amount_bits + flow_bits);
		}
	public:
		FiniteFluidState(id_t id, amount_t amount, flow_t flow, metadata_t metadata) noexcept :
				id(id), amount(amount), flow(flow.getId()), metadata(metadata) {}
		FiniteFluidState(FluidRef fluid, amount_t amount, flow_t flow, const metadata_t metadata) noexcept;
		FiniteFluidState(FluidRef fluid, amount_t amount, const metadata_t metadata) noexcept;
		FiniteFluidState(FluidRef fluid, amount_t amount, flow_t flow);
		FiniteFluidState(FluidRef fluid, amount_t amount);
		FiniteFluidState();

		FiniteFluidState(FluidStateRef state) noexcept :
				id(state.getFluidId()),
				amount(decodeAmount(state.getMetadata())),
				flow(decodeFlowId(state.getMetadata())),
				metadata(decodeMetadata(state.getMetadata())) {}

		operator FluidState() const noexcept {
			FluidState::metadata_t m {};
			m |= amount;
			m |= (flow << amount_bits);
			m |= (metadata << (amount_bits + flow_bits));
			return { getFluidId(), m };
		}

		FluidRef getFluid() const;
		inline id_t getFluidId() const noexcept { return static_cast<id_t>(id); }
		inline amount_t getAmount() const noexcept { return static_cast<amount_t>(amount); }
		inline flow_t getFlow() const noexcept { return FluidFlow::fromId(flow); }
		inline metadata_t getMetadata() const noexcept { return static_cast<metadata_t>(metadata); }

		inline FiniteFluidState withAmount(const amount_t amount) const noexcept {
			return { getFluidId(), amount, FluidFlow::fromId(flow), getMetadata() };
		}
		inline FiniteFluidState withFlow(const flow_t flow) const noexcept {
			return { getFluidId(), getAmount(), flow, getMetadata() };
		}
		inline FiniteFluidState withMetadata(const metadata_t metadata) const noexcept {
			return { getFluidId(), getAmount(), FluidFlow::fromId(flow), metadata };
		}

		inline bool operator ==(const FiniteFluidState& b) const noexcept {
			return (id == b.id) && (amount == b.amount) && (flow == b.flow) && (metadata == b.metadata);
		}
		inline bool operator !=(const FiniteFluidState& b) const noexcept {
			return (id != b.id) || (amount != b.amount) || (flow != b.flow) || (metadata != b.metadata);
		}

		bool isEmpty() const noexcept;
	};

	class InfiniteFluidState {
	public:
		using id_t = FluidState::id_t;
		using metadata_t = FluidState::metadata_t;
		using flow_t = FluidFlow;
		using flow_id_t = FluidFlowId;

		using state_data_t = FluidState::state_data_t;

		static constexpr auto id_bits = FluidState::id_bits;
		static constexpr auto flow_bits = 5;
		static constexpr auto metadata_bits = (8 * sizeof(state_data_t)) - id_bits - flow_bits;

		static constexpr metadata_t flow_mask = 0b00011111;
		static constexpr metadata_t metadata_mask = ~flow_mask;
	private:
		state_data_t id : FluidState::id_bits; // fluid id [0, 1024)
		state_data_t flow : flow_bits; // direction of flow (number of flow directions: 6 faces + 12 edges + 8 corners + 1 null = 27 (5 bits))
		state_data_t metadata : metadata_bits; // 17 bits of extra data that can be used differently by each fluid type
	
		inline static constexpr flow_id_t decodeFlowId(const FluidState::metadata_t m) noexcept {
			return (m & flow_mask);
		}
		inline static constexpr metadata_t decodeMetadata(const FluidState::metadata_t m) noexcept {
			return (m & metadata_mask) >> flow_bits;
		}
	public:
		InfiniteFluidState(id_t id, flow_t flow, metadata_t metadata) noexcept :
				id(id), flow(flow.getId()), metadata(metadata) {}
		InfiniteFluidState(FluidRef fluid, flow_t flow, const metadata_t metadata) noexcept;
		InfiniteFluidState(FluidRef fluid, const metadata_t metadata) noexcept;
		InfiniteFluidState(FluidRef fluid, flow_t flow);
		InfiniteFluidState(FluidRef fluid);
		InfiniteFluidState();

		InfiniteFluidState(FluidStateRef state) noexcept :
				id(state.getFluidId()),
				flow(decodeFlowId(state.getMetadata())),
				metadata(decodeMetadata(state.getMetadata())) {}

		operator FluidState() const noexcept {
			FluidState::metadata_t m {};
			m |= flow;
			m |= (metadata << flow_bits);
			return { getFluidId(), m };
		}

		FluidRef getFluid() const;
		inline id_t getFluidId() const noexcept { return static_cast<id_t>(id); }
		inline flow_t getFlow() const noexcept { return FluidFlow::fromId(flow); }
		inline metadata_t getMetadata() const noexcept { return static_cast<metadata_t>(metadata); }

		inline InfiniteFluidState withFlow(const flow_t flow) const noexcept {
			return { getFluidId(), flow, getMetadata() };
		}
		inline InfiniteFluidState withMetadata(const metadata_t metadata) const noexcept {
			return { getFluidId(), FluidFlow::fromId(flow), metadata };
		}

		inline bool operator ==(const InfiniteFluidState& b) const noexcept {
			return (id == b.id) && (flow == b.flow) && (metadata == b.metadata);
		}
		inline bool operator !=(const InfiniteFluidState& b) const noexcept {
			return (id != b.id) || (flow != b.flow) || (metadata != b.metadata);
		}

		bool isEmpty() const noexcept;
	};

}