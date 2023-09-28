#include "NBTList.h"

#include <sstream>

#include "NBT.h"

namespace nbt {

    NBTList::NBTList(const vector<nbt_byte>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_byte>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_short>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_short>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_int>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_int>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_long>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_long>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_float>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_float>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_double>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_double>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_string>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_string>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_list>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_list>&& v) noexcept : data(std::move(v)) {}
    NBTList::NBTList(const vector<nbt_compound>& v) noexcept : data(v) {}
    NBTList::NBTList(vector<nbt_compound>&& v) noexcept : data(std::move(v)) {}

    NBTList& NBTList::operator =(const vector<nbt_byte>& v) noexcept {
        data.emplace<vector<nbt_byte>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_byte>&& v) noexcept {
        data.emplace<vector<nbt_byte>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_short>& v) noexcept {
        data.emplace<vector<nbt_short>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_short>&& v) noexcept {
        data.emplace<vector<nbt_short>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_int>& v) noexcept {
        data.emplace<vector<nbt_int>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_int>&& v) noexcept {
        data.emplace<vector<nbt_int>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_long>& v) noexcept {
        data.emplace<vector<nbt_long>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_long>&& v) noexcept {
        data.emplace<vector<nbt_long>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_float>& v) noexcept {
        data.emplace<vector<nbt_float>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_float>&& v) noexcept {
        data.emplace<vector<nbt_float>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_double>& v) noexcept {
        data.emplace<vector<nbt_double>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_double>&& v) noexcept {
        data.emplace<vector<nbt_double>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_string>& v) noexcept {
        data.emplace<vector<nbt_string>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_string>&& v) noexcept {
        data.emplace<vector<nbt_string>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_list>& v) noexcept {
        data.emplace<vector<nbt_list>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_list>&& v) noexcept {
        data.emplace<vector<nbt_list>>(std::move(v));
        return *this;
    }
    NBTList& NBTList::operator =(const vector<nbt_compound>& v) noexcept {
        data.emplace<vector<nbt_compound>>(v);
        return *this;
    }
    NBTList& NBTList::operator =(vector<nbt_compound>&& v) noexcept {
        data.emplace<vector<nbt_compound>>(std::move(v));
        return *this;
    }


    [[nodiscard]] bool NBTList::is(const TagType tagType) const noexcept {
        return getTagType() == tagType;
    }

