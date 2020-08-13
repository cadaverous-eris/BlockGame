#pragma once

#include <variant>
#include <typeinfo>
#include <limits>
#include <algorithm>
#include <iostream>

#include "nbt_types.h"
#include "util/type_traits.h"
#include "NBTCompound.h"
#include "parsing_utils.h"
#include "NBTCompat.h"

namespace nbt {

    struct bad_nbt_cast : public std::bad_cast {
		const char* what() const noexcept override { return "Bad NBT cast"; };
	};

    using parsing::ParseError;
	using parsing::parseSNBT;

    class NBT;
	std::ostream& operator <<(std::ostream&, const NBT&);

    class NBT {
        using variant_type = std::variant<
			nbt_byte,
            nbt_short,
            nbt_int,
            nbt_long,
            nbt_float,
            nbt_double,
            nbt_byte_array,
            nbt_string,
            nbt_list,
            nbt_compound,
            nbt_int_array,
            nbt_long_array
		>;

		variant_type value;

    public:

        NBT() noexcept = default;

        NBT(const NBT&) = default;
        NBT(NBT&&) = default;

        NBT(const nbt_byte) noexcept;
        NBT(const nbt_short) noexcept;
        NBT(const nbt_int) noexcept;
        NBT(const nbt_long) noexcept;
        NBT(const nbt_float) noexcept;
        NBT(const nbt_double) noexcept;
        NBT(const nbt_byte_array&) noexcept;
        NBT(nbt_byte_array&&) noexcept;
        NBT(const nbt_string&) noexcept;
        NBT(nbt_string&&) noexcept;
        NBT(const nbt_list&) noexcept;
        NBT(nbt_list&&) noexcept;
        NBT(const nbt_compound&) noexcept;
        NBT(nbt_compound&&) noexcept;
        NBT(const nbt_int_array&) noexcept;
        NBT(nbt_int_array&&) noexcept;
        NBT(const nbt_long_array&) noexcept;
        NBT(nbt_long_array&&) noexcept;

        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T>), int> = 0>
		NBT(const T& t) : NBT(NBTTypeCompat<T>::toNBT(t)) {}

        ~NBT() = default;

        NBT& operator =(const NBT&) = default;
		NBT& operator =(NBT&&) = default;

        NBT& operator =(const nbt_byte b) noexcept;
        NBT& operator =(const nbt_short s) noexcept;
        NBT& operator =(const nbt_int i) noexcept;
        NBT& operator =(const nbt_long l) noexcept;
        NBT& operator =(const nbt_float f) noexcept;
        NBT& operator =(const nbt_double d) noexcept;
        NBT& operator =(const nbt_byte_array& a) noexcept;
        NBT& operator =(nbt_byte_array&& a) noexcept;
        NBT& operator =(const nbt_string& s) noexcept;
        NBT& operator =(nbt_string&& s) noexcept;
        NBT& operator =(const nbt_list& l) noexcept;
        NBT& operator =(nbt_list&& l) noexcept;
        NBT& operator =(const nbt_compound& c) noexcept;
        NBT& operator =(nbt_compound&& c) noexcept;
        NBT& operator =(const nbt_int_array& a) noexcept;
        NBT& operator =(nbt_int_array&& a) noexcept;
        NBT& operator =(const nbt_long_array& a) noexcept;
        NBT& operator =(nbt_long_array&& a) noexcept;

        template<typename T, std::enable_if_t<is_convertible_to_nbt_v<T>, int> = 0>
		NBT& operator =(const T& t) {
			*this = NBTTypeCompat<T>::toNBT(t);
			return *this;
		}

