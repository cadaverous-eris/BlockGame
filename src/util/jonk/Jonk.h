#pragma once

#include <variant>
#include <type_traits>
#include <typeinfo>
#include <limits>
#include <algorithm>
#include <iostream>

#include "util/type_traits.h"
#include "jonk_types.h"
#include "JonkObject.h"
#include "parsing_utils.h"
#include "JonkCompat.h"

namespace jonk {

	using std::nullptr_t;

	struct bad_jonk_cast : public std::bad_cast {
		const char* what() const noexcept override { return "Bad jonk cast"; };
	};

	using parsing::ParseError;
	using parsing::parseJonk;

	class Jonk;
	std::ostream& operator <<(std::ostream&, const Jonk&);

	class Jonk {
	private:
		using variant_type = std::variant<
			std::monostate, // null value
			JonkObject,
			JonkArray,
			JonkString,
			JonkFloat,
			JonkInt,
			JonkUint,
			bool
		>;
		
		variant_type data;

	public:

		Jonk() noexcept = default;

		Jonk(const Jonk&) = default;
		Jonk(Jonk&&) = default;

		Jonk(const nullptr_t) noexcept;
		Jonk(const JonkObject& obj) noexcept;
		Jonk(JonkObject&& obj) noexcept;
		Jonk(const JonkArray& arr) noexcept;
		Jonk(JonkArray&& arr) noexcept;
		//Jonk(const JonkString::value_type* str);
		Jonk(const JonkString& str) noexcept;
		Jonk(JonkString&& str) noexcept;
		Jonk(const JonkFloat f) noexcept;
		Jonk(const JonkInt i) noexcept;
		Jonk(const JonkUint u) noexcept;
		Jonk(const bool b) noexcept;

		template<typename T, std::enable_if_t<(is_convertible_to_jonk_v<T>), int> = 0>
		Jonk(const T& t) : Jonk(JonkTypeCompat<T>::toJonk(t)) {}

		~Jonk() = default;

		Jonk& operator =(const Jonk&) = default;
		Jonk& operator =(Jonk&&) = default;

		Jonk& operator =(const nullptr_t) noexcept;
		Jonk& operator =(const JonkObject& obj);
		Jonk& operator =(JonkObject&& obj);
		Jonk& operator =(const JonkArray& arr);
		Jonk& operator =(JonkArray&& arr) noexcept;
		//Jonk& operator =(const JonkString::value_type* str);
		Jonk& operator =(const JonkString& str);
		Jonk& operator =(JonkString&& str) noexcept;
		Jonk& operator =(const JonkFloat f) noexcept;
		Jonk& operator =(const JonkInt i) noexcept;
		Jonk& operator =(const JonkUint u) noexcept;
		Jonk& operator =(const bool b) noexcept;

		template<typename T, std::enable_if_t<is_convertible_to_jonk_v<T>, int> = 0>
		Jonk& operator =(const T& t) {
			*this = JonkTypeCompat<T>::toJonk(t);
			return *this;
		}

		[[nodiscard]] inline bool isNull() const noexcept { return is<JonkType::Null>(); }
		[[nodiscard]] inline bool isObject() const noexcept { return is<JonkType::Object>(); }
		[[nodiscard]] inline bool isArray() const noexcept { return is<JonkType::Array>(); }
		[[nodiscard]] inline bool isString() const noexcept { return is<JonkType::String>(); }
		[[nodiscard]] inline bool isFloat() const noexcept { return is<JonkType::Float>(); }
		[[nodiscard]] inline bool isInt() const noexcept { return is<JonkType::Int>(); }
		[[nodiscard]] inline bool isUint() const noexcept { return is<JonkType::Uint>(); }
		[[nodiscard]] inline bool isIntegral() const noexcept { return isInt() || isUint(); }
		[[nodiscard]] inline bool isNumber() const noexcept { return isFloat() || isIntegral(); }
		[[nodiscard]] inline bool isBool() const noexcept { return is<JonkType::Bool>(); }
		[[nodiscard]] bool is(const JonkType jonkType) const noexcept;
		template<JonkType jonkType>
		[[nodiscard]] inline bool is() const noexcept {
			if constexpr (jonkType == JonkType::Null) return std::holds_alternative<std::monostate>(data);
			else return std::holds_alternative<jonk_type<jonkType>>(data);
		}
		template<typename T, std::enable_if_t<is_testable_jonk_type_v<T> || is_jonk_type_v<T>, int> = 0>
		[[nodiscard]] bool is() const noexcept;