    [[nodiscard]] std::vector<nbt_byte>& NBTList::asBytes() {
        if (isBytes()) return std::get<vector<nbt_byte>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_byte>& NBTList::asBytes() const {
        if (isBytes()) return std::get<vector<nbt_byte>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_short>& NBTList::asShorts() {
        if (isShorts()) return std::get<vector<nbt_short>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_short>& NBTList::asShorts() const {
        if (isShorts()) return std::get<vector<nbt_short>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_int>& NBTList::asInts() {
        if (isInts()) return std::get<vector<nbt_int>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_int>& NBTList::asInts() const {
        if (isInts()) return std::get<vector<nbt_int>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_long>& NBTList::asLongs() {
        if (isLongs()) return std::get<vector<nbt_long>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_long>& NBTList::asLongs() const {
        if (isLongs()) return std::get<vector<nbt_long>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_float>& NBTList::asFloats() {
        if (isFloats()) return std::get<vector<nbt_float>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_float>& NBTList::asFloats() const {
        if (isFloats()) return std::get<vector<nbt_float>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_double>& NBTList::asDoubles() {
        if (isDoubles()) return std::get<vector<nbt_double>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_double>& NBTList::asDoubles() const {
        if (isDoubles()) return std::get<vector<nbt_double>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_string>& NBTList::asStrings() {
        if (isStrings()) return std::get<vector<nbt_string>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_string>& NBTList::asStrings() const {
        if (isStrings()) return std::get<vector<nbt_string>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_list>& NBTList::asLists() {
        if (isLists()) return std::get<vector<nbt_list>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_list>& NBTList::asLists() const {
        if (isLists()) return std::get<vector<nbt_list>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_compound>& NBTList::asCompounds() {
        if (isCompounds()) return std::get<vector<nbt_compound>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] const std::vector<nbt_compound>& NBTList::asCompounds() const {
        if (isCompounds()) return std::get<vector<nbt_compound>>(data);
        throw bad_nbt_cast();
    }


    [[nodiscard]] std::vector<nbt_byte> NBTList::getBytes() const {
        if (isBytes()) return std::get<vector<nbt_byte>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_short> NBTList::getShorts() const {
        if (isShorts()) return std::get<vector<nbt_short>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_int> NBTList::getInts() const {
        if (isInts()) return std::get<vector<nbt_int>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_long> NBTList::getLongs() const {
        if (isLongs()) return std::get<vector<nbt_long>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_float> NBTList::getFloats() const {
        if (isFloats()) return std::get<vector<nbt_float>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_double> NBTList::getDoubles() const {
        if (isDoubles()) return std::get<vector<nbt_double>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_string> NBTList::getStrings() const {
        if (isStrings()) return std::get<vector<nbt_string>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_list> NBTList::getLists() const {
        if (isLists()) return std::get<vector<nbt_list>>(data);
        throw bad_nbt_cast();
    }
    [[nodiscard]] std::vector<nbt_compound> NBTList::getCompounds() const {
        if (isCompounds()) return std::get<vector<nbt_compound>>(data);
        throw bad_nbt_cast();
    }


    void NBTList::emplaceListOfTagType(const TagType tagType) {
        switch (tagType) {
            case TagByte:
                data.emplace<vector<nbt_type<TagByte>>>();
                break;
            case TagShort:
                data.emplace<vector<nbt_type<TagShort>>>();
                break;
            case TagInt:
                data.emplace<vector<nbt_type<TagInt>>>();
                break;
            case TagLong:
                data.emplace<vector<nbt_type<TagLong>>>();
                break;
            case TagFloat:
                data.emplace<vector<nbt_type<TagFloat>>>();
                break;
            case TagDouble:
                data.emplace<vector<nbt_type<TagDouble>>>();
                break;
            case TagString:
                data.emplace<vector<nbt_type<TagString>>>();
                break;
            case TagList:
                data.emplace<vector<nbt_type<TagList>>>();
                break;
            case TagCompound:
                data.emplace<vector<nbt_type<TagCompound>>>();
                break;
            default:
                throw "Invalid tag type";
        }
    }


    void NBTList::resize(size_t count) {
        std::visit([&count](auto& list) { return list.resize(count); }, data);
    }

    [[nodiscard]] bool NBTList::operator ==(const NBTList& b) const noexcept { return data == b.data; }
	[[nodiscard]] bool NBTList::operator !=(const NBTList& b) const noexcept { return data != b.data; }

    [[nodiscard]] NBT NBTList::getElement(size_t i) const {
        return std::visit<NBT>([&i](const auto& list) -> NBT { return list.at(i); }, data);
    }


    std::string NBTList::toSNBT(const size_t indentAmount, const char indentChar, const size_t indentLevel) const {
        const auto innerIndentLevel = indentLevel + 1;
		const bool shouldInline = ((size() <= 4) && (isBytes() || isShorts() || isInts() || isLongs() || isFloats() || isDoubles())) || !indentAmount;

        return std::visit<std::string>([&](const auto& list) -> std::string {
            using T = std::decay_t<decltype(list)>;
            std::ostringstream sstr;
            sstr.put('[');
            for (const auto& val : list) {
                if (shouldInline) {
                    sstr.put(' ');
                } else if (indentAmount) {
                    sstr.put('\n');
                    for (size_t i = 0; i < innerIndentLevel * indentAmount; i++) sstr.put(indentChar);
                }
                if constexpr (std::is_same_v<T, nbt_compound> || std::is_same_v<T, nbt_list>)
                    sstr << val.toSNBT(indentAmount, indentChar, innerIndentLevel) << ',';
                else
                    sstr << NBT(val).toSNBT(indentAmount, indentChar, innerIndentLevel) << ',';
            }
            if (!list.empty()) {
                if (shouldInline) {
                    sstr.put(' ');
                } else if (indentAmount) {
                    sstr.put('\n');
                    for (size_t i = 0; i < (indentLevel * indentAmount); i++) sstr.put(indentChar);
                }
            }
            sstr.put(']');
            return sstr.str();
        }, data);
    }

    std::ostream& operator <<(std::ostream& os, const NBTList& j) {
		return os << j.toSNBT();
	}

}