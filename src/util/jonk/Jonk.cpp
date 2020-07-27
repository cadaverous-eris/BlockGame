#include "Jonk.h"

#include <sstream>
#include <algorithm>

#include "parsing_utils.h"

namespace jonk {

	Jonk::Jonk(const nullptr_t) noexcept : data(std::monostate()) {}
	Jonk::Jonk(const JonkObject& obj) noexcept : data(obj) {}
	Jonk::Jonk(JonkObject&& obj) noexcept : data(std::move(obj)) {}
	Jonk::Jonk(const JonkArray& arr) noexcept : data(arr) {}
	Jonk::Jonk(JonkArray&& arr) noexcept : data(std::move(arr)) {}
	//Jonk::Jonk(const JonkString::value_type* str) : data(JonkString(str)) {}
	Jonk::Jonk(const JonkString& str) noexcept : data(str) {}
	Jonk::Jonk(JonkString&& str) noexcept : data(std::move(str)) {}
	Jonk::Jonk(const JonkFloat f) noexcept : data(f) {}
	Jonk::Jonk(const JonkInt i) noexcept : data(i) {}
	Jonk::Jonk(const JonkUint u) noexcept : data(u) {}
	Jonk::Jonk(const bool b) noexcept : data(b) {}

	Jonk& Jonk::operator =(const nullptr_t) noexcept {
		data = std::monostate();
		return *this;
	}
	Jonk& Jonk::operator =(const JonkObject& obj) {
		data = obj;
		return *this;
	}
	Jonk& Jonk::operator =(JonkObject&& obj) {
		data = std::move(obj);
		return *this;
	}
	Jonk& Jonk::operator =(const JonkArray& arr) {
		data = arr;
		return *this;
	}
	Jonk& Jonk::operator =(JonkArray&& arr) noexcept {
		data = std::move(arr);
		return *this;
	}
	/*Jonk& Jonk::operator =(const JonkString::value_type* str) {
		data = JonkString(str);
		return *this;
	}*/
	Jonk& Jonk::operator =(const JonkString& str) {
		data = str;
		return *this;
	}
	Jonk& Jonk::operator =(JonkString&& str) noexcept {
		data = std::move(str);
		return *this;
	}
	Jonk& Jonk::operator =(const JonkFloat f) noexcept {
		data = f;
		return *this;
	}
	Jonk& Jonk::operator =(const JonkInt i) noexcept {
		data = i;
		return *this;
	}
	Jonk& Jonk::operator =(const JonkUint u) noexcept {
		data = u;
		return *this;
	}
	Jonk& Jonk::operator =(const bool b) noexcept {
		data = b;
		return *this;
	}
	
	bool Jonk::is(const JonkType jonkType) const noexcept {
		switch (jonkType) {
		case JonkType::Null: return isNull();
		case JonkType::Object: return isObject();
		case JonkType::Array: return isArray();
		case JonkType::String: return isString();
		case JonkType::Float: return isFloat();
		case JonkType::Int: return isInt();
		case JonkType::Uint: return isUint();
		case JonkType::Bool: return isBool();
		default: return false;
		}
	}

	JonkObject& Jonk::asObject() {
		if (isObject()) return std::get<JonkObject>(data);
		throw bad_jonk_cast();
	}
	const JonkObject& Jonk::asObject() const {
		if (isObject()) return std::get<JonkObject>(data);
		throw bad_jonk_cast();
	}
	JonkArray& Jonk::asArray() {
		if (isArray()) return std::get<JonkArray>(data);
		throw bad_jonk_cast();
	}
	const JonkArray& Jonk::asArray() const {
		if (isArray()) return std::get<JonkArray>(data);
		throw bad_jonk_cast();
	}
	JonkString& Jonk::asString() {
		if (isString()) return std::get<JonkString>(data);
		throw bad_jonk_cast();
	}
	const JonkString& Jonk::asString() const {
		if (isString()) return std::get<JonkString>(data);
		throw bad_jonk_cast();
	}
	JonkFloat& Jonk::asFloat() {
		if (isFloat()) return std::get<JonkFloat>(data);
		throw bad_jonk_cast();
	}
	const JonkFloat& Jonk::asFloat() const {
		if (isFloat()) return std::get<JonkFloat>(data);
		throw bad_jonk_cast();
	}
	JonkInt& Jonk::asInt() {
		if (isInt()) return std::get<JonkInt>(data);
		throw bad_jonk_cast();
	}
	const JonkInt& Jonk::asInt() const {
		if (isInt()) return std::get<JonkInt>(data);
		throw bad_jonk_cast();
	}
	JonkUint& Jonk::asUint() {
		if (isUint()) return std::get<JonkUint>(data);
		throw bad_jonk_cast();
	}
	const JonkUint& Jonk::asUint() const {
		if (isUint()) return std::get<JonkUint>(data);
		throw bad_jonk_cast();
	}
	bool& Jonk::asBool() {
		if (isBool()) return std::get<bool>(data);
		throw bad_jonk_cast();
	}
	const bool& Jonk::asBool() const {
		if (isBool()) return std::get<bool>(data);
		throw bad_jonk_cast();
	}

