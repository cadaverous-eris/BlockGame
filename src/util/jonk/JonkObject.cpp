#include "JonkObject.h"

#include <utility>
#include <sstream>

#include "Jonk.h"

namespace jonk {

	bool JonkObject::KeyCompare::operator ()(const std::string& lhs, const std::string& rhs) const noexcept {
		return lhs < rhs;
	}
	bool JonkObject::KeyCompare::operator ()(const std::string_view lhs, const std::string& rhs) const noexcept {
		return lhs < rhs;
	}
	bool JonkObject::KeyCompare::operator ()(const std::string& lhs, const std::string_view rhs) const noexcept {
		return lhs < rhs;
	}

	JonkObject::JonkObject(std::initializer_list<JonkObject::entry_type> initList) : data(initList) {}
	JonkObject::JonkObject(const JonkObject::map_type& map) : data(map) {}
	JonkObject::JonkObject(JonkObject::map_type&& map) : data(std::move(map)) {}

	JonkObject& JonkObject::operator =(std::initializer_list<JonkObject::entry_type> initList) {
		data = initList;
		return *this;
	}

	bool JonkObject::operator ==(const JonkObject& b) const { return data == b.data; }
	bool JonkObject::operator !=(const JonkObject& b) const { return data != b.data; }

	bool JonkObject::empty() const noexcept { return data.empty(); }
	JonkObject::size_type JonkObject::size() const noexcept { return data.size(); }
	JonkObject::size_type JonkObject::max_size() const noexcept { return data.max_size(); }
	JonkObject::size_type JonkObject::maxSize() const noexcept { return data.max_size(); }

	bool JonkObject::hasKey(const std::string_view key) const {
		return data.find(key) != data.end();
	}
	bool JonkObject::hasKey(const std::string_view key, const JonkType jonkType) const {
		const auto it = data.find(key);
		return (it != data.end()) && it->second.is(jonkType);
	}

	Jonk& JonkObject::operator[](const JonkObject::key_type& key) { return data[key]; }

	Jonk& JonkObject::at(const JonkObject::key_type& key) { return data.at(key); }
	const Jonk& JonkObject::at(const JonkObject::key_type& key) const { return data.at(key); }
	/*Jonk& JonkObject::at(const std::string_view key) {
		auto it = data.find(key);
		if (it != data.end()) return it->second;
		using namespace std::string_literals;
		throw std::out_of_range("JonkObject::at"s);
	}
	const Jonk& JonkObject::at(const std::string_view key) const {
		auto it = data.find(key);
		if (it != data.end()) return it->second;
		using namespace std::string_literals;
		throw std::out_of_range("JonkObject::at"s);
	}*/
	const Jonk& JonkObject::get(const JonkObject::key_type& key) const { return data.at(key); }
	/*Jonk& JonkObject::get(const std::string_view key) {
		auto it = data.find(key);
		if (it != data.end()) return it->second;
		using namespace std::string_literals;
		throw std::out_of_range("JonkObject::get"s);
	}
	const Jonk& JonkObject::get(const std::string_view key) const {
		auto it = data.find(key);
		if (it != data.end()) return it->second;
		using namespace std::string_literals;
		throw std::out_of_range("JonkObject::get"s);
	}*/

	JonkObject::iterator JonkObject::find(const JonkObject::key_type& key) {
		return data.find(key);
	}
	JonkObject::const_iterator JonkObject::find(const JonkObject::key_type& key) const {
		return data.find(key);
	}

	void JonkObject::clear() noexcept { data.clear(); }
	JonkObject::iterator JonkObject::erase(JonkObject::iterator pos) noexcept { return data.erase(pos); }
	JonkObject::iterator JonkObject::erase(JonkObject::const_iterator first, JonkObject::const_iterator last) noexcept {
		return data.erase(first, last);
	}
	JonkObject::size_type JonkObject::erase(const JonkObject::key_type& key) { return data.erase(key); }




