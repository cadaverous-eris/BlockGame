#include "NBT.h"

#include <utility>
#include <sstream>
#include <algorithm>
#include <stack>
#include <vector>
#include <stdexcept>

#include <fmt/core.h>

#include "parsing_utils.h"
#include "util/math/bytes.h"
#include "util/variant_visitor.h"

namespace nbt {

    NBT::NBT(const nbt_byte b) noexcept : value(b) {}
    NBT::NBT(const nbt_short s) noexcept : value(s) {}
    NBT::NBT(const nbt_int i) noexcept : value(i) {}
    NBT::NBT(const nbt_long l) noexcept : value(l) {}
    NBT::NBT(const nbt_float f) noexcept : value(f) {}
    NBT::NBT(const nbt_double d) noexcept : value(d) {}
    NBT::NBT(const nbt_string& s) noexcept : value(s) {}
    NBT::NBT(nbt_string&& s) noexcept : value(std::move(s)) {}
    NBT::NBT(const nbt_list& l) noexcept : value(l) {}
    NBT::NBT(nbt_list&& l) noexcept : value(std::move(l)) {}
    NBT::NBT(const nbt_compound& c) noexcept : value(c) {}
    NBT::NBT(nbt_compound&& c) noexcept : value(std::move(c)) {}

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


    bool NBT::is(const TagType tagType) const noexcept {
		switch (tagType) {
            case TagByte: return isByte();
            case TagShort: return isShort();
            case TagInt: return isInt();
            case TagLong: return isLong();
            case TagFloat: return isFloat();
            case TagDouble: return isDouble();
            case TagString: return isString();
            case TagList: return isList();
            case TagCompound: return isCompound();
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

    NBT::operator nbt_byte() const { return getByte(); }
	NBT::operator nbt_short() const { return getShort(); }
	NBT::operator nbt_int() const { return getInt(); }
	NBT::operator nbt_long() const { return getLong(); }
	NBT::operator nbt_float() const { return getFloat(); }
	NBT::operator nbt_double() const { return getDouble(); }
	NBT::operator nbt_string() const { return asString(); }
    NBT::operator nbt_string_view() const { return asString(); }
	NBT::operator nbt_list() const { return asList(); }
	NBT::operator nbt_compound() const { return asCompound(); }


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
			return fmt::format("{:#}F", val);
		} else if (isDouble()) {
			const nbt_double val = asDouble();
			if (val != val)
                return std::string(parsing::nan_string) + 'd';
			if (std::isinf(val))
				return ((val < 0) ? ('-' + std::string(parsing::infinity_string)) : std::string(parsing::infinity_string)) + 'D';
			return fmt::format("{:#}D", val);
		} else if (isString()) {
			return stringifyNBTString(asString());
		} else if (isList()) {
			const auto& nbtList = asList();
			if (nbtList.isBytes()) return stringifyNBTIntegerArray<nbt_list::vector<nbt_byte>, 'B'>(asListOf<TagByte>(), indentAmount, indentChar, indentLevel);
			else if (nbtList.isInts()) return stringifyNBTIntegerArray<nbt_list::vector<nbt_int>, 'I'>(asListOf<TagInt>(), indentAmount, indentChar, indentLevel);
			else if (nbtList.isLongs()) return stringifyNBTIntegerArray<nbt_list::vector<nbt_long>, 'L'>(asListOf<TagLong>(), indentAmount, indentChar, indentLevel);
			else return nbtList.toSNBT(indentAmount, indentChar, indentLevel);
		} else if (isCompound()) {
			return asCompound().toSNBT(indentAmount, indentChar, indentLevel);
		}
		return "";
	}

	std::ostream& operator <<(std::ostream& os, const NBT& j) {
		return os << j.toSNBT();
	}