	JonkObject Jonk::getObject() const {
		if (isObject()) return std::get<JonkObject>(data);
		throw bad_jonk_cast();
	}
	JonkArray Jonk::getArray() const {
		if (isArray()) return std::get<JonkArray>(data);
		throw bad_jonk_cast();
	}
	JonkString Jonk::getString() const {
		if (isString()) return std::get<JonkString>(data);
		throw bad_jonk_cast();
	}
	JonkStringView Jonk::getStringView() const {
		if (isString()) return std::get<JonkString>(data);
		throw bad_jonk_cast();
	}
	JonkFloat Jonk::getFloat() const {
		if (isFloat()) return std::get<JonkFloat>(data);
		if (isInt()) return static_cast<JonkFloat>(std::get<JonkInt>(data));
		if (isUint()) return static_cast<JonkFloat>(std::get<JonkUint>(data));
		throw bad_jonk_cast();
	}
	JonkInt Jonk::getInt() const {
		if (isInt()) return std::get<JonkInt>(data);
		if (isUint()) return static_cast<JonkInt>(std::get<JonkUint>(data));
		throw bad_jonk_cast();
	}
	JonkUint Jonk::getUint() const {
		if (isUint()) return std::get<JonkUint>(data);
		if (isInt() && (asInt() >= 0)) return static_cast<JonkUint>(std::get<JonkInt>(data));
		throw bad_jonk_cast();
	}
	bool Jonk::getBool() const {
		if (isBool()) return std::get<bool>(data);
		if (isFloat()) return std::get<JonkFloat>(data);
		if (isInt()) return std::get<JonkInt>(data);
		if (isUint()) return std::get<JonkUint>(data);
		throw bad_jonk_cast();
	}

	size_t Jonk::size() const {
		if (isObject()) return asObject().size();
		if (isArray()) return asArray().size();
		if (isString()) return asString().size();
		if (isNull()) return 0;
		return 1;
	}

	Jonk::operator nullptr_t() const {
		if (isNull()) return nullptr;
		throw bad_jonk_cast();
	}
	Jonk::operator JonkObject() const { return asObject(); }
	Jonk::operator JonkArray() const { return asArray(); }
	Jonk::operator JonkString() const { return asString(); }
	Jonk::operator JonkStringView() const { return getStringView(); }
	Jonk::operator JonkFloat() const { return getFloat(); }
	Jonk::operator JonkInt() const { return getInt(); }
	Jonk::operator JonkUint() const { return getUint(); }
	Jonk::operator bool() const noexcept {
		if (isBool()) return std::get<bool>(data);
		if (isNull()) return false;
		if (isFloat()) return std::get<JonkFloat>(data);
		if (isInt()) return std::get<JonkInt>(data);
		if (isUint()) return std::get<JonkUint>(data);
		if (isString()) return !std::get<JonkString>(data).empty();
		return true;
	}

	/*Jonk::operator JonkObject&() { return asObject(); }
	Jonk::operator const JonkObject&() const { return asObject(); }
	Jonk::operator JonkArray&() { return asArray(); }
	Jonk::operator const JonkArray&() const { return asArray(); }
	Jonk::operator JonkString&() { return asString(); }
	Jonk::operator const JonkString&() const { return asString(); }
	Jonk::operator JonkFloat&() { return asFloat(); }
	Jonk::operator const JonkFloat&() const { return asFloat(); }
	Jonk::operator JonkInt&() { return asInt(); }
	Jonk::operator const JonkInt&() const { return asInt(); }
	Jonk::operator JonkUint&() { return asUint(); }
	Jonk::operator const JonkUint&() const { return asUint(); }
	Jonk::operator bool&() { return asBool(); }
	Jonk::operator const bool&() const { return asBool(); }*/