		[[nodiscard]] inline JonkType getJonkType() const noexcept {
			return JonkType { data.index() };
		}

		[[nodiscard]] JonkObject& asObject();
		[[nodiscard]] const JonkObject& asObject() const;
		[[nodiscard]] JonkArray& asArray();
		[[nodiscard]] const JonkArray& asArray() const;
		[[nodiscard]] JonkString& asString();
		[[nodiscard]] const JonkString& asString() const;
		[[nodiscard]] JonkFloat& asFloat();
		[[nodiscard]] const JonkFloat& asFloat() const;
		[[nodiscard]] JonkInt& asInt();
		[[nodiscard]] const JonkInt& asInt() const;
		[[nodiscard]] JonkUint& asUint();
		[[nodiscard]] const JonkUint& asUint() const;
		[[nodiscard]] bool& asBool();
		[[nodiscard]] const bool& asBool() const;
		template<JonkType jonkType>
		[[nodiscard]] inline jonk_type_ref<jonkType> as() {
			if constexpr (jonkType == JonkType::Null) return nullptr;
			else if constexpr (jonkType == JonkType::Object) return asObject();
			else if constexpr (jonkType == JonkType::Array) return asArray();
			else if constexpr (jonkType == JonkType::String) return asString();
			else if constexpr (jonkType == JonkType::Float) return asFloat();
			else if constexpr (jonkType == JonkType::Int) return asInt();
			else if constexpr (jonkType == JonkType::Uint) return asUint();
			else if constexpr (jonkType == JonkType::Bool) return asBool();
			else throw bad_jonk_cast();
		}
		template<JonkType jonkType>
		[[nodiscard]] inline jonk_type_const_ref<jonkType> as() const {
			if constexpr (jonkType == JonkType::Null) return nullptr;
			else if constexpr (jonkType == JonkType::Object) return asObject();
			else if constexpr (jonkType == JonkType::Array) return asArray();
			else if constexpr (jonkType == JonkType::String) return asString();
			else if constexpr (jonkType == JonkType::Float) return asFloat();
			else if constexpr (jonkType == JonkType::Int) return asInt();
			else if constexpr (jonkType == JonkType::Uint) return asUint();
			else if constexpr (jonkType == JonkType::Bool) return asBool();
			else throw bad_jonk_cast();
		}

		[[nodiscard]] JonkObject getObject() const;
		[[nodiscard]] JonkArray getArray() const;
		[[nodiscard]] JonkString getString() const;
		[[nodiscard]] JonkStringView getStringView() const;
		[[nodiscard]] JonkFloat getFloat() const;
		[[nodiscard]] JonkInt getInt() const;
		[[nodiscard]] JonkUint getUint() const;
		[[nodiscard]] bool getBool() const;
		template<JonkType jonkType>
		[[nodiscard]] inline jonk_type<jonkType> get() const {
			if constexpr (jonkType == JonkType::Null) return nullptr;
			else if constexpr (jonkType == JonkType::Object) return getObject();
			else if constexpr (jonkType == JonkType::Array) return getArray();
			else if constexpr (jonkType == JonkType::String) return getString();
			else if constexpr (jonkType == JonkType::Float) return getFloat();
			else if constexpr (jonkType == JonkType::Int) return getInt();
			else if constexpr (jonkType == JonkType::Uint) return getUint();
			else if constexpr (jonkType == JonkType::Bool) return getBool();
		}
		template<typename T, std::enable_if_t<is_convertible_from_jonk_v<T> || is_jonk_type_v<T>, int> = 0>
		[[nodiscard]] inline T get() const;