	inline void writeTagTypeByte(std::vector<unsigned char>& bytes, const TagType& tagType) {
		bytes.emplace_back(static_cast<unsigned char>(getTagTypeId(tagType)));
	}
	template<TagType tagType, typename T = nbt_type<tagType>, typename Uint = eng::sized_uint_t<sizeof(T)>, std::enable_if_t<std::is_arithmetic_v<T>, int> = 0>
	void writeNumericBytes(std::vector<unsigned char>& bytes, const T& n) {
		const Uint u = eng::toBigEndianUintBytes(n);
		const auto vecSize = bytes.size();
		bytes.resize(vecSize + sizeof(Uint));
		std::memcpy(bytes.data() + vecSize, &u, sizeof(Uint));
	}
	template<TagType tagType, typename ValType = nbt_type<tagType>, typename T = nbt_list::vector<ValType>, std::enable_if_t<((tagType == TagByte) || (tagType == TagInt) || (tagType == TagLong)), int> = 0>
	void writeIntegerArrayBytes(std::vector<unsigned char>& bytes, const T& t) {
		const nbt_int length = static_cast<nbt_int>(t.size());
		constexpr size_t valSize = sizeof(ValType);
		writeNumericBytes<TagInt>(bytes, length);
		if constexpr (tagType == TagByte) {
			const auto vecSize = bytes.size();
			bytes.resize(vecSize + (static_cast<size_t>(length) * valSize));
			std::memcpy(bytes.data() + vecSize, t.data(), static_cast<size_t>(length) * valSize);
		} else {
			bytes.reserve(bytes.size() + (static_cast<size_t>(length) * valSize));
			for (const auto& val : t)
				writeNumericBytes<tagType>(bytes, val);
		}
	}
	void writeStringBytes(std::vector<unsigned char>& bytes, nbt_string_view s) {
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
			const NBTList* nbtList = nullptr;
			const NBTCompound* nbtCompound = nullptr;
			std::variant<size_t, NBTCompound::const_iterator> pos = size_t{0};
			std::variant<size_t, NBTCompound::const_iterator> end = size_t{0};
			TagType listType = TagEnd;
		};

		vector<unsigned char> bytes;

		bool isByteList = false;
		bool isIntList = false;
		bool isLongList = false;
		if (this->isList()) {
			const auto& list = this->asList();
			if (list.isBytes()) isByteList = true;
			else if (list.isInts()) isIntList = true;
			else if (list.isLongs()) isLongList = true;
		}

		if (isByteList) writeTagTypeByte(bytes, TagType{ 7 });
		else if (isIntList) writeTagTypeByte(bytes, TagType{ 11 });
		else if (isLongList) writeTagTypeByte(bytes, TagType{ 12 });
		else writeTagTypeByte(bytes, this->getTagType());
		writeStringBytes(bytes, name);

		stack<State> stateStack {};

		const auto pushListState = [&stateStack, &bytes](const nbt_list& list, const size_t& listSize) {
			writeTagTypeByte(bytes, list.getTagType());
			writeNumericBytes<TagInt>(bytes, static_cast<nbt_int>(listSize));
			stateStack.emplace(&list, nullptr, size_t{0}, listSize, list.getTagType());
		};
		const auto pushCompoundState = [&stateStack](const nbt_compound& compound) {
			stateStack.emplace(nullptr, &compound, compound.begin(), compound.end());
		};
		const auto writeSimpleNBTBytes = [&bytes](const NBT& nbt) {
			const TagType tagType = nbt.getTagType();
			switch (tagType) {
			case TagByte:
				writeNumericBytes<TagByte>(bytes, nbt.as<TagByte>());
				break;
			case TagShort:
				writeNumericBytes<TagShort>(bytes, nbt.as<TagShort>());
				break;
			case TagInt:
				writeNumericBytes<TagInt>(bytes, nbt.as<TagInt>());
				break;
			case TagLong:
				writeNumericBytes<TagLong>(bytes, nbt.as<TagLong>());
				break;
			case TagFloat:
				writeNumericBytes<TagFloat>(bytes, nbt.as<TagFloat>());
				break;
			case TagDouble:
				writeNumericBytes<TagDouble>(bytes, nbt.as<TagDouble>());
				break;
			case TagString:
				writeStringBytes(bytes, nbt.as<TagString>());
				break;
			case TagList:
			case TagCompound:
				// TODO: throw?
				//throw std::runtime_error(fmt::format("Invalid NBT serializer state: {} payloads must be pushed to the state stack for serialization", to_string(tagType)));
				break;
			case TagEnd:
				// TODO: throw?
				//throw std::runtime_error(fmt::format("Invalid NBT serializer state: {} has no payload", to_string(tagType)));
				break;
			}
		};