	static std::string stringifyJonkString(const JonkStringView str) {
		std::string result {};
		result.reserve(str.size() + 2);
		result += '"';
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				result += '\\';
				result += '"';
			} else if (str[i] == '\'') {
				result += '\\';
				result += '\'';
			} else if (str[i] == '`') {
				result += '\\';
				result += '`';
			} else if (str[i] == '\\') {
				result += '\\';
				result += '\\';
			} else if (str[i] == '\a') {
				result += '\\';
				result += 'a';
			} else if (str[i] == '\b') {
				result += '\\';
				result += 'b';
			} else if (str[i] == '\t') {
				result += '\\';
				result += 't';
			} else if (str[i] == '\n') {
				result += '\\';
				result += 'n';
			} else if (str[i] == '\v') {
				result += '\\';
				result += 'v';
			} else if (str[i] == '\f') {
				result += '\\';
				result += 'f';
			} else if (str[i] == '\r') {
				result += '\\';
				result += 'r';
			} else if (str[i] == '\0') {
				result += '\\';
				result += '0';
			} else {
				result += str[i];
			}
		}
		return result + '"';
	}

	static std::string stringifyJonkArray(const JonkArray& arr, const size_t indentAmount, const char indentChar, const size_t indentLevel) {
		const auto innerIndentLevel = indentLevel + 1;
		const bool shouldInline = (arr.size() <= 4) && std::all_of(arr.begin(), arr.end(), [](const Jonk& j) noexcept {
			return j.isNull() || j.isBool() || j.isNumber();
		});

		std::ostringstream sstr;
		sstr.put('[');
		for (auto& val : arr) {
			if (shouldInline) {
				sstr.put(' ');
			} else if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < innerIndentLevel * indentAmount; i++) sstr.put(indentChar);
			}
			sstr << val.toJonkString(indentAmount, indentChar, innerIndentLevel) << ',';
		}
		if (!arr.empty()) {
			if (shouldInline) {
				sstr.put(' ');
			} else if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < indentLevel * indentAmount; i++) sstr.put(indentChar);
			}
		}
		sstr.put(']');
		return sstr.str();
	}

	std::string Jonk::toJonkString(const size_t indentAmount, const char indentChar, const size_t indentLevel) const {
		using namespace std::string_view_literals;
		if (isNull()) {
			return std::string(parsing::null_string);
		} else if (isBool()) {
			const bool val = as<JonkType::Bool>();
			if (val) return std::string(parsing::true_string);
			else return std::string(parsing::false_string);
		} else if (isFloat()) {
			const JonkFloat val = as<JonkType::Float>();
			if (val != val) return std::string(parsing::nan_string);
			if (std::isinf(val)) {
				if (val < 0) return '-' + std::string(parsing::infinity_string);
				else return std::string(parsing::infinity_string);
			}
			// TODO: don't use string streams to convert numbers to strings
			std::ostringstream sstr {};
			sstr.precision(12);
			sstr.flags(std::ios::showpoint);
			sstr << val;
			std::string s = sstr.str();
			size_t i = s.length() - 1;
			for (; i >= 1; i--) {
				if ((s[i] != '0') || (s[i - 1] == '.')) break;
			}
			s = s.substr(0, i + 1);
			if (s.ends_with('.')) s += '0';
			return s;
		} else if (isUint()) {
			const JonkUint val = as<JonkType::Uint>();
			// TODO: don't use string streams to convert numbers to strings
			std::ostringstream sstr {};
			sstr << "0x"sv;
			sstr.flags(std::ios::hex | std::ios::uppercase);
			sstr << val;
			return sstr.str();
		} else if (isInt()) {
			const JonkInt val = as<JonkType::Int>();
			// TODO: don't use string streams to convert numbers to strings
			std::ostringstream sstr {};
			sstr << val;
			return sstr.str();
		} else if (isString()) {
			return stringifyJonkString(as<JonkType::String>());
		} else if (isObject()) {
			const JonkObject& val = as<JonkType::Object>();
			return val.toJonkString(indentAmount, indentChar, indentLevel);
		} else if (isArray()) {
			const JonkArray& val = as<JonkType::Array>();
			return stringifyJonkArray(val, indentAmount, indentChar, indentLevel);
		}
		return "";
	}

	std::ostream& operator <<(std::ostream& os, const Jonk& j) {
		return os << j.toJonkString();
	}

}