		template<typename T, std::enable_if_t<(is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>, int> = 0>
		[[nodiscard]] inline T getOrDefault(const T& dflt) const noexcept {
			return is<T>() ? get<T>() : dflt;
		}
		template<typename T, std::enable_if_t<((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>) && std::is_default_constructible_v<T>, int> = 0>
		[[nodiscard]] inline T getOrDefault() const noexcept(std::is_nothrow_default_constructible_v<T>) {
			return is<T>() ? get<T>() : T();
		}
		template<typename T, std::enable_if_t<(((is_testable_jonk_type_v<T>&& is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>) && !eng::is_optional_v<T> && !std::is_same_v<T, nullptr_t> && !std::is_same_v<T, std::nullopt_t>), int> = 0>
		[[nodiscard]] inline std::optional<T> getOptional() const noexcept {
			return is<T>() ? std::make_optional<T>(get<T>()) : std::nullopt;
		}
		

		// TODO: visit function???


		template<typename T, typename... Args>
		inline T& emplace(Args&&... args);
		template<typename T, typename U, typename... Args>
		inline T& emplace(std::initializer_list<U> initList, Args&&... args);
		template<JonkType jonkType, typename... Args>
		jonk_type_ref<jonkType> emplace(Args&&... args);
		template<JonkType jonkType, typename U, typename... Args>
		jonk_type_ref<jonkType> emplace(std::initializer_list<U> initList, Args&&... args);

		size_t size() const;

		[[nodiscard]] inline bool operator ==(const Jonk& b) const noexcept { return data == b.data; }
		[[nodiscard]] inline bool operator !=(const Jonk& b) const noexcept { return data != b.data; }

		operator nullptr_t() const;
		[[nodiscard]] operator JonkObject() const;
		[[nodiscard]] operator JonkArray() const;
		[[nodiscard]] operator JonkString() const;
		[[nodiscard]] operator JonkStringView() const;
		[[nodiscard]] operator JonkFloat() const;
		[[nodiscard]] operator JonkInt() const;
		[[nodiscard]] operator JonkUint() const;
		[[nodiscard]] operator bool() const noexcept;

		/*operator JonkObject&();
		operator const JonkObject&() const;
		operator JonkArray&();
		operator const JonkArray&() const;
		operator JonkString&();
		operator const JonkString&() const;
		operator JonkFloat&();
		operator const JonkFloat&() const;
		operator JonkInt&();
		operator const JonkInt&() const;
		operator JonkUint&();
		operator const JonkUint&() const;
		operator bool&();
		operator const bool&() const;*/

		template<typename T, std::enable_if_t<is_convertible_from_jonk_v<T>, int> = 0>
		inline operator T() const { return JonkTypeCompat<T>::fromJonk(*this); }


		std::string toJonkString(const size_t indentAmount = 2, const char indentChar = ' ', const size_t indentLevel = 0) const;

		friend std::ostream& operator <<(std::ostream&, const Jonk&);

	};

	[[nodiscard]] inline bool operator ==(const nullptr_t, const Jonk& jonk) noexcept { return jonk.isNull(); }
	[[nodiscard]] inline bool operator ==(const Jonk& jonk, const nullptr_t) noexcept { return jonk.isNull(); }
	[[nodiscard]] inline bool operator !=(const nullptr_t, const Jonk& jonk) noexcept { return !jonk.isNull(); }
	[[nodiscard]] inline bool operator !=(const Jonk& jonk, const nullptr_t) noexcept { return !jonk.isNull(); }


	template<typename T, std::enable_if_t<(is_testable_jonk_type_v<T> || is_jonk_type_v<T>), int>>
	[[nodiscard]] inline bool Jonk::is() const noexcept { return JonkTypeCompat<T>::jonkIs(*this); }
	template<> [[nodiscard]] inline bool Jonk::is<nullptr_t, 0>() const noexcept { return is<JonkType::Null>(); }
	template<> [[nodiscard]] inline bool Jonk::is<JonkObject, 0>() const noexcept { return is<JonkType::Object>(); }
	template<> [[nodiscard]] inline bool Jonk::is<JonkArray, 0>() const noexcept { return is<JonkType::Array>(); }
	template<> [[nodiscard]] inline bool Jonk::is<JonkString, 0>() const noexcept { return is<JonkType::String>(); }
	//template<> [[nodiscard]] inline bool Jonk::is<JonkFloat, 0>() const noexcept { return is<JonkType::Float>(); }
	//template<> [[nodiscard]] inline bool Jonk::is<JonkInt, 0>() const noexcept { return is<JonkType::Int>(); }
	//template<> [[nodiscard]] inline bool Jonk::is<JonkUint, 0>() const noexcept { return is<JonkType::Uint>(); }
	template<> [[nodiscard]] inline bool Jonk::is<bool, 0>() const noexcept { return is<JonkType::Bool>(); }

	template<typename T, std::enable_if_t<(is_convertible_from_jonk_v<T> || is_jonk_type_v<T>), int>>
	[[nodiscard]] inline T Jonk::get() const { return JonkTypeCompat<T>::fromJonk(*this); }
	template<> [[nodiscard]] inline nullptr_t Jonk::get<nullptr_t>() const { return get<JonkType::Null>(); }
	template<> [[nodiscard]] inline JonkObject Jonk::get<JonkObject>() const { return get<JonkType::Object>(); }
	template<> [[nodiscard]] inline JonkArray Jonk::get<JonkArray>() const { return get<JonkType::Array>(); }
	template<> [[nodiscard]] inline JonkString Jonk::get<JonkString>() const { return get<JonkType::String>(); }
	//template<> [[nodiscard]] inline JonkFloat Jonk::get<JonkFloat>() const { return get<JonkType::Float>(); }
	//template<> [[nodiscard]] inline JonkInt Jonk::get<JonkInt>() const { return get<JonkType::Int>(); }
	//template<> [[nodiscard]] inline JonkUint Jonk::get<JonkUint>() const { return get<JonkType::Uint>(); }
	template<> [[nodiscard]] inline bool Jonk::get<bool>() const { return get<JonkType::Bool>(); }

	template<typename T, typename... Args>
	inline T& Jonk::emplace(Args&&... args) {
		return data.emplace<T>(std::forward<Args>(args)...);
	}
	template<typename T, typename U, typename... Args>
	inline T& Jonk::emplace(std::initializer_list<U> initList, Args&&... args) {
		return data.emplace(initList, std::forward<Args>(args)...);
	}
	template<JonkType jonkType, typename... Args>
	jonk_type_ref<jonkType> Jonk::emplace(Args&&... args) {
		if constexpr (jonkType == JonkType::Null) {
			data.emplace<std::monostate>();
			return nullptr;
		} else {
			return data.emplace<jonk_type<jonkType>>(std::forward<Args>(args)...);
		}
	}
	template<JonkType jonkType, typename U, typename... Args>
	jonk_type_ref<jonkType> Jonk::emplace(std::initializer_list<U> initList, Args&&... args) {
		if constexpr (jonkType == JonkType::Null) {
			data.emplace<std::monostate>();
			return nullptr;
		} else {
			return data.emplace<jonk_type<jonkType>>(initList, std::forward<Args>(args)...);
		}
	}

	template<>
	struct JonkTypeCompat<std::nullopt_t> {
		static Jonk toJonk(const std::nullopt_t&) { return Jonk(nullptr); }
		static std::nullopt_t fromJonk(const Jonk& jonk) {
			if (jonk.isNull()) return std::nullopt;
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept { return jonk.isNull(); }
	};
	template<typename T>
	struct JonkTypeCompat<std::optional<T>, std::enable_if_t<(!std::is_same_v<T, nullptr_t> && !std::is_same_v<T, std::nullopt_t>)>> {
		using Opt = std::optional<T>;
		template<typename U = T, std::enable_if_t<(is_convertible_from_jonk_v<U> || is_jonk_type_v<U>), int> = 0>
		static Jonk toJonk(const Opt& opt) {
			return (opt) ? *opt : nullptr;
		}
		template<typename U = T, std::enable_if_t<((is_testable_jonk_type_v<U> && is_convertible_from_jonk_v<U>) || is_jonk_type_v<U>), int> = 0>
		static Opt fromJonk(const Jonk& jonk) {
			if (jonk.is<U>()) return jonk.get<U>();
			if (jonk.isNull()) return std::nullopt;
			throw bad_jonk_cast();
		}
		template<typename U = T, std::enable_if_t<(is_testable_jonk_type_v<U> || is_jonk_type_v<U>), int> = 0>
		static bool jonkIs(const Jonk& jonk) noexcept {
			return jonk.is<U>() || jonk.isNull();
		}
	};

	template<>
	struct JonkTypeCompat<JonkStringView> {
		//static Jonk toJonk(const JonkStringView& sv) { return JonkString(sv); }
		static JonkStringView fromJonk(const Jonk& jonk) { return jonk.getStringView(); }
		static bool jonkIs(const Jonk& jonk) noexcept { return jonk.isString(); }
	};

	template<typename T>
	struct JonkTypeCompat<T, std::enable_if_t<std::is_floating_point_v<T>>> {
		static Jonk toJonk(const T& f) {
			return static_cast<JonkFloat>(f);
		}
		static T fromJonk(const Jonk& jonk) {
			if (jonk.isFloat()) return static_cast<T>(jonk.asFloat());
			if (jonk.isInt()) return static_cast<T>(jonk.asInt());
			if (jonk.isUint()) return static_cast<T>(jonk.asUint());
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept { return jonk.isNumber(); }
	};
	template<typename T>
	struct JonkTypeCompat<T, std::enable_if_t<(std::is_integral_v<T> && !std::is_same_v<T, bool>)>> {
		static Jonk toJonk(const T& i) {
			if constexpr (std::is_signed_v<T>)
				return static_cast<JonkInt>(i);
			else
				return static_cast<JonkUint>(i);
		}
		static T fromJonk(const Jonk& jonk) {
			if constexpr (std::is_signed_v<T>) {
				if (jonk.isInt()) return static_cast<T>(jonk.asInt());
			} else {
				if (jonk.isInt() && (jonk.asInt() >= 0)) return static_cast<T>(jonk.asInt());
			}
			if (jonk.isUint()) return static_cast<T>(jonk.asUint());
			throw bad_jonk_cast();
		}
		static bool jonkIs(const Jonk& jonk) noexcept {
			if constexpr (std::is_signed_v<T>)
				return jonk.isInt() && (jonk.asInt() <= std::numeric_limits<T>::max());
			else
				return (jonk.isUint() && (jonk.asUint() <= std::numeric_limits<T>::max()))
					|| (jonk.isInt() && (jonk.asInt() >= 0) && (jonk.asInt() <= std::numeric_limits<T>::max()));
		}
	};

}

// JonkObject member function definitions that rely on Jonk being defined

namespace jonk {

	template<typename T, std::enable_if_t<(is_testable_jonk_type_v<T> || is_jonk_type_v<T>), int>>
	bool JonkObject::hasKey(const std::string_view key) const {
		const auto it = data.find(key);
		return (it != data.end()) && it->second.template is<T>();
	}

	template<JonkType jonkType>
	jonk_type_ref<jonkType> JonkObject::at(const key_type& key) {
		return data.at(key).template as<jonkType>();
	}
	template<JonkType jonkType>
	const jonk_type_ref<jonkType> JonkObject::at(const key_type& key) const {
		return data.at(key).template as<jonkType>();
	}

	template<JonkType jonkType>
	const jonk_type_ref<jonkType> JonkObject::get(const key_type& key) const {
		return data.at(key).template as<jonkType>();
	}
	template<typename T, std::enable_if_t<is_convertible_from_jonk_v<T> || is_jonk_type_v<T>, int>>
	T JonkObject::get(const key_type& key) const {
		return data.at(key).template get<T>();
	}

	template<typename T, std::enable_if_t<((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>), int>>
	T JonkObject::getOrDefault(const std::string_view key, const T& dflt) const noexcept {
		const auto it = data.find(key);
		return ((it != data.end()) && it->second.template is<T>()) ? it->second.template get<T>() : dflt;
	}
	template<typename T, std::enable_if_t<((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>) && std::is_default_constructible_v<T>, int>>
	T JonkObject::getOrDefault(const std::string_view key) const noexcept(std::is_nothrow_default_constructible_v<T>) {
		const auto it = data.find(key);
		return ((it != data.end()) && it->second.template is<T>()) ? it->second.template get<T>() : T();
	}
	
	template<typename T, std::enable_if_t<(((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>) && !eng::is_optional_v<T> && !std::is_same_v<T, nullptr_t> && !std::is_same_v<T, std::nullopt_t>), int>>
	std::optional<T> JonkObject::getOptional(const std::string_view key) const noexcept {
		const auto it = data.find(key);
		return ((it != data.end()) && it->second.template is<T>()) ? std::make_optional<T>(it->second.template get<T>()) : std::nullopt;
	}

}

// Jonk compat for common types
#include "MiscJonkCompat.h"
