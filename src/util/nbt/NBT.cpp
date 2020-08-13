#include "NBT.h"

#include <utility>
#include <sstream>
#include <algorithm>

#include <fmt/core.h>

#include "parsing_utils.h"

namespace nbt {

    NBT::NBT(const nbt_byte b) noexcept : value(b) {}
    NBT::NBT(const nbt_short s) noexcept : value(s) {}
    NBT::NBT(const nbt_int i) noexcept : value(i) {}
    NBT::NBT(const nbt_long l) noexcept : value(l) {}
    NBT::NBT(const nbt_float f) noexcept : value(f) {}
    NBT::NBT(const nbt_double d) noexcept : value(d) {}
    NBT::NBT(const nbt_byte_array& a) noexcept : value(a) {}
    NBT::NBT(nbt_byte_array&& a) noexcept : value(std::move(a)) {}
    NBT::NBT(const nbt_string& s) noexcept : value(s) {}
    NBT::NBT(nbt_string&& s) noexcept : value(std::move(s)) {}
    NBT::NBT(const nbt_list& l) noexcept : value(l) {}
    NBT::NBT(nbt_list&& l) noexcept : value(std::move(l)) {}
    NBT::NBT(const nbt_compound& c) noexcept : value(c) {}
    NBT::NBT(nbt_compound&& c) noexcept : value(std::move(c)) {}
    NBT::NBT(const nbt_int_array& a) noexcept : value(a) {}
    NBT::NBT(nbt_int_array&& a) noexcept : value(std::move(a)) {}
    NBT::NBT(const nbt_long_array& a) noexcept : value(a) {}
    NBT::NBT(nbt_long_array&& a) noexcept : value(std::move(a)) {}

    NBT& NBT::operator =(const nbt_byte b) noexcept {
		value = b;//value.emplace<nbt_byte>(b);
		return *this;
	}
	NBT& NBT::operator =(const nbt_short s) noexcept {
		value = s;//value.emplace<nbt_short>(s);
		return *this;
	}
	NBT& NBT::operator =(const nbt_int i) noexcept {
		value = i;//value.emplace<nbt_int>(i);
		return *this;
	}
	NBT& NBT::operator =(const nbt_long l) noexcept {
		value = l;//value.emplace<nbt_long>(l);
		return *this;
	}
	NBT& NBT::operator =(const nbt_float f) noexcept {
		value = f;//value.emplace<nbt_float>(f);
		return *this;
	}
	NBT& NBT::operator =(const nbt_double d) noexcept {
		value = d;//value.emplace<nbt_double>(d);
		return *this;
	}
	NBT& NBT::operator =(const nbt_byte_array& a) noexcept {
		value = a;//value.emplace<nbt_byte_array>(a);
		return *this;
	}
	NBT& NBT::operator =(nbt_byte_array&& a) noexcept {
		value = std::move(a);//value.emplace<nbt_byte_array>(std::move(a));
		return *this;
	}
	NBT& NBT::operator =(const nbt_string& s) noexcept {
		value = s;//value.emplace<nbt_string>(s);
		return *this;
	}
	NBT& NBT::operator =(nbt_string&& s) noexcept {
		value = std::move(s);//value.emplace<nbt_string>(std::move(s));
		return *this;
	}
	NBT& NBT::operator =(const nbt_list& l) noexcept {
		value = l;//value.emplace<nbt_list>(l);
		return *this;
	}
	NBT& NBT::operator =(nbt_list&& l) noexcept {
		value = std::move(l);//value.emplace<nbt_list>(std::move(l));
		return *this;
	}
	NBT& NBT::operator =(const nbt_compound& c) noexcept {
		value = c;//value.emplace<nbt_compound>(c);
		return *this;
	}
	NBT& NBT::operator =(nbt_compound&& c) noexcept {
		value = std::move(c);//value.emplace<nbt_compound>(std::move(c));
		return *this;
	}
	NBT& NBT::operator =(const nbt_int_array& a) noexcept {
		value = a;//value.emplace<nbt_int_array>(a);
		return *this;
	}
	NBT& NBT::operator =(nbt_int_array&& a) noexcept {
		value = std::move(a);//value.emplace<nbt_int_array>(std::move(a));
		return *this;
	}
	NBT& NBT::operator =(const nbt_long_array& a) noexcept {
		value = a;//value.emplace<nbt_long_array>(a);
		return *this;
	}
	NBT& NBT::operator =(nbt_long_array&& a) noexcept {
		value = std::move(a);//value.emplace<nbt_long_array>(std::move(a));
		return *this;
	}


