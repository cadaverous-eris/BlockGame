#include "parsing_utils.h"

#include <map>
#include <vector>
#include <variant>
#include <stack>
#include <functional>
#include <limits>
#include <cmath>

#include <fmt/core.h>

#include "NBT.h"
#include "util/math/bytes.h"
#include "util/text/text_utils.h"
#include "util/text/unicode_utils.h"

namespace nbt::parsing {

	// binary NBT parsing

	class NBTParser {
	public:
		using NBTRef = std::reference_wrapper<NBT>;

		struct State {
			NBTRef payload;
			TagType listType = TagByte;
			size_t listLength = 0;
		};

		using StateStack = std::stack<State>;

		template<size_t S = std::dynamic_extent>
		using ByteSpan = std::span<const unsigned char, S>;

	private:
		ByteSpan<> bytes;
		StateStack stateStack {};
		std::stack<NBT> currentListItemStack {};
		size_t pos { 0 };

	public:
		NBTParser(ByteSpan<> bytes) : bytes(bytes) {}

		NBT operator ()() {
			if (bytes.empty())
				throw ParseError("No data");

			NBT result(nbt_compound{});
			stateStack.emplace(NBTRef(result));

			size_t loops = 0;
			while ((!stateStack.empty()) && (pos < bytes.size())) {
				if ((++loops >= bytes.size()) || (loops == size_t(-1)))
					throw ParseError(fmt::format("Too many parser iterations ({})", loops));

				auto& stackTop = stateStack.top();
				auto& currentPayload = stackTop.payload.get();

				if (currentPayload.is<TagList>()) {
					const size_t listLength = stackTop.listLength;
					//const TagType listType = stackTop.listType;
					NBTList& list = currentPayload.asList();
					//fmt::print("List Size: {}, List Length: {}, List Type: {}\n", list.size(), listLength, to_string(listType));
					if (list.size() >= listLength)
						popState();
					else
						readListItemPayload(stackTop.listType);
				} else if (currentPayload.is<TagCompound>()) {
					const TagType tagType { bytes[pos++] };
					if (tagType == TagEnd) {
						popState();
					} else {
						if (pos >= bytes.size())
							throw ParseError("NBT data ended unexpectedly");
						const auto tagName = readStringPayload();
						nbt_compound& compound = currentPayload.as<TagCompound>();
						auto [it, emplaced] = compound.emplace(std::move(tagName));
						if (!emplaced) throw ParseError(fmt::format("Duplicate key in NBT Compound: \"{}\"", it->first));
						stateStack.emplace(NBTRef(it->second));
						readPayload(tagType);
					}
				} else {
					throw ParseError("Invalid parser state");
				}
			}
			//if (pos < bytes.size()) throw ParseError("Finished parsing NBT before reaching end of data");
			if (stateStack.size() > 1) throw ParseError("NBT data ended unexpectedly");
			return result;
		}


	private:

		void popState() {
			const auto& poppedState = stateStack.top();
			const auto& poppedPayload = poppedState.payload.get();
			stateStack.pop();
			if (!currentListItemStack.empty() && (&poppedPayload == &currentListItemStack.top())) {
				auto& topState = stateStack.top();
				auto& topPayload = topState.payload.get();
				NBT& currentListItem = currentListItemStack.top();
				if (!topPayload.isList())
					throw ParseError("Parsed NBT payload was stored on list stack, but the container value is not a list");
				nbt_list& list = topPayload.asList();
				if (currentListItem.getTagType() != list.getTagType())
					throw ParseError(fmt::format("NBT List of type {} cannot accept value of type {}", to_string(list.getTagType()), to_string(currentListItem.getTagType())));
				std::visit([&currentListItem](auto& l) {
					using ListVal = typename std::decay_t<decltype(l)>::value_type;
					constexpr TagType listType = nbt_tag_type<ListVal>;
					l.emplace_back(std::move(currentListItem.as<listType>()));
				}, list.asListVariant());
				currentListItemStack.pop();
			}
		}