        [[nodiscard]] inline bool isByte() const noexcept { return is<TagByte>(); }
		[[nodiscard]] inline bool isShort() const noexcept { return is<TagShort>(); }
        [[nodiscard]] inline bool isInt() const noexcept { return is<TagInt>(); }
        [[nodiscard]] inline bool isLong() const noexcept { return is<TagLong>(); }
        [[nodiscard]] inline bool isFloat() const noexcept { return is<TagFloat>(); }
        [[nodiscard]] inline bool isDouble() const noexcept { return is<TagDouble>(); }
        [[nodiscard]] inline bool isByteArray() const noexcept { return is<TagByteArray>(); }
        [[nodiscard]] inline bool isString() const noexcept { return is<TagString>(); }
        [[nodiscard]] inline bool isList() const noexcept { return is<TagList>(); }
        [[nodiscard]] inline bool isCompound() const noexcept { return is<TagCompound>(); }
        [[nodiscard]] inline bool isIntArray() const noexcept { return is<TagIntArray>(); }
        [[nodiscard]] inline bool isLongArray() const noexcept { return is<TagLongArray>(); }
		[[nodiscard]] inline bool isIntegral() const noexcept { return isByte() || isShort() || isInt() || isLong(); }
        [[nodiscard]] inline bool isFloatingPoint() const noexcept { return isFloat() || isDouble(); }
		[[nodiscard]] inline bool isNumber() const noexcept { return isIntegral() || isFloatingPoint(); }
		[[nodiscard]] bool is(const TagType tagType) const noexcept;
		template<TagType tagType>
		[[nodiscard]] inline bool is() const noexcept {
			if constexpr (tagType == TagEnd) return false;
			else return std::holds_alternative<nbt_type<tagType>>(value);
		}
		template<typename T, std::enable_if_t<is_testable_nbt_type_v<T> || is_nbt_type_v<T>, int> = 0>
		[[nodiscard]] bool is() const noexcept {
            if constexpr (std::is_same_v<T, nbt_byte>) return is<TagByte>();
            else if constexpr (std::is_same_v<T, nbt_short>) return is<TagShort>();
            else if constexpr (std::is_same_v<T, nbt_int>) return is<TagInt>();
            else if constexpr (std::is_same_v<T, nbt_long>) return is<TagLong>();
            else if constexpr (std::is_same_v<T, nbt_float>) return is<TagFloat>();
            else if constexpr (std::is_same_v<T, nbt_double>) return is<TagDouble>();
            else if constexpr (std::is_same_v<T, nbt_byte_array>) return is<TagByteArray>();
			else if constexpr (std::is_same_v<T, nbt_string>) return is<TagString>();
			else if constexpr (std::is_same_v<T, nbt_list>) return is<TagList>();
			else if constexpr (std::is_same_v<T, nbt_compound>) return is<TagCompound>();
			else if constexpr (std::is_same_v<T, nbt_int_array>) return is<TagIntArray>();
            else if constexpr (std::is_same_v<T, nbt_long_array>) return is<TagLongArray>();
			else return NBTTypeCompat<T>::nbtIs(*this);
        }

		[[nodiscard]] inline TagType getTagType() const noexcept {
			return TagType { static_cast<int>(value.index()) + 1 };
		}