    bool NBT::is(const TagType tagType) const noexcept {
		switch (tagType) {
            case TagByte: return isByte();
            case TagShort: return isShort();
            case TagInt: return isInt();
            case TagLong: return isLong();
            case TagFloat: return isFloat();
            case TagDouble: return isDouble();
            case TagByteArray: return isByteArray();
            case TagString: return isString();
            case TagList: return isList();
            case TagCompound: return isCompound();
            case TagIntArray: return isIntArray();
            case TagLongArray: return isLongArray();
		    default: return false;
		}
	}


    nbt_byte& NBT::asByte() {
		if (isByte()) return std::get<nbt_byte>(value);
		throw bad_nbt_cast();
	}
	const nbt_byte& NBT::asByte() const {
		if (isByte()) return std::get<nbt_byte>(value);
		throw bad_nbt_cast();
	}
	nbt_short& NBT::asShort() {
		if (isShort()) return std::get<nbt_short>(value);
		throw bad_nbt_cast();
	}
	const nbt_short& NBT::asShort() const {
		if (isShort()) return std::get<nbt_short>(value);
		throw bad_nbt_cast();
	}
	nbt_int& NBT::asInt() {
		if (isInt()) return std::get<nbt_int>(value);
		throw bad_nbt_cast();
	}
	const nbt_int& NBT::asInt() const {
		if (isInt()) return std::get<nbt_int>(value);
		throw bad_nbt_cast();
	}
	nbt_long& NBT::asLong() {
		if (isLong()) return std::get<nbt_long>(value);
		throw bad_nbt_cast();
	}
	const nbt_long& NBT::asLong() const {
		if (isLong()) return std::get<nbt_long>(value);
		throw bad_nbt_cast();
	}
	nbt_float& NBT::asFloat() {
		if (isFloat()) return std::get<nbt_float>(value);
		throw bad_nbt_cast();
	}
	const nbt_float& NBT::asFloat() const {
		if (isFloat()) return std::get<nbt_float>(value);
		throw bad_nbt_cast();
	}
	nbt_double& NBT::asDouble() {
		if (isDouble()) return std::get<nbt_double>(value);
		throw bad_nbt_cast();
	}
	const nbt_double& NBT::asDouble() const {
		if (isDouble()) return std::get<nbt_double>(value);
		throw bad_nbt_cast();
	}
	nbt_byte_array& NBT::asByteArray() {
		if (isByteArray()) return std::get<nbt_byte_array>(value);
		throw bad_nbt_cast();
	}
	const nbt_byte_array& NBT::asByteArray() const {
		if (isByteArray()) return std::get<nbt_byte_array>(value);
		throw bad_nbt_cast();
	}
	nbt_string& NBT::asString() {
		if (isString()) return std::get<nbt_string>(value);
		throw bad_nbt_cast();
	}
	const nbt_string& NBT::asString() const {
		if (isString()) return std::get<nbt_string>(value);
		throw bad_nbt_cast();
	}
	nbt_list& NBT::asList() {
		if (isList()) return std::get<nbt_list>(value);
		throw bad_nbt_cast();
	}
	const nbt_list& NBT::asList() const {
		if (isList()) return std::get<nbt_list>(value);
		throw bad_nbt_cast();
	}
	nbt_compound& NBT::asCompound() {
		if (isCompound()) return std::get<nbt_compound>(value);
		throw bad_nbt_cast();
	}
	const nbt_compound& NBT::asCompound() const {
		if (isCompound()) return std::get<nbt_compound>(value);
		throw bad_nbt_cast();
	}
	nbt_int_array& NBT::asIntArray() {
		if (isIntArray()) return std::get<nbt_int_array>(value);
		throw bad_nbt_cast();
	}
	const nbt_int_array& NBT::asIntArray() const {
		if (isIntArray()) return std::get<nbt_int_array>(value);
		throw bad_nbt_cast();
	}
	nbt_long_array& NBT::asLongArray() {
		if (isLongArray()) return std::get<nbt_long_array>(value);
		throw bad_nbt_cast();
	}
	const nbt_long_array& NBT::asLongArray() const {
		if (isLongArray()) return std::get<nbt_long_array>(value);
		throw bad_nbt_cast();
	}

