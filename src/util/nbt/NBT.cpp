#include "NBT.h"

#include <utility>
#include <sstream>
#include <algorithm>
#include <stack>
#include <vector>

#include <fmt/core.h>

#include "parsing_utils.h"
#include "util/math/bytes.h"

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

    template<typename T, char typeChar>
    static std::string stringifyNBTIntegerArray(const T& arr, const size_t indentAmount, const char indentChar, const size_t indentLevel) {
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
            return fmt::format("{:d}B", asByte());
		} else if (isShort()) {
			return fmt::format("{}S", asShort());
		} else if (isInt()) {
			return fmt::format("{}", asInt());
		} else if (isLong()) {
			return fmt::format("{}L", asLong());
		} else if (isFloat()) {
			const nbt_float val = asFloat();
			if (val != val)
                return std::string(parsing::nan_string) + 'f';
			if (std::isinf(val))
				return ((val < 0) ? ('-' + std::string(parsing::infinity_string)) : std::string(parsing::infinity_string)) + 'F';
			return fmt::format("{}F", val);
		} else if (isDouble()) {
			const nbt_double val = asDouble();
			if (val != val)
                return std::string(parsing::nan_string) + 'd';
			if (std::isinf(val))
				return ((val < 0) ? ('-' + std::string(parsing::infinity_string)) : std::string(parsing::infinity_string)) + 'D';
			return fmt::format("{}D", val);
		} else if (isByteArray()) {
			return stringifyNBTIntegerArray<nbt_byte_array, 'B'>(asByteArray(), indentAmount, indentChar, indentLevel);
		} else if (isString()) {
			return stringifyNBTString(asString());
		} else if (isList()) {
			return asList().toSNBT(indentAmount, indentChar, indentLevel);
		} else if (isCompound()) {
			return asCompound().toSNBT(indentAmount, indentChar, indentLevel);
		} else if (isIntArray()) {
			return stringifyNBTIntegerArray<nbt_int_array, 'I'>(asIntArray(), indentAmount, indentChar, indentLevel);
		} else if (isLongArray()) {
			return stringifyNBTIntegerArray<nbt_long_array, 'L'>(asLongArray(), indentAmount, indentChar, indentLevel);
		}
		return "";
	}

	std::ostream& operator <<(std::ostream& os, const NBT& j) {
		return os << j.toSNBT();
	}


	inline void writeTagType(std::vector<unsigned char> bytes, const TagType& tagType) {
		bytes.emplace_back(static_cast<unsigned char>(getTagTypeId(tagType)));
	}
	template<TagType tagType, typename T = nbt_type<tagType>, typename Uint = eng::sized_uint_t<sizeof(T)>, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	void writeNumericBytes(std::vector<unsigned char> bytes, const T& n) {
		const Uint u = eng::toBigEndianUintBytes(n);
		const auto vecSize = bytes.size();
		bytes.resize(vecSize + sizeof(Uint));
		std::memcpy(bytes.data() + vecSize, &u, sizeof(Uint));
	}
	template<TagType tagType, typename T = nbt_type<tagType>, typename ValType = typename T::value_type, std::enable_if_t<((tagType == TagByteArray) || (tagType == TagIntArray) || (tagType == TagLongArray)), int> = 0>
	void writeIntegerArrayBytes(std::vector<unsigned char> bytes, const T& t) {
		const nbt_int length = static_cast<nbt_int>(t.size());
		constexpr size_t valSize = sizeof(ValType);
		writeNumericBytes<TagInt>(bytes, length);
		if constexpr (tagType == TagByteArray) {
			const auto vecSize = bytes.size();
			bytes.resize(vecSize + (length * valSize));
			std::memcpy(bytes.data() + vecSize, t.data(), length * valSize);
		} else {
			bytes.reserve(bytes.size() + (length * valSize));
			for (const auto& val : t)
				writeNumericBytes<ValType>(bytes, val);
		}
	}
	void writeStringBytes(std::vector<unsigned char> bytes, nbt_string_view s) {
		using CharType = typename nbt_string::value_type;
		static_assert(sizeof(CharType) == 1);
		const nbt_short length = static_cast<nbt_short>(s.size());
		writeNumericBytes<TagShort>(bytes, length);
		const auto vecSize = bytes.size();
		bytes.resize(vecSize + static_cast<size_t>(length));
		std::memcpy(bytes.data() + vecSize, s.data(), static_cast<size_t>(length));
	}

	std::vector<unsigned char> NBT::toBinary(nbt_string_view name) const {
		using std::stack, std::reference_wrapper, std::vector;

		struct State {
			const NBT* nbt;
			const NBTList* listList = nullptr;
			const NBTCompound* listCompound = nullptr;
			std::variant<size_t, NBTCompound::const_iterator> pos = size_t{0};
			std::variant<size_t, NBTCompound::const_iterator> end = size_t{0};
			TagType listType = TagEnd;
		};

		vector<unsigned char> bytes;

		stack<State> stateStack {};
		if (this->isList()) {
			const auto& list = this->asList();
			stateStack.emplace(nullptr, &list, nullptr, 0, list.size(), list.getTagType());
		} else if (this->isCompound()) {
			const auto& compound = this->asCompound();
			stateStack.emplace(nullptr, nullptr, &compound, compound.begin(), compound.end());
		} else {
			stateStack.emplace(this);
		}

		while (!stateStack.empty()) {
			// TODO: implement
			assert(false);




		}

		return bytes;
	}

}