		void readPayload(TagType tagType) {
			auto& stackTop = stateStack.top();
			switch (tagType) {
			case TagEnd:
				throw ParseError(fmt::format("{} has no payload", to_string(TagEnd)));
				stateStack.pop();
				return;
			case TagByte:
				stackTop.payload.get().emplace<nbt_byte>(readNumericPayload<TagByte>());
				stateStack.pop();
				return;
			case TagShort:
				stackTop.payload.get().emplace<nbt_short>(readNumericPayload<TagShort>());
				stateStack.pop();
				return;
			case TagInt:
				stackTop.payload.get().emplace<nbt_int>(readNumericPayload<TagInt>());
				stateStack.pop();
				return;
			case TagLong:
				stackTop.payload.get().emplace<nbt_long>(readNumericPayload<TagLong>());
				stateStack.pop();
				return;
			case TagFloat:
				stackTop.payload.get().emplace<nbt_float>(readNumericPayload<TagFloat>());
				stateStack.pop();
				return;
			case TagDouble:
				stackTop.payload.get().emplace<nbt_double>(readNumericPayload<TagDouble>());
				stateStack.pop();
				return;
			case TagString:
				stackTop.payload.get().emplace<nbt_string>(readStringPayload());
				stateStack.pop();
				return;
			case TagList:
				{
					const TagType listType { bytes[pos++] };
					const size_t listLength = static_cast<size_t>(readNumericPayload<TagInt>());
					stackTop.payload.get().emplace<TagList>(createNBTList(listType, listLength));
					if (listLength == 0) {
						stateStack.pop();
					} else {
						stackTop.listType = listType;
						stackTop.listLength = listLength;
					}
				}
				return;
			case TagCompound:
				stackTop.payload.get().emplace<TagCompound>();
				return;
			}
			if (tagType == TagType{ 7 }) {
				stackTop.payload.get().emplace<nbt_list>(readIntegerArrayPayload<TagByte>());
				stateStack.pop();
				return;
			} else if (tagType == TagType{ 11 }) {
				stackTop.payload.get().emplace<nbt_list>(readIntegerArrayPayload<TagInt>());
				stateStack.pop();
				return;
			} else if (tagType == TagType{ 12 }) {
				stackTop.payload.get().emplace<nbt_list>(readIntegerArrayPayload<TagLong>());
				stateStack.pop();
				return;
			}
			throw ParseError(fmt::format("Invalid NBT Tag Type: {}", getTagTypeId(tagType)));
		}

		void readListItemPayload(TagType tagType) {
			auto& stackTop = stateStack.top();
			switch (tagType) {
			case TagEnd:
				throw ParseError(fmt::format("{} has no payload", to_string(TagEnd)));
				return;
			case TagByte:
				stackTop.payload.get().asListOf<TagByte>().emplace_back(readNumericPayload<TagByte>());
				return;
			case TagShort:
				stackTop.payload.get().asListOf<TagShort>().emplace_back(readNumericPayload<TagShort>());
				return;
			case TagInt:
				stackTop.payload.get().asListOf<TagInt>().emplace_back(readNumericPayload<TagInt>());
				return;
			case TagLong:
				stackTop.payload.get().asListOf<TagLong>().emplace_back(readNumericPayload<TagLong>());
				return;
			case TagFloat:
				stackTop.payload.get().asListOf<TagFloat>().emplace_back(readNumericPayload<TagFloat>());
				return;
			case TagDouble:
				stackTop.payload.get().asListOf<TagDouble>().emplace_back(readNumericPayload<TagDouble>());
				return;
			case TagString:
				stackTop.payload.get().asListOf<TagString>().emplace_back(readStringPayload());
				return;
			case TagList:
				{
					const TagType listType { bytes[pos++] };
					const size_t listLength = static_cast<size_t>(readNumericPayload<TagInt>());
					if (listLength == 0) {
						stackTop.payload.get().asListOf<TagList>().emplace_back(createNBTList(listType, listLength));
					} else {
						currentListItemStack.emplace(createNBTList(listType, listLength));
						stateStack.emplace(NBTRef(currentListItemStack.top()), listType, listLength);
					}
				}
				return;
			case TagCompound:
				currentListItemStack.emplace(nbt_compound {});
				stateStack.emplace(NBTRef(currentListItemStack.top()));
				return;
			}
			if (tagType == TagType{ 7 }) {
				stackTop.payload.get().asListOf<TagList>().emplace_back(readIntegerArrayPayload<TagByte>());
				return;
			} else if (tagType == TagType{ 11 }) {
				stackTop.payload.get().asListOf<TagList>().emplace_back(readIntegerArrayPayload<TagInt>());
				return;
			} else if (tagType == TagType{ 12 }) {
				stackTop.payload.get().asListOf<TagList>().emplace_back(readIntegerArrayPayload<TagLong>());
				return;
			}
			throw ParseError(fmt::format("Invalid NBT Tag Type: {}", getTagTypeId(tagType)));
		}