        [[nodiscard]] nbt_byte& asByte();
		[[nodiscard]] const nbt_byte& asByte() const;
        [[nodiscard]] nbt_short& asShort();
		[[nodiscard]] const nbt_short& asShort() const;
		[[nodiscard]] nbt_int& asInt();
		[[nodiscard]] const nbt_int& asInt() const;
        [[nodiscard]] nbt_long& asLong();
		[[nodiscard]] const nbt_long& asLong() const;
        [[nodiscard]] nbt_float& asFloat();
		[[nodiscard]] const nbt_float& asFloat() const;
        [[nodiscard]] nbt_double& asDouble();
		[[nodiscard]] const nbt_double& asDouble() const;
        [[nodiscard]] nbt_byte_array& asByteArray();
		[[nodiscard]] const nbt_byte_array& asByteArray() const;
        [[nodiscard]] nbt_string& asString();
		[[nodiscard]] const nbt_string& asString() const;
        [[nodiscard]] nbt_list& asList();
		[[nodiscard]] const nbt_list& asList() const;
        [[nodiscard]] nbt_compound& asCompound();
		[[nodiscard]] const nbt_compound& asCompound() const;
        [[nodiscard]] nbt_int_array& asIntArray();
		[[nodiscard]] const nbt_int_array& asIntArray() const;
        [[nodiscard]] nbt_long_array& asLongArray();
		[[nodiscard]] const nbt_long_array& asLongArray() const;
		template<TagType tagType, typename = std::void_t<nbt_type<tagType>>>
		[[nodiscard]] inline nbt_type<tagType>& as() {
			if constexpr (tagType == TagByte) return asByte();
			else if constexpr (tagType == TagShort) return asShort();
			else if constexpr (tagType == TagInt) return asInt();
			else if constexpr (tagType == TagLong) return asLong();
			else if constexpr (tagType == TagFloat) return asFloat();
			else if constexpr (tagType == TagDouble) return asDouble();
			else if constexpr (tagType == TagByteArray) return asByteArray();
			else if constexpr (tagType == TagString) return asString();
            else if constexpr (tagType == TagList) return asList();
			else if constexpr (tagType == TagCompound) return asCompound();
			else if constexpr (tagType == TagIntArray) return asIntArray();
			else if constexpr (tagType == TagLongArray) return asLongArray();
		}
		template<TagType tagType, typename = std::void_t<nbt_type<tagType>>>
		[[nodiscard]] inline const nbt_type<tagType>& as() const {
			if constexpr (tagType == TagByte) return asByte();
			else if constexpr (tagType == TagShort) return asShort();
			else if constexpr (tagType == TagInt) return asInt();
			else if constexpr (tagType == TagLong) return asLong();
			else if constexpr (tagType == TagFloat) return asFloat();
			else if constexpr (tagType == TagDouble) return asDouble();
			else if constexpr (tagType == TagByteArray) return asByteArray();
			else if constexpr (tagType == TagString) return asString();
            else if constexpr (tagType == TagList) return asList();
			else if constexpr (tagType == TagCompound) return asCompound();
			else if constexpr (tagType == TagIntArray) return asIntArray();
			else if constexpr (tagType == TagLongArray) return asLongArray();
		}

        [[nodiscard]] nbt_byte getByte() const;
        [[nodiscard]] nbt_short getShort() const;
		[[nodiscard]] nbt_int getInt() const;
        [[nodiscard]] nbt_long getLong() const;
        [[nodiscard]] nbt_float getFloat() const;
        [[nodiscard]] nbt_double getDouble() const;
        [[nodiscard]] nbt_byte_array getByteArray() const;
        [[nodiscard]] nbt_string getString() const;
        [[nodiscard]] nbt_list getList() const;
        [[nodiscard]] nbt_compound getCompound() const;
        [[nodiscard]] nbt_int_array getIntArray() const;
        [[nodiscard]] nbt_long_array getLongArray() const;
		template<TagType tagType, typename = std::void_t<nbt_type<tagType>>>
		[[nodiscard]] inline nbt_type<tagType> get() const {
			if constexpr (tagType == TagByte) return getByte();
			else if constexpr (tagType == TagShort) return getShort();
			else if constexpr (tagType == TagInt) return getInt();
			else if constexpr (tagType == TagLong) return getLong();
			else if constexpr (tagType == TagFloat) return getFloat();
			else if constexpr (tagType == TagDouble) return getDouble();
			else if constexpr (tagType == TagByteArray) return getByteArray();
			else if constexpr (tagType == TagString) return getString();
            else if constexpr (tagType == TagList) return getList();
			else if constexpr (tagType == TagCompound) return getCompound();
			else if constexpr (tagType == TagIntArray) return getIntArray();
			else if constexpr (tagType == TagLongArray) return getLongArray();
		}
		template<typename T>
		[[nodiscard]] inline std::enable_if_t<is_convertible_from_nbt_v<T> || is_nbt_type_v<T>, T> get() const {
            if constexpr (std::is_same_v<T, nbt_byte>) return getByte();
            else if constexpr (std::is_same_v<T, nbt_short>) return getShort();
            else if constexpr (std::is_same_v<T, nbt_int>) return getInt();
            else if constexpr (std::is_same_v<T, nbt_long>) return getLong();
            else if constexpr (std::is_same_v<T, nbt_float>) return getFloat();
            else if constexpr (std::is_same_v<T, nbt_double>) return getDouble();
            else if constexpr (std::is_same_v<T, nbt_byte_array>) return getByteArray();
			else if constexpr (std::is_same_v<T, nbt_string>) return getString();
			else if constexpr (std::is_same_v<T, nbt_list>) return getList();
			else if constexpr (std::is_same_v<T, nbt_compound>) return getCompound();
			else if constexpr (std::is_same_v<T, nbt_int_array>) return getIntArray();
            else if constexpr (std::is_same_v<T, nbt_long_array>) return getLongArray();
			else return NBTTypeCompat<T>::fromNBT(*this);
        }

