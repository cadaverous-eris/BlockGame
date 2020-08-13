#include "NBTCompound.h"

#include <utility>
#include <sstream>

#include "NBT.h"

namespace nbt {

    NBTCompound::NBTCompound(std::initializer_list<NBTCompound::entry_type> initList) : data(initList) {}
	NBTCompound::NBTCompound(const NBTCompound::map_type& map) : data(map) {}
	NBTCompound::NBTCompound(NBTCompound::map_type&& map) : data(std::move(map)) {}

	NBTCompound& NBTCompound::operator =(std::initializer_list<NBTCompound::entry_type> initList) {
		data = initList;
		return *this;
	}

	bool NBTCompound::operator ==(const NBTCompound& b) const { return data == b.data; }
	bool NBTCompound::operator !=(const NBTCompound& b) const { return data != b.data; }

    bool NBTCompound::empty() const noexcept { return data.empty(); }
	NBTCompound::size_type NBTCompound::size() const noexcept { return data.size(); }
	NBTCompound::size_type NBTCompound::max_size() const noexcept { return data.max_size(); }
	NBTCompound::size_type NBTCompound::maxSize() const noexcept { return data.max_size(); }

	bool NBTCompound::hasKey(const nbt_string_view key) const {
		return data.find(key) != data.end();
	}
	bool NBTCompound::hasKey(const nbt_string_view key, const TagType tagType) const {
		const auto it = data.find(key);
		return (it != data.end()) && it->second.is(tagType);
	}

	NBT& NBTCompound::operator[](const NBTCompound::key_type& key) { return data[key]; }

	NBT& NBTCompound::at(const NBTCompound::key_type& key) { return data.at(key); }
	const NBT& NBTCompound::at(const NBTCompound::key_type& key) const { return data.at(key); }

    const NBT& NBTCompound::get(const NBTCompound::key_type& key) const { return data.at(key); }

    NBTCompound::iterator NBTCompound::find(const NBTCompound::key_type& key) {
		return data.find(key);
	}
	NBTCompound::const_iterator NBTCompound::find(const NBTCompound::key_type& key) const {
		return data.find(key);
	}

	void NBTCompound::clear() noexcept { data.clear(); }
	NBTCompound::iterator NBTCompound::erase(NBTCompound::iterator pos) noexcept { return data.erase(pos); }
	NBTCompound::iterator NBTCompound::erase(NBTCompound::const_iterator first, NBTCompound::const_iterator last) noexcept {
		return data.erase(first, last);
	}
	NBTCompound::size_type NBTCompound::erase(const NBTCompound::key_type& key) { return data.erase(key); }


    void NBTCompound::swap(NBTCompound& other) noexcept(std::allocator_traits<NBTCompound::allocator_type>::is_always_equal::value) {
		data.swap(other.data);
	}

	NBTCompound::iterator NBTCompound::begin() noexcept { return data.begin(); }
	NBTCompound::const_iterator NBTCompound::begin() const noexcept { return data.begin(); }
	NBTCompound::const_iterator NBTCompound::cbegin() const noexcept { return data.cbegin(); }
	NBTCompound::reverse_iterator NBTCompound::rbegin() noexcept { return data.rbegin(); }
	NBTCompound::const_reverse_iterator NBTCompound::rbegin() const noexcept { return data.rbegin(); }
	NBTCompound::const_reverse_iterator NBTCompound::crbegin() const noexcept { return data.crbegin(); }
	NBTCompound::iterator NBTCompound::end() noexcept { return data.end(); }
	NBTCompound::const_iterator NBTCompound::end() const noexcept { return data.end(); }
	NBTCompound::const_iterator NBTCompound::cend() const noexcept { return data.cend(); }
	NBTCompound::reverse_iterator NBTCompound::rend() { return data.rend(); }
	NBTCompound::const_reverse_iterator NBTCompound::rend() const noexcept { return data.rend(); }
	NBTCompound::const_reverse_iterator NBTCompound::crend() const noexcept { return data.crend(); }


    static std::string stringifyKey(const nbt_string_view str) {
		bool needsQuotes = false;
		std::string result {};
		result.reserve(str.size());
		for (size_t i = 0; i < str.size(); i++) {
			if (str[i] == '"') {
				result += '\\';
				result += '"';
				needsQuotes = true;
			} else if (str[i] == '\'') {
				result += '\'';
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

    std::string NBTCompound::toSNBT(const size_t indentAmount, const char indentChar, const size_t indentLevel) const {
		using namespace std::string_view_literals;
		const auto innerIndentLevel = indentLevel + 1;

		std::ostringstream sstr {};
		sstr.put('{');
		for (auto& [key, val] : data) {
			if (indentAmount) {
				sstr.put('\n');
				for (size_t i = 0; i < (innerIndentLevel * indentAmount); i++) sstr.put(indentChar);
			} else {
				sstr.put(' ');
			}
			sstr << stringifyKey(key) << ": "sv << val.toSNBT(indentAmount, indentChar, innerIndentLevel) << ',';
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

}