    nbt_byte NBT::getByte() const {
		if (isByte()) return std::get<nbt_byte>(value);
		throw bad_nbt_cast();
	}
	nbt_short NBT::getShort() const {
		if (isShort()) return std::get<nbt_short>(value);
        if (isByte()) return static_cast<nbt_short>(std::get<nbt_byte>(value));
		throw bad_nbt_cast();
	}
	nbt_int NBT::getInt() const {
		if (isInt()) return std::get<nbt_int>(value);
        if (isByte()) return static_cast<nbt_int>(std::get<nbt_byte>(value));
        if (isShort()) return static_cast<nbt_int>(std::get<nbt_short>(value));
		throw bad_nbt_cast();
	}
	nbt_long NBT::getLong() const {
		if (isLong()) return std::get<nbt_long>(value);
        if (isByte()) return static_cast<nbt_long>(std::get<nbt_byte>(value));
        if (isShort()) return static_cast<nbt_long>(std::get<nbt_short>(value));
        if (isInt()) return static_cast<nbt_long>(std::get<nbt_int>(value));
		throw bad_nbt_cast();
	}
	nbt_float NBT::getFloat() const {
		if (isFloat()) return std::get<nbt_float>(value);
        if (isDouble()) return static_cast<nbt_float>(std::get<nbt_double>(value));
        if (isByte()) return static_cast<nbt_float>(std::get<nbt_byte>(value));
        if (isShort()) return static_cast<nbt_float>(std::get<nbt_short>(value));
        if (isInt()) return static_cast<nbt_float>(std::get<nbt_int>(value));
        if (isLong()) return static_cast<nbt_float>(std::get<nbt_long>(value));
		throw bad_nbt_cast();
	}
	nbt_double NBT::getDouble() const {
		if (isDouble()) return std::get<nbt_double>(value);
        if (isFloat()) return static_cast<nbt_double>(std::get<nbt_float>(value));
        if (isByte()) return static_cast<nbt_double>(std::get<nbt_byte>(value));
        if (isShort()) return static_cast<nbt_double>(std::get<nbt_short>(value));
        if (isInt()) return static_cast<nbt_double>(std::get<nbt_int>(value));
        if (isLong()) return static_cast<nbt_double>(std::get<nbt_long>(value));
		throw bad_nbt_cast();
	}
	nbt_byte_array NBT::getByteArray() const {
		if (isByteArray()) return std::get<nbt_byte_array>(value);
		throw bad_nbt_cast();
	}
	nbt_string NBT::getString() const {
		if (isString()) return std::get<nbt_string>(value);
		throw bad_nbt_cast();
	}
	nbt_list NBT::getList() const {
		if (isList()) return std::get<nbt_list>(value);
		throw bad_nbt_cast();
	}
	nbt_compound NBT::getCompound() const {
		if (isCompound()) return std::get<nbt_compound>(value);
		throw bad_nbt_cast();
	}
	nbt_int_array NBT::getIntArray() const {
		if (isIntArray()) return std::get<nbt_int_array>(value);
		throw bad_nbt_cast();
	}
	nbt_long_array NBT::getLongArray() const {
		if (isLongArray()) return std::get<nbt_long_array>(value);
		throw bad_nbt_cast();
	}

    size_t NBT::size() const {
		if (isByteArray()) return asByteArray().size();
		if (isString()) return asString().size();
		if (isList()) return asList().size();
		if (isCompound()) return asCompound().size();
        if (isIntArray()) return asIntArray().size();
        if (isLongArray()) return asLongArray().size();
		return 1;
	}

    NBT::operator nbt_byte() const { return getByte(); }
	NBT::operator nbt_short() const { return getShort(); }
	NBT::operator nbt_int() const { return getInt(); }
	NBT::operator nbt_long() const { return getLong(); }
	NBT::operator nbt_float() const { return getFloat(); }
	NBT::operator nbt_double() const { return getDouble(); }
	NBT::operator nbt_byte_array() const { return asByteArray(); }
	NBT::operator nbt_string() const { return asString(); }
    NBT::operator nbt_string_view() const { return asString(); }
	NBT::operator nbt_list() const { return asList(); }
	NBT::operator nbt_compound() const { return asCompound(); }
	NBT::operator nbt_int_array() const { return asIntArray(); }
	NBT::operator nbt_long_array() const { return asLongArray(); }