		template<size_t S>
		inline ByteSpan<S> getDataSpan(size_t offset) {
			if ((offset + S) > bytes.size())
				throw ParseError("NBT data ended unexpectedly");
			return ByteSpan<S>(bytes.data() + offset, S);
		}
		inline ByteSpan<> getDataSpan(size_t offset, size_t length) {
			if ((offset + length) > bytes.size())
				throw ParseError("NBT data ended unexpectedly");
			return ByteSpan<>(bytes.data() + offset, length);
		}

		template<TagType tagType, typename NBTType = nbt_type<tagType>, std::enable_if_t<std::is_arithmetic_v<NBTType>, int> = 0>
		NBTType readNumericPayload() {
			const size_t offset = pos;
			constexpr size_t valSize = sizeof(NBTType);
			pos += valSize;
			return eng::fromBigEndianByteSpan<NBTType>(getDataSpan<valSize>(offset));
		}
		template<TagType tagType, typename ValueType = nbt_type<tagType>, std::enable_if_t<((tagType == TagByte) || (tagType == TagInt) || (tagType == TagLong)), int> = 0>
		nbt_list readIntegerArrayPayload() {
			constexpr size_t valSize = sizeof(ValueType);
			const size_t length = static_cast<size_t>(readNumericPayload<TagInt>());
			nbt_list::vector<ValueType> payload {};
			if constexpr (tagType == TagByte) {
				if ((pos + (length * valSize)) > bytes.size())
					throw ParseError("NBT data ended unexpectedly");
				payload.resize(length);
				std::memcpy(payload.data(), bytes.data() + pos, length * valSize);
			} else {
				payload.reserve(length);
				for (size_t i = 0; i < length; i++)
					payload.emplace_back(eng::fromBigEndianByteSpan<ValueType>(getDataSpan<valSize>(pos + (i * valSize))));
			}
			pos += length * valSize;
			return nbt_list(std::move(payload));
		}
		nbt_string readStringPayload() {
			using CharType = typename nbt_string::value_type;
			static_assert(sizeof(CharType) == 1);
			const size_t length = static_cast<size_t>(readNumericPayload<TagShort>());
			if ((pos + length) > bytes.size())
				throw ParseError("NBT data ended unexpectedly");
			nbt_string result(length, '\0');
			//result.resize(length);
			std::memcpy(result.data(), bytes.data() + pos, length);
			pos += length;
			return result;
		}