	void JonkObject::swap(JonkObject& other) noexcept(std::allocator_traits<JonkObject::allocator_type>::is_always_equal::value) {
		data.swap(other.data);
	}

	JonkObject::iterator JonkObject::begin() noexcept { return data.begin(); }
	JonkObject::const_iterator JonkObject::begin() const noexcept { return data.begin(); }
	JonkObject::const_iterator JonkObject::cbegin() const noexcept { return data.cbegin(); }
	JonkObject::reverse_iterator JonkObject::rbegin() noexcept { return data.rbegin(); }
	JonkObject::const_reverse_iterator JonkObject::rbegin() const noexcept { return data.rbegin(); }
	JonkObject::const_reverse_iterator JonkObject::crbegin() const noexcept { return data.crbegin(); }
	JonkObject::iterator JonkObject::end() noexcept { return data.end(); }
	JonkObject::const_iterator JonkObject::end() const noexcept { return data.end(); }
	JonkObject::const_iterator JonkObject::cend() const noexcept { return data.cend(); }
	JonkObject::reverse_iterator JonkObject::rend() { return data.rend(); }
	JonkObject::const_reverse_iterator JonkObject::rend() const noexcept { return data.rend(); }
	JonkObject::const_reverse_iterator JonkObject::crend() const noexcept { return data.crend(); }



	static std::string stringifyKey(const std::string_view str) {
		bool needsQuotes = false;
		std::string result {};
		result.reserve(str.size());
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				result += '\\';
				result += '"';
				needsQuotes = true;
			} else if (str[i] == '\'') {
				result += '\\';
				result += '\'';
				needsQuotes = true;
			} else if (str[i] == '`') {
				result += '\\';
				result += '`';
				needsQuotes = true;
			} else if (str[i] == '\\') {
				result += '\\';
				result += '\\';
				needsQuotes = true;
			} else if (str[i] == '\a') {
				result += '\\';
				result += 'a';
				needsQuotes = true;
			} else if (str[i] == '\b') {
				result += '\\';
				result += 'b';
				needsQuotes = true;
			} else if (str[i] == '\t') {
				result += '\\';
				result += 't';
				needsQuotes = true;
			} else if (str[i] == '\n') {
				result += '\\';
				result += 'n';
				needsQuotes = true;
			} else if (str[i] == '\v') {
				result += '\\';
				result += 'v';
				needsQuotes = true;
			} else if (str[i] == '\f') {
				result += '\\';
				result += 'f';
				needsQuotes = true;
			} else if (str[i] == '\r') {
				result += '\\';
				result += 'r';
				needsQuotes = true;
			} else if (str[i] == '\0') {
				result += '\\';
				result += '0';
				needsQuotes = true;
			} else if (str[i] == ' ') {
				result += ' ';
				needsQuotes = true;
			} else {
				result += str[i];
			}
		}
		return needsQuotes ? ('"' + result + '"') : result;
	}

	std::string JonkObject::toJonkString(const size_t indentAmount, const char indentChar, const size_t indentLevel) const {
		using namespace std::string_view_literals;
		const auto innerIndentLevel = indentLevel + 1;

		std::ostringstream sstr {};
		sstr.put('{');
		for (auto& [key, val] : data) {
			if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < innerIndentLevel * indentAmount; i++) sstr.put(indentChar);
			} else {
				sstr.put(' ');
			}
			sstr << stringifyKey(key) << ": "sv << val.toJonkString(indentAmount, indentChar, innerIndentLevel) << ","sv;
		}
		if (!data.empty()) {
			if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < indentLevel * indentAmount; i++) sstr.put(indentChar);
			} else {
				sstr.put(' ');
			}
		}
		sstr.put('}');
		return sstr.str();
	}

	std::ostream& operator <<(std::ostream& os, const JonkObject& obj) {
		return os << obj.toJonkString();
	}

}