    static std::string stringifyNBTString(const nbt_string_view str) {
		std::string result {};
		result.reserve(str.size() + 2);
		result += '"';
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				result += '\\';
				result += '"';
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

    static std::string stringifyNBTList(const nbt_list& arr, const size_t indentAmount, const char indentChar, const size_t indentLevel) {
		const auto innerIndentLevel = indentLevel + 1;
		const bool shouldInline = ((arr.size() <= 4) && std::all_of(arr.begin(), arr.end(), [](const NBT& n) noexcept { return n.isNumber(); })) || !indentAmount;

		std::ostringstream sstr;
		sstr.put('[');
		for (auto& val : arr) {
			if (shouldInline) {
				sstr.put(' ');
			} else if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < innerIndentLevel * indentAmount; i++) sstr.put(indentChar);
			}
			sstr << val.toSNBT(indentAmount, indentChar, innerIndentLevel) << ',';
		}
		if (!arr.empty()) {
			if (shouldInline) {
				sstr.put(' ');
			} else if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < (indentLevel * indentAmount); i++) sstr.put(indentChar);
			}
		}
		sstr.put(']');
		return sstr.str();
	}

    template<typename T, char typeChar>
    static std::string stringifyNBTTypedArray(const T& arr, const size_t indentAmount, const char indentChar, const size_t indentLevel) {
		const auto innerIndentLevel = indentLevel + 1;
		const bool shouldInline = (arr.size() <= 4) || !indentAmount;

		std::ostringstream sstr;
		sstr << '[' << typeChar << ';';
		for (auto& val : arr) {
			if (shouldInline) {
				sstr.put(' ');
			} else if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < (innerIndentLevel * indentAmount); i++) sstr.put(indentChar);
			}
			sstr << fmt::format("{:d}", val) << ',';
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

    std::string NBT::toSNBT(const size_t indentAmount, const char indentChar, const size_t indentLevel) const {
		using namespace std::string_view_literals;
		if (isByte()) {
            return fmt::format("{:d}b", asByte());
		} else if (isShort()) {
			return fmt::format("{}s", asShort());
		} else if (isInt()) {
			return fmt::format("{}", asInt());
		} else if (isLong()) {
			return fmt::format("{}l", asLong());
		} else if (isFloat()) {
			const nbt_float val = asFloat();
			if (val != val)
                return std::string(parsing::nan_string) + 'f';
			if (std::isinf(val))
				return ((val < 0) ? ('-' + std::string(parsing::infinity_string)) : std::string(parsing::infinity_string)) + 'f';
			return fmt::format("{}f", val);
		} else if (isDouble()) {
			const nbt_double val = asDouble();
			if (val != val)
                return std::string(parsing::nan_string) + 'd';
			if (std::isinf(val))
				return ((val < 0) ? ('-' + std::string(parsing::infinity_string)) : std::string(parsing::infinity_string)) + 'd';
			return fmt::format("{}d", val);
		} else if (isByteArray()) {
			return stringifyNBTTypedArray<nbt_byte_array, 'B'>(asByteArray(), indentAmount, indentChar, indentLevel);
		} else if (isString()) {
			return stringifyNBTString(asString());
		} else if (isList()) {
			return stringifyNBTList(asList(), indentAmount, indentChar, indentLevel);
		} else if (isCompound()) {
			return asCompound().toSNBT(indentAmount, indentChar, indentLevel);
		} else if (isIntArray()) {
			return stringifyNBTTypedArray<nbt_int_array, 'I'>(asIntArray(), indentAmount, indentChar, indentLevel);
		} else if (isLongArray()) {
			return stringifyNBTTypedArray<nbt_long_array, 'L'>(asLongArray(), indentAmount, indentChar, indentLevel);
		}
		return "";
	}

	std::ostream& operator <<(std::ostream& os, const NBT& j) {
		return os << j.toSNBT();
	}

}