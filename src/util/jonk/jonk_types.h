#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <variant>
#include <iostream>

namespace jonk {

	using std::nullptr_t;

	class Jonk;
	class JonkObject;

	using JonkInt = int64_t;
	using JonkUint = uint64_t;
	using JonkFloat = double;
	using JonkString = std::string;
	using JonkStringView = std::string_view;
	using JonkArray = std::vector<Jonk>;


	enum class JonkType : size_t {
		Null,
		Object,
		Array,
		String,
		Float,
		Int,
		Uint,
		Bool,
	};
	constexpr JonkType Null = JonkType::Null;
	constexpr JonkType Object = JonkType::Object;
	constexpr JonkType Array = JonkType::Array;
	constexpr JonkType String = JonkType::String;
	constexpr JonkType Float = JonkType::Float;
	constexpr JonkType Int = JonkType::Int;
	constexpr JonkType Uint = JonkType::Uint;
	constexpr JonkType Bool = JonkType::Bool;

	std::ostream& operator <<(std::ostream&, const JonkType&);


	template<JonkType jonkType> struct get_jonk_type;
	template<> struct get_jonk_type<JonkType::Null> {
		using type = nullptr_t;
		using reference = nullptr_t;
		using const_reference = const nullptr_t;
		using pointer = nullptr_t;
	};
	template<> struct get_jonk_type<JonkType::Object> {
		using type = JonkObject;
		using reference = JonkObject&;
		using const_reference = const JonkObject&;
		using pointer = JonkObject*;
	};
	template<> struct get_jonk_type<JonkType::Array> {
		using type = JonkArray;
		using reference = JonkArray&;
		using const_reference = const JonkArray&;
		using pointer = JonkArray*;
	};
	template<> struct get_jonk_type<JonkType::String> {
		using type = JonkString;
		using reference = JonkString&;
		using const_reference = const JonkString&;
		using pointer = JonkString*;
	};
	template<> struct get_jonk_type<JonkType::Float> {
		using type = JonkFloat;
		using reference = JonkFloat&;
		using const_reference = const JonkFloat&;
		using pointer = JonkFloat*;
	};
	template<> struct get_jonk_type<JonkType::Int> {
		using type = JonkInt;
		using reference = JonkInt&;
		using const_reference = const JonkInt&;
		using pointer = JonkInt*;
	};
	template<> struct get_jonk_type<JonkType::Uint> {
		using type = JonkUint;
		using reference = JonkUint&;
		using const_reference = const JonkUint&;
		using pointer = JonkUint*;
	};
	template<> struct get_jonk_type<JonkType::Bool> {
		using type = bool;
		using reference = bool&;
		using const_reference = const bool&;
		using pointer = bool*;
	};
	template<JonkType jonkType> using jonk_type = typename get_jonk_type<jonkType>::type;
	template<JonkType jonkType> using jonk_type_ref = typename get_jonk_type<jonkType>::reference;
	template<JonkType jonkType> using jonk_type_const_ref = typename get_jonk_type<jonkType>::const_reference;
	template<JonkType jonkType> using jonk_type_ptr = typename get_jonk_type<jonkType>::pointer;

	template<typename T> struct get_jonk_type_enum;
	template<> struct get_jonk_type_enum<nullptr_t> { static constexpr JonkType type_enum = JonkType::Null; };
	template<> struct get_jonk_type_enum<std::monostate> { static constexpr JonkType type_enum = JonkType::Null; };
	template<> struct get_jonk_type_enum<JonkObject> { static constexpr JonkType type_enum = JonkType::Object; };
	template<> struct get_jonk_type_enum<JonkArray> { static constexpr JonkType type_enum = JonkType::Array; };
	template<> struct get_jonk_type_enum<JonkString> { static constexpr JonkType type_enum = JonkType::String; };
	template<> struct get_jonk_type_enum<JonkFloat> { static constexpr JonkType type_enum = JonkType::Float; };
	template<> struct get_jonk_type_enum<JonkInt> { static constexpr JonkType type_enum = JonkType::Int; };
	template<> struct get_jonk_type_enum<JonkUint> { static constexpr JonkType type_enum = JonkType::Uint; };
	template<> struct get_jonk_type_enum<bool> { static constexpr JonkType type_enum = JonkType::Bool; };
	template<typename T> inline constexpr JonkType jonk_type_enum = get_jonk_type_enum<T>::type_enum;

	template<typename T> struct is_jonk_type : std::false_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Null>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Object>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Array>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::String>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Float>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Int>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Uint>> : std::true_type {};
	template<> struct is_jonk_type<jonk_type<JonkType::Bool>> : std::true_type {};
	template<typename T> inline constexpr bool is_jonk_type_v = is_jonk_type<T>::value;

}