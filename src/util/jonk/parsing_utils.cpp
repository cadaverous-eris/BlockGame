#include "parsing_utils.h"

#include <map>
#include <vector>
#include <variant>
#include <stack>
#include <functional>
#include <limits>
#include <cmath>
#include <sstream>

#include "Jonk.h"
#include "util/text/text_utils.h"
#include "util/text/unicode_utils.h"

namespace jonk::parsing {

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
		std::ostringstream sstr {};
		sstr << what << '\n';
		sstr << "(line " << errorPos.line << ", col " << errorPos.col << ')';
		return sstr.str();
	}
	ParseError::ParseError(const std::string_view str, const size_t pos, const std::string& what) :
			std::runtime_error(createParseErrorString(str, pos, what)) {}

	static constexpr std::string_view whitespace_chars = "\x09\x0A\x0B\x0C\x0D\x20";

	std::string parseString(const std::string_view str, size_t& pos, const char quoteChar = '"', const bool allowNewlines = true) {
		using namespace std::string_literals;
		const bool eatNewLines = quoteChar != '`';
		std::string result {};
		pos = str.find_first_of(quoteChar, pos);
		if (pos == std::string_view::npos)
			throw ParseError(str, pos, "Jonk string literal is missing an opening "s + quoteChar);
		pos++;
		for (size_t cursor = pos; ; cursor++) {
			if (cursor >= str.size())
				throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
			if (str[cursor] == '\\') { // escape sequence
				if (cursor > pos) result.append(str, pos, cursor - pos);
				cursor++;
				if (cursor >= str.size())
					throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
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
				case '`':
					result += '`';
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
							throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
						char nextChar = str[cursor + 1];
						if ((nextChar >= '0') && (nextChar <= '7')) {
							oct = (oct << 3) | (nextChar - '0');
							cursor++;
						} else {
							break;
						}
					}
					result += oct;
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
								throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
							const char hexChar = str[cursor];
							if ((hexChar >= '0') && (hexChar <= '9'))
								hex = (hex << 4) | (hexChar - '0');
							else if ((hexChar >= 'A') && (hexChar <= 'F'))
								hex = (hex << 4) | (10 + hexChar - 'A');
							else if ((hexChar >= 'a') && (hexChar <= 'f'))
								hex = (hex << 4) | (10 + hexChar - 'a');
							else
								throw ParseError(str, cursor, "Invalid hex literal");
						}
						result += hex;
						pos = cursor + 1;
					}
					break;
				case 'u':
					{
						char32_t codepoint { 0 };
						for (size_t digits = 0; digits < 4; digits++) {
							cursor++;
							if (cursor >= str.size())
								throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
							const char hexChar = str[cursor];
							if ((hexChar >= '0') && (hexChar <= '9'))
								codepoint = (codepoint << 4) | (hexChar - '0');
							else if ((hexChar >= 'A') && (hexChar <= 'F'))
								codepoint = (codepoint << 4) | (10 + hexChar - 'A');
							else if ((hexChar >= 'a') && (hexChar <= 'f'))
								codepoint = (codepoint << 4) | (10 + hexChar - 'a');
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
								throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
							const char hexChar = str[cursor];
							if ((hexChar >= '0') && (hexChar <= '9'))
								codepoint = (codepoint << 4) | (hexChar - '0');
							else if ((hexChar >= 'A') && (hexChar <= 'F'))
								codepoint = (codepoint << 4) | (10 + hexChar - 'A');
							else if ((hexChar >= 'a') && (hexChar <= 'f'))
								codepoint = (codepoint << 4) | (10 + hexChar - 'a');
							else
								throw ParseError(str, cursor, "Invalid 32-bit unicode literal");
						}
						eng::unicode::appendUtf32CharToUtf8(result, codepoint);
						pos = cursor + 1;
					}
					break;
				}
			} else if (str[cursor] == '\n') {
				if (!allowNewlines) throw ParseError(str, cursor, "Unexpected char '\\n'");
				if (eatNewLines) {
					if (cursor > pos) result.append(str, pos, cursor - pos);
					while (eng::isWhitespace(str[cursor + 1])) {
						cursor++;
						if (cursor >= str.size())
							throw ParseError(str, cursor, "Jonk string literal is missing a closing "s + quoteChar);
					}
					pos = cursor + 1;
				}
			} else if (str[cursor] == quoteChar) {
				if (cursor > pos) result.append(str, pos, cursor - pos);
				pos = cursor + 1;
				return result;
			}
		}
	}

	std::string parseKey(const std::string_view str, size_t& pos) {
		using namespace std::string_view_literals;
		constexpr std::string_view whitespace_colon_chars = "\x09\x0A\x0B\x0C\x0D\x20:"sv;
		pos = str.find_first_not_of(whitespace_chars, pos);
		if (pos == std::string_view::npos)
			throw ParseError(str, pos, "Could not find jonk object key");
		const char firstChar = str[pos];
		if (firstChar == '\'' || firstChar == '"' || firstChar == '`') {
			std::string result = parseString(str, pos, firstChar, false);
			if (result.empty())
				throw ParseError(str, pos, "Could not find jonk object key");
			return result;
		}
		auto endPos = str.find_first_of(whitespace_colon_chars, pos);
		if (endPos == std::string_view::npos) endPos = str.size();
		std::string result { str.substr(pos, endPos - pos) };
		pos = endPos;
		if (result.empty())
			throw ParseError(str, pos, "Could not find jonk object key");
		return result;
	}

	std::variant<JonkInt, JonkUint, JonkFloat> parseNumber(const std::string_view str, size_t& pos) {
		using namespace std::string_literals;
		using namespace std::string_view_literals;
		constexpr auto npos = std::string_view::npos;
		constexpr std::string_view whitespace_comma_chars = "\x09\x0A\x0B\x0C\x0D\x20,";

		size_t endIndex = str.find_first_of(whitespace_comma_chars, pos);
		if (endIndex == npos) endIndex = str.size();

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

		if (valString == nan_string) {
			pos = endIndex;
			JonkFloat value = std::numeric_limits<JonkFloat>::quiet_NaN();
			return isNegative ? -value : value;
		}
		if (valString == infinity_string) {
			pos = endIndex;
			JonkFloat value = std::numeric_limits<JonkFloat>::infinity();
			return isNegative ? -value : value;
		}
		if (valString.starts_with("0o"sv) || valString.starts_with("0O"sv)) { // octal literal
			pos += 2;
			if (pos >= str.size()) throw ParseError(str, pos, "Jonk string ended unexpectedly (incomplete octal literal)");
			JonkUint value { 0 };
			bool wasSeparator = false;
			size_t i = pos, digits = 0;
			for (; ; i++) {
				if (i >= str.size()) {
					if (digits == 0) throw ParseError(str, i, "Jonk string ended unexpectedly (incomplete octal literal)");
					else break;
				}
				const char digitChar = str[i];
				if (digitChar == '_') {
					if (wasSeparator || (digits == 0))
						throw ParseError(str, i, "Unexpected token '_', expected octal digit");
					wasSeparator = true;
				} else if (digitChar >= '0' && digitChar <= '7') {
					digits++;
					wasSeparator = false;
					value <<= 3;
					value |= digitChar - '0';
				} else {
					break;
				}
			}
			pos = (wasSeparator) ? i - 1 : i;
			if (digits == 0)
				throw ParseError(str, pos, "Unexpected token '"s + str[pos] + "', expected octal digit"s);
			if (hasSign) {
				return (isNegative) ? -static_cast<JonkInt>(value) : static_cast<JonkInt>(value);
			}
			return value;
		}
		if (valString.starts_with("0x"sv) || valString.starts_with("0X"sv)) { // hex literal
			pos += 2;
			if (pos >= str.size()) throw ParseError(str, pos, "Jonk string ended unexpectedly (incomplete hexadecimal literal))");
			JonkUint value { 0 };
			bool wasSeparator = false;
			size_t i = pos, digits = 0;
			for (; ; i++) {
				if (i >= str.size()) {
					if (digits == 0) throw ParseError(str, i, "Jonk string ended unexpectedly (incomplete hexadecimal literal))");
					else break;
				}
				const char digitChar = str[i];
				if (digitChar == '_') {
					if (wasSeparator || (digits == 0))
						throw ParseError(str, i, "Unexpected token '_', expected hexadecimal digit");
					wasSeparator = true;
				} else if (digitChar >= '0' && digitChar <= '9') {
					digits++;
					wasSeparator = false;
					value <<= 4;
					value |= digitChar - '0';
				} else if (digitChar >= 'A' && digitChar <= 'F') {
					digits++;
					wasSeparator = false;
					value <<= 4;
					value |= 10 + (digitChar - 'A');
				} else if (digitChar >= 'a' && digitChar <= 'f') {
					digits++;
					wasSeparator = false;
					value <<= 4;
					value |= 10 + (digitChar - 'a');
				} else {
					break;
				}
			}
			pos = (wasSeparator) ? i - 1 : i;
			if (digits == 0)
				throw ParseError(str, pos, "Unexpected token '"s + str[pos] + "', expected hexadecimal digit"s);
			if (hasSign) {
				return (isNegative) ? -static_cast<JonkInt>(value) : static_cast<JonkInt>(value);
			}
			return value;
		}

		bool hasDecimal = false, hasExp = false, hasExpSign = false, isExpNegative = false;
		JonkInt value { 0 };
		JonkFloat floatValue { 0 };
		int16_t expValue { 0 };
		char prevChar { 0 };
		size_t i = pos, digits = 0, fractDigits = 0, expDigits = 0;
		for (; ; i++) {
			if (i >= str.size()) {
				if (digits == 0) throw ParseError(str, i, "Jonk string ended unexpectedly (incomplete numeric literal)");
				else break;
			}
			const char digitChar = str[i];
			if (digitChar == '_') {
				if ((digits == 0) || (prevChar == '_'))
					throw ParseError(str, i, "Unexpected token '_', expected decimal digit");
				prevChar = digitChar;
			} else if (digitChar == '.') {
				if (prevChar == '_')
					throw ParseError(str, i, "Unexpected token '.', expected decimal digit");
				hasDecimal = true;
				pos = i + 1;
				prevChar = digitChar;
				break;
			} else if ((digitChar == 'E') || (digitChar == 'e')) {
				if ((digits == 0) || (prevChar == '_'))
					throw ParseError(str, i, "Unexpected token '"s + digitChar + "', expected decimal digit"s);
				hasExp = true;
				pos = i + 1;
				prevChar = digitChar;
				break;
			} else if ((digitChar >= '0') && (digitChar <= '9')) {
				digits++;
				value = (value * 10) + (digitChar - '0');
				floatValue = (floatValue * 10.0) + (digitChar - '0');
				prevChar = digitChar;
			} else {
				break;
			}
		}
		if (hasDecimal) {
			for (i = pos; ; i++) {
				if (i >= str.size()) {
					if ((digits == 0) && (fractDigits == 0)) throw ParseError(str, i, "Jonk string ended unexpectedly (incomplete numeric literal)");
					else break;
				}
				const char digitChar = str[i];
				if (digitChar == '_') {
					if ((fractDigits == 0) || (prevChar == '_'))
						throw ParseError(str, i, "Unexpected token '_', expected decimal digit");
					prevChar = digitChar;
				} else if ((digitChar == 'E') || (digitChar == 'e')) {
					if (((fractDigits == 0) && (digits == 0)) || (prevChar == '_'))
						throw ParseError(str, i, "Unexpected token '"s + digitChar + "', expected decimal digit"s);
					hasExp = true;
					pos = i + 1;
					prevChar = digitChar;
					break;
				} else if ((digitChar >= '0') && (digitChar <= '9')) {
					fractDigits++;
					floatValue += static_cast<JonkFloat>(digitChar - '0') / std::pow(JonkFloat{ 10 }, fractDigits);
					prevChar = digitChar;
				} else {
					break;
				}
			}
		}
		if (hasExp) {
			for (i = pos; ; i++) {
				if (i >= str.size()) {
					if (expDigits == 0) throw ParseError(str, i, "Jonk string ended unexpectedly (incomplete numeric literal)");
					else break;
				}
				const char digitChar = str[i];
				if (digitChar == '_') {
					if ((expDigits == 0) || (prevChar == '_'))
						throw ParseError(str, i, "Unexpected token '_', expected decimal digit");
					prevChar = digitChar;
				} else if (!hasExpSign && (expDigits == 0) && (digitChar == '+')) {
					hasExpSign = true;
					prevChar = digitChar;
				} else if (!hasExpSign && (expDigits == 0) && (digitChar == '-')) {
					hasExpSign = true;
					isExpNegative = true;
					prevChar = digitChar;
				} else if ((digitChar >= '0') && (digitChar <= '9')) {
					expDigits++;
					expValue = (expValue * 10) + (digitChar - '0');
					prevChar = digitChar;
				} else {
					break;
				}
			}
			if (expDigits > 0) {
				if (isExpNegative) expValue = -expValue;
				floatValue *= std::pow(JonkFloat{ 10.0 }, expValue);
			}
		}
		pos = (prevChar == '_') ? i - 1 : i;
		if (((digits == 0) && (fractDigits == 0)) || (hasExp && (expDigits == 0)))
			throw ParseError(str, i, "Unexpected token '"s + str[pos] + "', expected decimal digit"s);
		if (hasDecimal || hasExp) {
			return (isNegative) ? -floatValue : floatValue;
		} else {
			return (isNegative) ? -value : value;
		}
		return value;
	}


	Jonk parseJonk(std::string_view str) {
		using namespace std::string_literals;
		using namespace std::string_view_literals;
		using std::stack, std::variant, std::reference_wrapper;
		constexpr auto npos = std::string_view::npos;
		enum class ParserState { Key, Colon, Value, Comma, };
		
		Jonk result {};
		stack<reference_wrapper<Jonk>> jonkStack { { result } };
		ParserState parserState = ParserState::Value;
		size_t pos = 0;
		while ((pos != npos) && (pos < str.size()) && !jonkStack.empty()) {
			pos = str.find_first_not_of(whitespace_chars, pos);
			if (pos == std::string_view::npos) break;
			const char currentChar = str[pos];
			const bool parsingObject = jonkStack.top().get().is<JonkType::Object>();
			const bool parsingArray = jonkStack.top().get().is<JonkType::Array>();

			if (parserState == ParserState::Key) {
				if (currentChar == '}') {
					if (!parsingObject)
						throw ParseError(str, pos, "Unexpected token '}', expected key");
					jonkStack.pop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				std::string key = parseKey(str, pos);
				JonkObject& obj = jonkStack.top().get().as<JonkType::Object>();
				auto [it, emplaced] = obj.emplace(std::move(key));
				if (!emplaced) throw ParseError(str, pos, "Duplicate key: \""s + it->first + "\""s);
				jonkStack.push(it->second);
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
					if (!parsingArray)
						throw ParseError(str, pos, "Unexpected token ']', expected value");
					jonkStack.pop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				if (parsingArray) {
					JonkArray& arr = jonkStack.top().get().as<JonkType::Array>();
					jonkStack.push(arr.emplace_back());
				}

				if (currentChar == '{') {
					jonkStack.top().get().emplace<JonkType::Object>();
					pos++;
					parserState = ParserState::Key;
					continue;
				}
				if (currentChar == '[') {
					jonkStack.top().get().emplace<JonkType::Array>();
					pos++;
					parserState = ParserState::Value;
					continue;
				}
				if ((currentChar == '\'') || (currentChar == '"') || (currentChar == '`')) {
					jonkStack.top().get().emplace<JonkType::String>(parseString(str, pos, currentChar));
					jonkStack.pop();
					parserState = ParserState::Comma;
					continue;
				}
				std::string_view remainingStr = str.substr(pos);
				if (remainingStr.starts_with(null_string)) {
					jonkStack.top().get().emplace<JonkType::Null>();
					jonkStack.pop();
					pos += null_string.size();
					parserState = ParserState::Comma;
					continue;
				}
				if (remainingStr.starts_with(true_string)) {
					jonkStack.top().get().emplace<JonkType::Bool>(true);
					jonkStack.pop();
					pos += null_string.size();
					parserState = ParserState::Comma;
					continue;
				}
				if (remainingStr.starts_with(false_string)) {
					jonkStack.top().get().emplace<JonkType::Bool>(false);
					jonkStack.pop();
					pos += null_string.size();
					parserState = ParserState::Comma;
					continue;
				}
				if (currentChar == '+' || currentChar == '-' || (currentChar >= '0' && currentChar <= '9')) {
					const auto parsedNumber = parseNumber(str, pos);
					if (std::holds_alternative<JonkInt>(parsedNumber)) {
						jonkStack.top().get().emplace<JonkInt>(std::get<JonkInt>(parsedNumber));
						jonkStack.pop();
						parserState = ParserState::Comma;
						continue;
					}
					if (std::holds_alternative<JonkUint>(parsedNumber)) {
						jonkStack.top().get().emplace<JonkUint>(std::get<JonkUint>(parsedNumber));
						jonkStack.pop();
						parserState = ParserState::Comma;
						continue;
					}
					if (std::holds_alternative<JonkFloat>(parsedNumber)) {
						jonkStack.top().get().emplace<JonkFloat>(std::get<JonkFloat>(parsedNumber));
						jonkStack.pop();
						parserState = ParserState::Comma;
						continue;
					}
					throw ParseError(str, pos, "Invalid number literal");
				}
				throw ParseError(str, pos, "Unexpected token '"s + currentChar + "', expected value"s);
			} else if (parserState == ParserState::Comma) {
				if (parsingObject && currentChar == '}') {
					jonkStack.pop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				if (parsingArray && currentChar == ']') {
					jonkStack.pop();
					pos++;
					parserState = ParserState::Comma;
					continue;
				}
				if (currentChar == ',') {
					pos++;
					if (parsingObject) parserState = ParserState::Key;
					else if (parsingArray) parserState = ParserState::Value;
					else throw ParseError(str, pos, "Invalid parser state (cannot be in state ParserState::Comma while outside of object or array)");
				} else {
					throw ParseError(str, pos, "Expected ',', found '"s + currentChar + "' instead"s);
				}
			} else {
				throw ParseError(str, pos, "Invalid parser state");
			}
		}
		if (!jonkStack.empty())
			throw ParseError(str, pos, "Jonk string ended unexpectedly");
		if (pos < str.size()) {
			pos = str.find_first_not_of(whitespace_chars, pos);
			if (pos != npos) {
				/*if (false && (parserState == ParserState::Comma) && (str[pos] == ',')) {
					pos++;
					if (pos < str.size()) {
						pos = str.find_first_not_of(whitespace_chars, pos);
						if (pos != npos) {
							throw ParseError(str, pos, "Unexpected token '"s + str[pos] + "'");
						}
					}
				} else {*/
					throw ParseError(str, pos, "Unexpected token '"s + str[pos] + "'");
				//}
			}
		}
		return result;
	}

}