		if (this->isList()) {
			const auto& list = this->asList();
			if (isByteList) {
				writeIntegerArrayBytes<TagByte>(bytes, list.as<TagByte>());
				return bytes;
			} else if (isIntList) {
				writeIntegerArrayBytes<TagInt>(bytes, list.as<TagInt>());
				return bytes;
			} else if (isLongList) {
				writeIntegerArrayBytes<TagLong>(bytes, list.as<TagLong>());
				return bytes;
			} else {
				pushListState(list, list.size());
			}
		} else if (this->isCompound()) {
			pushCompoundState(this->asCompound());
		} else {
			writeSimpleNBTBytes(*this);
			return bytes;
		}

		while (!stateStack.empty()) {
			auto& topState = stateStack.top();
			if (topState.nbtCompound) { // Compound
				const auto& end = std::get<NBTCompound::const_iterator>(topState.end);
				auto& pos = std::get<NBTCompound::const_iterator>(topState.pos);
				if (pos == end) {
					writeTagTypeByte(bytes, TagEnd);
					stateStack.pop();
				} else {
					while (pos != end) {
						const NBT& value = pos->second;
						if (value.isList()) {
							const auto& list = value.asList();
							if (list.is<TagByte>()) {
								writeTagTypeByte(bytes, TagType{ 7 });
								writeStringBytes(bytes, pos->first);
								pos++;
								writeIntegerArrayBytes<TagByte>(bytes, list.as<TagByte>());
							} else if (list.is<TagInt>()) {
								writeTagTypeByte(bytes, TagType{ 11 });
								writeStringBytes(bytes, pos->first);
								pos++;
								writeIntegerArrayBytes<TagInt>(bytes, list.as<TagInt>());
							} else if (list.is<TagLong>()) {
								writeTagTypeByte(bytes, TagType{ 12 });
								writeStringBytes(bytes, pos->first);
								pos++;
								writeIntegerArrayBytes<TagLong>(bytes, list.as<TagLong>());
							} else {
								writeTagTypeByte(bytes, value.getTagType());
								writeStringBytes(bytes, pos->first);
								pos++;
								const auto listSize = list.size();
								if (listSize == 0) {
									writeTagTypeByte(bytes, list.getTagType());
									writeNumericBytes<TagInt>(bytes, static_cast<nbt_int>(listSize));
								} else {
									pushListState(list, listSize);
									break;
								}
							}
						} else {
							writeTagTypeByte(bytes, value.getTagType());
							writeStringBytes(bytes, pos->first);
							pos++;
							if (value.isCompound()) {
								const auto& compound = value.asCompound();
								if (compound.empty()) {
									writeTagTypeByte(bytes, TagEnd);
								} else {
									pushCompoundState(compound);
									break;
								}
							} else {
								writeSimpleNBTBytes(value);
							}
						}
					}
				}
			} else if (topState.nbtList) { // List
				const auto& end = std::get<size_t>(topState.end);
				auto& pos = std::get<size_t>(topState.pos);
				if (pos < end) {
					std::visit([&](const auto& list) {
						using NBTType = typename std::decay_t<decltype(list)>::value_type;
						constexpr TagType tagType = nbt_tag_type<NBTType>;
						if constexpr (tagType == TagList) {
							// TODO: write lists of bytes, ints, and longs as NBT Tag Arrays
							pushListState(list[pos], list[pos].size());
							pos++;
						} else if constexpr (tagType == TagCompound) {
							pushCompoundState(list[pos++]);
						} else {
							while (pos < end) {
								if constexpr (tagType == TagString)
									writeStringBytes(bytes, list[pos]);
								//else if constexpr ((tagType == TagByteArray) || (tagType == TagIntArray) || (tagType == TagLongArray))
								//	writeIntegerArrayBytes<tagType>(bytes, list[pos]);
								else
									writeNumericBytes<tagType>(bytes, list[pos]);
								pos++;
							}
							stateStack.pop();
						}
					}, topState.nbtList->asListVariant());
				} else {
					stateStack.pop();
				}
			}/* else { // TODO: throw?
				throw std::runtime_error("Invalid NBT serializer state");
			}*/
		}

		return bytes;
	}

}