		nbt_list createNBTList(TagType listType, size_t length) {
			switch (listType) {
			case TagEnd:
				throw ParseError(fmt::format("Invalid NBT Tag Type for {}: {}", to_string(TagList), to_string(TagEnd)));
				break;
			case TagByte:
				{
					std::vector<nbt_byte> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagShort:
				{
					std::vector<nbt_short> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagInt:
				{
					std::vector<nbt_int> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagLong:
				{
					std::vector<nbt_long> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagFloat:
				{
					std::vector<nbt_float> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagDouble:
				{
					std::vector<nbt_double> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagString:
				{
					std::vector<nbt_string> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagList:
				{
					std::vector<nbt_list> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			case TagCompound:
				{
					std::vector<nbt_compound> list {};
					list.reserve(length);
					return nbt_list(std::move(list));
				}
			}
			if ((listType == TagType{ 7 }) || (listType == TagType{ 11 }) || (listType == TagType{ 12 })) {
				std::vector<nbt_list> list {};
				list.reserve(length);
				return nbt_list(std::move(list));
			}
			throw ParseError(fmt::format("Invalid NBT Tag Type: {}", getTagTypeId(listType)));
		}

	};

	NBT parseNBT(NBTParser::ByteSpan<> bytes) {
		return NBTParser(bytes)();
	}


	// SNBT parsing

	struct StringPos {
		const size_t line, col;
	};
	static StringPos calculateStringPos(const std::string_view str, const size_t pos) noexcept {
		if (pos == std::string_view::npos)
			return { size_t(-1), 0 };
		size_t line = 1, col = 0;
		char prevChar = 0;
		for (size_t i = 0; i < str.size(); i++) {
			if (i == pos) return { line, col };
			const char currentChar = str[i];
			if (currentChar == '\n') {
				line++;
				col = 0;
			} else if ((currentChar != '\r') || (prevChar != '\n')) {
				col++;
			}
			prevChar = currentChar;
		}
		if (pos == str.size()) return { line, col };
		return { line + 1, 0 };
	}

	static std::string createParseErrorString(const std::string_view str, const size_t pos, const std::string& what) {
		const StringPos errorPos = calculateStringPos(str, pos);
		return fmt::format("{}\n(line {}, col {})", what, errorPos.line, errorPos.col);
	}
	ParseError::ParseError(const std::string_view str, const size_t pos, const std::string& what) :
			std::runtime_error(createParseErrorString(str, pos, what)) {}
	ParseError::ParseError(const std::string& what) :
			std::runtime_error(what) {}

	static constexpr std::string_view whitespace_chars = "\x09\x0A\x0B\x0C\x0D\x20";

    static std::string parseString(const std::string_view str, size_t& pos, const char quoteChar) {
		using namespace std::string_literals;
		std::string result {};
		pos = str.find_first_of(quoteChar, pos);
		if (pos == std::string_view::npos)
			throw ParseError(str, pos, "SNBT string literal is missing an opening "s + quoteChar);
		pos++;
		for (size_t cursor = pos; ; cursor++) {
			if (cursor >= str.size())
				throw ParseError(str, cursor, "SNBT string literal is missing a closing "s + quoteChar);
			if (str[cursor] == '\\') { // escape sequence
				if (cursor > pos) result.append(str, pos, cursor - pos);
				cursor++;
				if (cursor >= str.size())
					throw ParseError(str, cursor, "SNBT string literal is missing a closing "s + quoteChar);
				pos = cursor;
				switch (str[cursor]) {
				case '\\':
					result += '\\';
					pos++;
					break;
				case '/':
					result += '/';
					pos++;
					break;
				case '\'':
					result += '\'';
					pos++;
					break;
				case '"':
					result += '"';
					pos++;
					break;
				case 'a':
					result += '\a';
					pos++;
					break;
				case 'b':
					result += '\b';
					pos++;
					break;
				case 't':
					result += '\t';
					pos++;
					break;
				case 'n':
					result += '\n';
					pos++;
					break;
				case 'v':
					result += '\v';
					pos++;
					break;
				case 'f':
					result += '\f';
					pos++;
					break;
				case 'r':
					result += '\r';
					pos++;
					break;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				{
					unsigned char oct = static_cast<unsigned char>(str[cursor] - '0');
					for (size_t digits { 1 }; digits < 3; digits++) {
						if (cursor >= str.size())
							throw ParseError(str, cursor, "SNBT string literal is missing a closing "s + quoteChar);
						char nextChar = str[cursor + 1];
						if ((nextChar >= '0') && (nextChar <= '7')) {
							oct = static_cast<unsigned char>(oct << 3) | static_cast<unsigned char>(nextChar - '0');
							cursor++;
						} else {
							break;
						}
					}
					result += static_cast<char>(oct);
					pos = cursor + 1;
				}
				break;
				case 'X':
				case 'x':
					{
						unsigned char hex { 0 };
						for (size_t digits = 0; digits < 2; digits++) {
							cursor++;
							if (cursor >= str.size())
								throw ParseError(str, cursor, "SNBT string literal is missing a closing "s + quoteChar);
							const char hexChar = str[cursor];
							if ((hexChar >= '0') && (hexChar <= '9'))
								hex = static_cast<unsigned char>(hex << 4) | static_cast<unsigned char>(hexChar - '0');
							else if ((hexChar >= 'A') && (hexChar <= 'F'))
								hex = static_cast<unsigned char>(hex << 4) | static_cast<unsigned char>(10 + hexChar - 'A');
							else if ((hexChar >= 'a') && (hexChar <= 'f'))
								hex = static_cast<unsigned char>(hex << 4) | static_cast<unsigned char>(10 + hexChar - 'a');
							else
								throw ParseError(str, cursor, "Invalid hex literal");
						}
						result += static_cast<char>(hex);
						pos = cursor + 1;
					}
					break;
				case 'u':
					{
						char32_t codepoint { 0 };
						for (size_t digits = 0; digits < 4; digits++) {
							cursor++;
							if (cursor >= str.size())
								throw ParseError(str, cursor, "SNBT string literal is missing a closing "s + quoteChar);
							const char hexChar = str[cursor];
							if ((hexChar >= '0') && (hexChar <= '9'))
								codepoint = (codepoint << 4) | static_cast<char32_t>(hexChar - '0');
							else if ((hexChar >= 'A') && (hexChar <= 'F'))
								codepoint = (codepoint << 4) | static_cast<char32_t>(10 + hexChar - 'A');
							else if ((hexChar >= 'a') && (hexChar <= 'f'))
								codepoint = (codepoint << 4) | static_cast<char32_t>(10 + hexChar - 'a');
							else
								throw ParseError(str, cursor, "Invalid 16-bit unicode literal");
						}
						eng::unicode::appendUtf32CharToUtf8(result, codepoint);
						pos = cursor + 1;
					}
					break;
				case 'U':
					{
						char32_t codepoint { 0 };
						for (size_t digits = 0; digits < 8; digits++) {
							cursor++;
							if (cursor >= str.size())
								throw ParseError(str, cursor, "SNBT string literal is missing a closing "s + quoteChar);
							const char hexChar = str[cursor];
							if ((hexChar >= '0') && (hexChar <= '9'))
								codepoint = (codepoint << 4) | static_cast<char32_t>(hexChar - '0');
							else if ((hexChar >= 'A') && (hexChar <= 'F'))
								codepoint = (codepoint << 4) | static_cast<char32_t>(10 + hexChar - 'A');
							else if ((hexChar >= 'a') && (hexChar <= 'f'))
								codepoint = (codepoint << 4) | static_cast<char32_t>(10 + hexChar - 'a');
							else
								throw ParseError(str, cursor, "Invalid 32-bit unicode literal");
						}
						eng::unicode::appendUtf32CharToUtf8(result, codepoint);
						pos = cursor + 1;
					}
					break;
				}
			} else if (str[cursor] == '\n') {
				throw ParseError(str, cursor, "Unexpected char '\\n'. Line breaks are not allowed in SNBT string literals.");
			} else if (str[cursor] == quoteChar) {
				if (cursor > pos) result.append(str, pos, cursor - pos);
				pos = cursor + 1;
				return result;
			}
		}
	}

	static std::string parseKey(const std::string_view str, size_t& pos) {
		using namespace std::string_view_literals;
		constexpr std::string_view whitespace_colon_chars = "\x09\x0A\x0B\x0C\x0D\x20:"sv;
		pos = str.find_first_not_of(whitespace_chars, pos);
		if (pos == std::string_view::npos)
			throw ParseError(str, pos, "Could not find SNBT object key");
		const char firstChar = str[pos];
		if (firstChar == '\'' || firstChar == '"') {
			std::string result = parseString(str, pos, firstChar);
			if (result.empty())
				throw ParseError(str, pos, "Could not find SNBT object key");
			return result;
		}
		auto endPos = str.find_first_of(whitespace_colon_chars, pos);
		if (endPos == std::string_view::npos) endPos = str.size();
		std::string result { str.substr(pos, endPos - pos) };
		pos = endPos;
		if (result.empty())
			throw ParseError(str, pos, "Could not find SNBT object key");
		return result;
	}

    template<typename Int>
    static Int parseIntegral(const std::string_view str, size_t& pos, std::string_view validSuffixChars) {
        using namespace std::string_literals;
		using namespace std::string_view_literals;
		constexpr auto npos = std::string_view::npos;

        size_t endIndex = str.size();
        if (pos >= endIndex)
			throw ParseError(str, pos, "Invalid number literal");
        bool hasSign = false;
		bool isNegative = false;
		if (str[pos] == '+') {
			hasSign = true;
			pos++;
		} else if (str[pos] == '-') {
			hasSign = true;
			isNegative = true;
			pos++;
		}
		if (hasSign && (pos >= endIndex))
			throw ParseError(str, pos, "Invalid number literal");

        Int value { 0 };
		size_t i = pos, digits = 0;
        for (; ; i++) {
			if (i >= str.size()) {
				if (digits == 0) throw ParseError(str, i, "SNBT string ended unexpectedly (incomplete numeric literal)");
				else break;
			}
			const char digitChar = str[i];
			if ((digitChar >= '0') && (digitChar <= '9')) {
				digits++;
                // TODO: throw exception if value of literal would be too high or low for the type
				value = (value * 10) + static_cast<Int>(digitChar - '0');
			} else {
				break;
			}
		}
        pos = i;
		if (digits == 0)
			throw ParseError(str, i, "Unexpected token '"s + str[pos] + "', expected decimal digit"s);
		if ((validSuffixChars.length() > 0) && (validSuffixChars.find_first_of(str[i]) != npos))
			pos = i + 1;
		return (isNegative) ? -value : value;
    }

    template<typename Float>
    static Float parseFloatingPoint(const std::string_view str, size_t& pos, std::string_view validSuffixChars) {
        using namespace std::string_literals;
		using namespace std::string_view_literals;
		constexpr auto npos = std::string_view::npos;

        size_t endIndex = str.size();
        if (pos >= endIndex)
			throw ParseError(str, pos, "Invalid number literal");
        bool hasSign = false;
		bool isNegative = false;
		if (str[pos] == '+') {
			hasSign = true;
			pos++;
		} else if (str[pos] == '-') {
			hasSign = true;
			isNegative = true;
			pos++;
		}
		if (pos >= endIndex)
			throw ParseError(str, pos, "Invalid number literal");
        std::string_view valString = str.substr(pos, endIndex - pos);

        if (valString.starts_with(nan_string)) {
			pos += nan_string.length();
			if ((validSuffixChars.length() > 0) && (validSuffixChars.find_first_of(str[pos]) != npos))
				pos += 1;
			Float value = std::numeric_limits<Float>::quiet_NaN();
			return isNegative ? -value : value;
		}
		if (valString.starts_with(infinity_string)) {
			pos += infinity_string.length();
			if ((validSuffixChars.length() > 0) && (validSuffixChars.find_first_of(str[pos]) != npos))
				pos += 1;
			Float value = std::numeric_limits<Float>::infinity();
			return isNegative ? -value : value;
		}

        bool hasDecimal = false, hasExp = false, hasExpSign = false, isExpNegative = false;
        Float value { 0 };
        nbt_short expValue { 0 };
		size_t i = pos, digits = 0, fractDigits = 0, expDigits = 0;
        for (; ; i++) {
			if (i >= str.size()) {
				if (digits == 0) throw ParseError(str, i, "SNBT string ended unexpectedly (incomplete numeric literal)");
				else break;
			}
			const char digitChar = str[i];
			if (digitChar == '.') {
				hasDecimal = true;
				pos = i + 1;
				break;
			} else if ((digitChar == 'E') || (digitChar == 'e')) {
				if (digits == 0)
					throw ParseError(str, i, "Unexpected token '"s + digitChar + "', expected decimal digit"s);
				hasExp = true;
				pos = i + 1;
				break;
			} else if ((digitChar >= '0') && (digitChar <= '9')) {
				digits++;
				value = (value * 10) + (digitChar - '0');
			} else {
				break;
			}
		}
		if (hasDecimal) {
			for (i = pos; ; i++) {
				if (i >= str.size()) {
					if ((digits == 0) && (fractDigits == 0)) throw ParseError(str, i, "SNBT string ended unexpectedly (incomplete numeric literal)");
					else break;
				}
				const char digitChar = str[i];
				if ((digitChar == 'E') || (digitChar == 'e')) {
					if ((fractDigits == 0) && (digits == 0))
						throw ParseError(str, i, "Unexpected token '"s + digitChar + "', expected decimal digit"s);
					hasExp = true;
					pos = i + 1;
					break;
				} else if ((digitChar >= '0') && (digitChar <= '9')) {
					fractDigits++;
                    const Float digitVal = static_cast<Float>(digitChar - '0');
                    if (digitVal != 0)
                        // TODO: use a lookup table for powers of ten, limit number of digits after decimal
					    value += digitVal / std::pow(Float{ 10 }, static_cast<Float>(fractDigits));
				} else {
					break;
				}
			}
		}
		if (hasExp) {
			for (i = pos; ; i++) {
				if (i >= str.size()) {
					if (expDigits == 0) throw ParseError(str, i, "SNBT string ended unexpectedly (incomplete numeric literal)");
					else break;
				}
				const char digitChar = str[i];
                const bool couldBeSign = !hasExpSign && (expDigits == 0);
				if (couldBeSign && (digitChar == '+')) {
					hasExpSign = true;
				} else if (couldBeSign && (digitChar == '-')) {
					hasExpSign = true;
					isExpNegative = true;
				} else if ((digitChar >= '0') && (digitChar <= '9')) {
					expDigits++;
					expValue = (expValue * 10) + (digitChar - '0');
				} else {
					break;
				}
			}
			if (expDigits > 0) {
                // TODO: use a lookup table for powers of ten, limit number of exponent digits
				const Float expFactor = std::pow(Float{ 10 }, static_cast<Float>(expValue));
                if (isExpNegative) value /= expFactor;
                else value *= expFactor;
			}
		}
		pos = i;
		if (((digits == 0) && (fractDigits == 0)) || (hasExp && (expDigits == 0)))
			throw ParseError(str, i, "Unexpected token '"s + str[pos] + "', expected decimal digit"s);
		if ((validSuffixChars.length() > 0) && (validSuffixChars.find_first_of(str[i]) != npos))
			pos = i + 1;
		return (isNegative) ? -value : value;
    }

    static std::variant<nbt_byte, nbt_short, nbt_int, nbt_long, nbt_float, nbt_double> parseNumber(const std::string_view str, size_t& pos) {
		using namespace std::string_literals;
		using namespace std::string_view_literals;
		constexpr auto npos = std::string_view::npos;
		constexpr std::string_view number_end_chars = "\x09\x0A\x0B\x0C\x0D\x20,]}";

		size_t endIndex = str.find_first_of(number_end_chars, pos);
		if (endIndex == npos) endIndex = str.size();

		if (pos >= endIndex)
			throw ParseError(str, pos, "Invalid number literal");

        char suffix = '\0';
        const bool hasSuffix = "bBsSlLfFdD"sv.find_first_of(str[endIndex - 1]) != npos;
        if (hasSuffix) {
            suffix = str[endIndex - 1];
        }

        if (hasSuffix) {
            if (suffix == 'b' || suffix == 'B')
                return parseIntegral<nbt_byte>(str, pos, "bB"sv);
            if (suffix == 's' || suffix == 'S')
                return parseIntegral<nbt_short>(str, pos, "sS"sv);
            if (suffix == 'l' || suffix == 'L')
                return parseIntegral<nbt_long>(str, pos, "lL"sv);
            if (suffix == 'f' || suffix == 'F')
                return parseFloatingPoint<nbt_float>(str, pos, "fF"sv);
            if (suffix == 'd' || suffix == 'D')
                return parseFloatingPoint<nbt_double>(str, pos, "dD"sv);
        }
        const bool isFloatingPoint = str.substr(pos, endIndex - pos).find_first_of(".eEn"sv) != npos; // the 'n' catches both 'nan' and 'inf';
        if (isFloatingPoint)
            return parseFloatingPoint<nbt_double>(str, pos, "dD"sv);
        else
            return parseIntegral<nbt_int>(str, pos, ""sv);
	}

    NBT parseSNBT(std::string_view str) {
		using namespace std::string_literals;
		using namespace std::string_view_literals;
		using std::stack, std::variant, std::reference_wrapper;
		constexpr auto npos = std::string_view::npos;
		enum class ParserState { Key, Colon, Value, Comma, };

	    NBT result {};
		stack<reference_wrapper<NBT>> nbtStack { { result } };
		stack<NBT> currentListItemStack {};
		ParserState parserState = ParserState::Value;
		size_t pos = 0;

		const auto nbtStackPop = [&nbtStack, &currentListItemStack, &str, &pos] {
			const auto& popped = nbtStack.top().get();
			nbtStack.pop();
			if (!currentListItemStack.empty() && (&popped == &currentListItemStack.top())) {
				auto& top = nbtStack.top().get();
				NBT& currentListItem = currentListItemStack.top();
				if (!top.isList())
					throw ParseError(str, pos, "Parsed value was stored on list stack, but the container value is not a list");
				nbt_list& list = top.asList();
				if (list.empty())
					list.emplaceListOfTagType(popped.getTagType());
				else if (currentListItem.getTagType() != list.getTagType())
					throw ParseError(str, pos, fmt::format("List of type {} cannot accept value of type {}", to_string(list.getTagType()), to_string(currentListItem.getTagType())));
				std::visit([&currentListItem](auto& l) {
					using ListVal = typename std::decay_t<decltype(l)>::value_type;
					constexpr TagType listType = nbt_tag_type<ListVal>;
					l.emplace_back(std::move(currentListItem.as<listType>()));
				}, list.asListVariant());
				currentListItemStack.pop();
			}
		};

		while ((pos != npos) && (pos < str.size()) && !nbtStack.empty()) {
			pos = str.find_first_not_of(whitespace_chars, pos);
			if (pos == npos) break;
            const auto& stackTop = nbtStack.top().get();
			const char currentChar = str[pos];
            const bool parsingByteArray = stackTop.isListOf<TagByte>();
            const bool parsingIntArray = stackTop.isListOf<TagInt>();
            const bool parsingLongArray = stackTop.isListOf<TagLong>();
            const bool parsingIntegerArray = parsingByteArray || parsingIntArray || parsingLongArray;
			const bool parsingList = (!parsingIntegerArray) && stackTop.is<TagList>();
			const bool parsingCompound= stackTop.is<TagCompound>();

			if (parserState == ParserState::Key) {
				if (currentChar == '}') {
					if (!parsingCompound)
						throw ParseError(str, pos, "Unexpected token '}', expected key");
					nbtStackPop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				std::string key = parseKey(str, pos);
				nbt_compound& compound = nbtStack.top().get().as<TagCompound>();
				auto [it, emplaced] = compound.emplace(std::move(key));
				if (!emplaced) throw ParseError(str, pos, "Duplicate key: \""s + it->first + "\""s);
				nbtStack.push(it->second);
				parserState = ParserState::Colon;
			} else if (parserState == ParserState::Colon) {
				if (currentChar == ':') {
					pos++;
					parserState = ParserState::Value;
				} else {
					throw ParseError(str, pos, "Expected ':', found '"s + currentChar + "' instead"s);
				}
			} else if (parserState == ParserState::Value) {
				if (currentChar == ']') {
					if (!parsingList && !parsingIntegerArray)
						throw ParseError(str, pos, "Unexpected token ']', expected value");
					nbtStackPop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}

                if (parsingByteArray) {
                    auto& arr = nbtStack.top().get().asListOf<TagByte>();
                    const auto parsedNumber = parseIntegral<nbt_byte>(str, pos, ""sv);
                    arr.emplace_back(parsedNumber);
                    parserState = ParserState::Comma;
					continue;
                }
                if (parsingIntArray) {
                    auto& arr = nbtStack.top().get().asListOf<TagInt>();
                    const auto parsedNumber = parseIntegral<nbt_int>(str, pos, ""sv);
                    arr.emplace_back(parsedNumber);
                    parserState = ParserState::Comma;
					continue;
                }
                if (parsingLongArray) {
                    auto& arr = nbtStack.top().get().asListOf<TagLong>();
                    const auto parsedNumber = parseIntegral<nbt_long>(str, pos, ""sv);
                    arr.emplace_back(parsedNumber);
                    parserState = ParserState::Comma;
					continue;
                }

				if (parsingList) {
					currentListItemStack.emplace();
					nbtStack.push(currentListItemStack.top());
				}

				if (currentChar == '{') {
					nbtStack.top().get().emplace<TagCompound>();
					pos++;
					parserState = ParserState::Key;
					continue;
				}
				if (currentChar == '[') {
                    if ((pos + 2) < str.length()) {
                        const char typeChar = str[pos + 1];
                        bool isByteArray = (typeChar == 'B'), isIntArray = (typeChar == 'I'), isLongArray = (typeChar == 'L');
                        if (isByteArray || isIntArray || isLongArray) {
                            if (str[pos + 2] != ';')
                                throw ParseError(str, pos + 2, "Unexpected token '"s + str[pos + 2] + "', expected ';'"s);
                            if (isByteArray) nbtStack.top().get().emplace<TagList>(nbt_list::vector<nbt_byte>{});
                            else if (isIntArray) nbtStack.top().get().emplace<TagList>(nbt_list::vector<nbt_int>{});
                            else/* if (isLongArray)*/ nbtStack.top().get().emplace<TagList>(nbt_list::vector<nbt_long>{});
                            pos += 3;
                            parserState = ParserState::Value;
                            continue;
                        }
                    }
					nbtStack.top().get().emplace<TagList>();
					pos++;
					parserState = ParserState::Value;
					continue;
				}
				if ((currentChar == '\'') || (currentChar == '"')) {
					nbtStack.top().get().emplace<TagString>(parseString(str, pos, currentChar));
					nbtStackPop();
					parserState = ParserState::Comma;
					continue;
				}
				if ((currentChar == '+') || (currentChar == '-') ||
					((currentChar >= '0') && (currentChar <= '9')) ||
					(currentChar == nan_string[0]) || (currentChar == infinity_string[0])
				) {
					const auto parsedNumberVariant = parseNumber(str, pos);
                    std::visit([&nbtStack](const auto& parsedNumber) {
                        using T = std::remove_cv_t<std::remove_reference_t<decltype(parsedNumber)>>;
                        nbtStack.top().get().emplace<T>(parsedNumber);
                    }, parsedNumberVariant);
                    nbtStackPop();
                    parserState = ParserState::Comma;
					continue;
					//throw ParseError(str, pos, "Invalid number literal");
				}
				throw ParseError(str, pos, "Unexpected token '"s + currentChar + "', expected value"s);
			} else if (parserState == ParserState::Comma) {
				if (parsingCompound && (currentChar == '}')) {
					nbtStackPop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				if ((parsingList || parsingIntegerArray) && (currentChar == ']')) {
					nbtStackPop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				if (currentChar == ',') {
					pos++;
					if (parsingCompound) parserState = ParserState::Key;
					else if (parsingList || parsingIntegerArray) parserState = ParserState::Value;
					else throw ParseError(str, pos, "Invalid parser state (cannot be in state ParserState::Comma while outside of compound, list, or typed array)");
				} else {
					throw ParseError(str, pos, "Expected ',', found '"s + currentChar + "' instead"s);
				}
			} else {
				throw ParseError(str, pos, "Invalid parser state");
			}
		}
		if (!nbtStack.empty())
			throw ParseError(str, pos, "SNBT string ended unexpectedly");
		if (pos < str.size()) {
			pos = str.find_first_not_of(whitespace_chars, pos);
			if (pos != npos)
			    throw ParseError(str, pos, "Unexpected token '"s + str[pos] + "'");
		}
		return result;
	}

}