		template<typename T, std::enable_if_t<(is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>, int> = 0>
		[[nodiscard]] inline T getOrDefault(const T& dflt) const noexcept {
			return is<T>() ? get<T>() : dflt;
		}
		template<typename T, std::enable_if_t<((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>) && std::is_default_constructible_v<T>, int> = 0>
		[[nodiscard]] inline T getOrDefault() const noexcept(std::is_nothrow_default_constructible_v<T>) {
			return is<T>() ? get<T>() : T();
		}
		template<typename T, std::enable_if_t<(((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>) && !eng::is_optional_v<T> && !std::is_same_v<T, std::nullptr_t> && !std::is_same_v<T, std::nullopt_t>), int> = 0>
		[[nodiscard]] inline std::optional<T> getOptional() const noexcept {
			return is<T>() ? std::make_optional<T>(get<T>()) : std::nullopt;
		}

        template<typename T, typename... Args>
		inline T& emplace(Args&&... args);
		template<typename T, typename U, typename... Args>
		inline T& emplace(std::initializer_list<U> initList, Args&&... args);
		template<TagType tagType, typename... Args>
		nbt_type<tagType>& emplace(Args&&... args);
		template<TagType tagType, typename U, typename... Args>
		nbt_type<tagType>& emplace(std::initializer_list<U> initList, Args&&... args);

		size_t size() const;

		[[nodiscard]] inline bool operator ==(const NBT& b) const noexcept { return value == b.value; }
		[[nodiscard]] inline bool operator !=(const NBT& b) const noexcept { return value != b.value; }

        [[nodiscard]] operator nbt_byte() const;
		[[nodiscard]] operator nbt_short() const;
		[[nodiscard]] operator nbt_int() const;
		[[nodiscard]] operator nbt_long() const;
		[[nodiscard]] operator nbt_float() const;
		[[nodiscard]] operator nbt_double() const;
		[[nodiscard]] operator nbt_byte_array() const;
        [[nodiscard]] operator nbt_string() const;
        [[nodiscard]] operator nbt_string_view() const;
        [[nodiscard]] operator nbt_list() const;
        [[nodiscard]] operator nbt_compound() const;
        [[nodiscard]] operator nbt_int_array() const;
        [[nodiscard]] operator nbt_long_array() const;

		//[[nodiscard]] operator bool() const noexcept;

        template<typename T, std::enable_if_t<is_convertible_from_nbt_v<T>, int> = 0>
		inline operator T() const { return NBTTypeCompat<T>::fromNBT(*this); }

        std::string toSNBT(const size_t indentAmount = 0, const char indentChar = ' ', const size_t indentLevel = 0) const;

		friend std::ostream& operator <<(std::ostream&, const NBT&);

    };

    template<typename T, typename... Args>
	inline T& NBT::emplace(Args&&... args) {
		return value.emplace<T>(std::forward<Args>(args)...);
	}
	template<typename T, typename U, typename... Args>
	inline T& NBT::emplace(std::initializer_list<U> initList, Args&&... args) {
		return value.emplace(initList, std::forward<Args>(args)...);
	}
	template<TagType tagType, typename... Args>
	nbt_type<tagType>& NBT::emplace(Args&&... args) {
		return value.emplace<nbt_type<tagType>>(std::forward<Args>(args)...);
	}
	template<TagType tagType, typename U, typename... Args>
	nbt_type<tagType>& NBT::emplace(std::initializer_list<U> initList, Args&&... args) {
		return value.emplace<nbt_type<tagType>>(initList, std::forward<Args>(args)...);
	}


	template<>
	struct NBTTypeCompat<nbt_string_view> {
		//static NBT toNBT(const nbt_string_view& sv) { return nbt_string(sv); }
		static nbt_string_view fromNBT(const NBT& nbt) { return nbt.asString(); }
		static bool nbtIs(const NBT& nbt) noexcept { return nbt.isString(); }
	};

    template<>
	struct NBTTypeCompat<uint8_t> {
		static NBT toNBT(const uint8_t& b) { return static_cast<nbt_byte>(b); }
		static uint8_t fromNBT(const NBT& nbt) { return static_cast<uint8_t>(nbt.asByte()); }
		static bool nbtIs(const NBT& nbt) noexcept { return nbt.isByte(); }
	};
    template<>
	struct NBTTypeCompat<uint16_t> {
		static NBT toNBT(const uint16_t& b) { return static_cast<nbt_short>(b); }
		static uint16_t fromNBT(const NBT& nbt) { return static_cast<uint16_t>(nbt.asShort()); }
		static bool nbtIs(const NBT& nbt) noexcept { return nbt.isShort(); }
	};
    template<>
	struct NBTTypeCompat<uint32_t> {
		static NBT toNBT(const uint32_t& b) { return static_cast<nbt_int>(b); }
		static uint32_t fromNBT(const NBT& nbt) { return static_cast<uint32_t>(nbt.asInt()); }
		static bool nbtIs(const NBT& nbt) noexcept { return nbt.isInt(); }
	};
    template<>
	struct NBTTypeCompat<uint64_t> {
		static NBT toNBT(const uint64_t& b) { return static_cast<nbt_long>(b); }
		static uint64_t fromNBT(const NBT& nbt) { return static_cast<uint64_t>(nbt.asLong()); }
		static bool nbtIs(const NBT& nbt) noexcept { return nbt.isInt(); }
	};

    template<>
	struct NBTTypeCompat<bool> {
		static NBT toNBT(const bool& b) { return static_cast<nbt_byte>(b); }
		static bool fromNBT(const NBT& nbt) { return static_cast<bool>(nbt.asByte()); }
		static bool nbtIs(const NBT& nbt) noexcept { return nbt.isByte(); }
	};

}

namespace nbt {

    template<typename T>
	std::enable_if_t<(is_testable_nbt_type_v<T> || is_nbt_type_v<T>), bool>
	NBTCompound::hasKey(const nbt_string_view key) const {
		const auto it = data.find(key);
		return (it != data.end()) && it->second.template is<T>();
	}

	template<TagType tagType>
	nbt_type<tagType>& NBTCompound::at(const key_type& key) {
		return data.at(key).template as<tagType>();
	}
	template<TagType tagType>
	const nbt_type<tagType>& NBTCompound::at(const key_type& key) const {
		return data.at(key).template as<tagType>();
	}

	template<TagType tagType>
	const nbt_type<tagType>& NBTCompound::get(const key_type& key) const {
		return data.at(key).template as<tagType>();
	}
	template<typename T, std::enable_if_t<is_convertible_from_nbt_v<T> || is_nbt_type_v<T>, int>>
	T NBTCompound::get(const key_type& key) const {
		return data.at(key).template get<T>();
	}

	template<typename T>
	std::enable_if_t<((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>), T>
	NBTCompound::getOrDefault(const nbt_string_view key, const T& dflt) const noexcept {
		const auto it = data.find(key);
		return ((it != data.end()) && it->second.template is<T>()) ? it->second.template get<T>() : dflt;
	}
	template<typename T>
	std::enable_if_t<((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>) && std::is_default_constructible_v<T>, T>
	NBTCompound::getOrDefault(const nbt_string_view key) const noexcept(std::is_nothrow_default_constructible_v<T>) {
		const auto it = data.find(key);
		return ((it != data.end()) && it->second.template is<T>()) ? it->second.template get<T>() : T();
	}

	template<typename T>
	std::enable_if_t<(((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>) && !eng::is_optional_v<T> && !std::is_same_v<T, std::nullptr_t> && !std::is_same_v<T, std::nullopt_t>), std::optional<T>>
	NBTCompound::getOptional(const nbt_string_view key) const noexcept {
		const auto it = data.find(key);
		return ((it != data.end()) && it->second.template is<T>()) ? std::make_optional<T>(it->second.template get<T>()) : std::nullopt;
	}

}

#include "MiscNBTCompat.h"