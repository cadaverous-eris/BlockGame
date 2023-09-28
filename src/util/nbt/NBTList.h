#ifndef NBT_LIST_H
#define NBT_LIST_H

#include <variant>
#include <vector>
#include <optional>
#include <algorithm>
#include <iterator>

#include "nbt_types.h"
#include "NBTCompat.h"
#include "util/type_traits.h"

namespace nbt {

    class NBTList;
    std::ostream& operator <<(std::ostream&, const NBTList&);

    class NBT;

    class NBTList {
    public:
        template<typename T> using vector = std::vector<T>;
    private:
        using variant_type = std::variant<
			vector<nbt_byte>,
            vector<nbt_short>,
            vector<nbt_int>,
            vector<nbt_long>,
            vector<nbt_float>,
            vector<nbt_double>,
            vector<nbt_string>,
            vector<nbt_list>,
            vector<nbt_compound>
		>;

		variant_type data;

    public:
        template<TagType tagType> using iterator = typename vector<nbt_type<tagType>>::iterator;
        template<TagType tagType> using const_iterator = typename vector<nbt_type<tagType>>::const_iterator;
        template<TagType tagType> using reverse_iterator = typename vector<nbt_type<tagType>>::reverse_iterator;
        template<TagType tagType> using const_reverse_iterator = typename vector<nbt_type<tagType>>::const_reverse_iterator;

        NBTList() = default;

        NBTList(const NBTList&) = default;
        NBTList(NBTList&&) = default;

        NBTList(const vector<nbt_byte>&) noexcept;
        NBTList(vector<nbt_byte>&&) noexcept;
        NBTList(const vector<nbt_short>&) noexcept;
        NBTList(vector<nbt_short>&&) noexcept;
        NBTList(const vector<nbt_int>&) noexcept;
        NBTList(vector<nbt_int>&&) noexcept;
        NBTList(const vector<nbt_long>&) noexcept;
        NBTList(vector<nbt_long>&&) noexcept;
        NBTList(const vector<nbt_float>&) noexcept;
        NBTList(vector<nbt_float>&&) noexcept;
        NBTList(const vector<nbt_double>&) noexcept;
        NBTList(vector<nbt_double>&&) noexcept;
        NBTList(const vector<nbt_string>&) noexcept;
        NBTList(vector<nbt_string>&&) noexcept;
        NBTList(const vector<nbt_list>&) noexcept;
        NBTList(vector<nbt_list>&&) noexcept;
        NBTList(const vector<nbt_compound>&) noexcept;
        NBTList(vector<nbt_compound>&&) noexcept;

        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T>), int> = 0>
		explicit NBTList(const vector<T>& vec);
        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T>), int> = 0>
		explicit NBTList(vector<T>&& vec);
        template<typename Iter, typename IterVal = typename std::iterator_traits<Iter>::value_type, std::enable_if_t<(is_convertible_to_nbt_v<IterVal> || is_nbt_type_v<IterVal>), int> = 0>
        NBTList(Iter first, Iter last);
        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T> || is_nbt_type_v<T>), int> = 0>
        NBTList(std::initializer_list<T> initList);

        ~NBTList() = default;

        NBTList& operator =(const NBTList&) = default;
		NBTList& operator =(NBTList&&) = default;

        NBTList& operator =(const vector<nbt_byte>& b) noexcept;
        NBTList& operator =(vector<nbt_byte>&& b) noexcept;
        NBTList& operator =(const vector<nbt_short>& s) noexcept;
        NBTList& operator =(vector<nbt_short>&& s) noexcept;
        NBTList& operator =(const vector<nbt_int>& i) noexcept;
        NBTList& operator =(vector<nbt_int>&& i) noexcept;
        NBTList& operator =(const vector<nbt_long>& l) noexcept;
        NBTList& operator =(vector<nbt_long>&& l) noexcept;
        NBTList& operator =(const vector<nbt_float>& f) noexcept;
        NBTList& operator =(vector<nbt_float>&& f) noexcept;
        NBTList& operator =(const vector<nbt_double>& d) noexcept;
        NBTList& operator =(vector<nbt_double>&& d) noexcept;
        NBTList& operator =(const vector<nbt_string>& s) noexcept;
        NBTList& operator =(vector<nbt_string>&& s) noexcept;
        NBTList& operator =(const vector<nbt_list>& l) noexcept;
        NBTList& operator =(vector<nbt_list>&& l) noexcept;
        NBTList& operator =(const vector<nbt_compound>& c) noexcept;
        NBTList& operator =(vector<nbt_compound>&& c) noexcept;

        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T>), int> = 0>
		NBTList& operator =(const vector<T>& vec);
        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T>), int> = 0>
		NBTList& operator =(vector<T>&& vec);
        template<typename T, std::enable_if_t<(is_convertible_to_nbt_v<T> || is_nbt_type_v<T>), int> = 0>
        NBTList& operator =(std::initializer_list<T> initList);


        [[nodiscard]] inline bool isBytes() const noexcept { return is<TagByte>(); }
		[[nodiscard]] inline bool isShorts() const noexcept { return is<TagShort>(); }
        [[nodiscard]] inline bool isInts() const noexcept { return is<TagInt>(); }
        [[nodiscard]] inline bool isLongs() const noexcept { return is<TagLong>(); }
        [[nodiscard]] inline bool isFloats() const noexcept { return is<TagFloat>(); }
        [[nodiscard]] inline bool isDoubles() const noexcept { return is<TagDouble>(); }
        [[nodiscard]] inline bool isStrings() const noexcept { return is<TagString>(); }
        [[nodiscard]] inline bool isLists() const noexcept { return is<TagList>(); }
        [[nodiscard]] inline bool isCompounds() const noexcept { return is<TagCompound>(); }
		[[nodiscard]] inline bool isIntegrals() const noexcept { return isBytes() || isShorts() || isInts() || isLongs(); }
        [[nodiscard]] inline bool isFloatingPoints() const noexcept { return isFloats() || isDoubles(); }
		[[nodiscard]] inline bool isNumbers() const noexcept { return isIntegrals() || isFloatingPoints(); }
		[[nodiscard]] bool is(TagType tagType) const noexcept;
		template<TagType tagType, typename = nbt_type<tagType>>
		[[nodiscard]] inline bool is() const noexcept {
			if constexpr (tagType == TagEnd) return false;
			else return std::holds_alternative<vector<nbt_type<tagType>>>(data);
		}
		template<typename T>
		[[nodiscard]] std::enable_if_t<is_testable_nbt_type_v<T> || is_testable_nbt_list_type_v<T> || is_nbt_type_v<T>, bool> is() const noexcept;

        template<typename T>
        [[nodiscard]] std::enable_if_t<is_testable_nbt_type_v<T>, bool> isEach() const noexcept;

        [[nodiscard]] inline TagType getTagType() const noexcept {
            return tag_type_values[data.index()];
		}

        [[nodiscard]] inline variant_type& asListVariant() noexcept { return data; }
		[[nodiscard]] inline const variant_type& asListVariant() const noexcept { return data; }

        [[nodiscard]] vector<nbt_byte>& asBytes();
		[[nodiscard]] const vector<nbt_byte>& asBytes() const;
        [[nodiscard]] vector<nbt_short>& asShorts();
		[[nodiscard]] const vector<nbt_short>& asShorts() const;
		[[nodiscard]] vector<nbt_int>& asInts();
		[[nodiscard]] const vector<nbt_int>& asInts() const;
        [[nodiscard]] vector<nbt_long>& asLongs();
		[[nodiscard]] const vector<nbt_long>& asLongs() const;
        [[nodiscard]] vector<nbt_float>& asFloats();
		[[nodiscard]] const vector<nbt_float>& asFloats() const;
        [[nodiscard]] vector<nbt_double>& asDoubles();
		[[nodiscard]] const vector<nbt_double>& asDoubles() const;
        [[nodiscard]] vector<nbt_string>& asStrings();
		[[nodiscard]] const vector<nbt_string>& asStrings() const;
        [[nodiscard]] vector<nbt_list>& asLists();
		[[nodiscard]] const vector<nbt_list>& asLists() const;
        [[nodiscard]] vector<nbt_compound>& asCompounds();
		[[nodiscard]] const vector<nbt_compound>& asCompounds() const;
		template<TagType tagType, typename = nbt_type<tagType>>
		[[nodiscard]] inline vector<nbt_type<tagType>>& as() {
			if constexpr (tagType == TagByte) return asBytes();
			else if constexpr (tagType == TagShort) return asShorts();
			else if constexpr (tagType == TagInt) return asInts();
			else if constexpr (tagType == TagLong) return asLongs();
			else if constexpr (tagType == TagFloat) return asFloats();
			else if constexpr (tagType == TagDouble) return asDoubles();
			else if constexpr (tagType == TagString) return asStrings();
            else if constexpr (tagType == TagList) return asLists();
			else if constexpr (tagType == TagCompound) return asCompounds();
		}
		template<TagType tagType, typename = nbt_type<tagType>>
		[[nodiscard]] inline const vector<nbt_type<tagType>>& as() const {
			if constexpr (tagType == TagByte) return asBytes();
			else if constexpr (tagType == TagShort) return asShorts();
			else if constexpr (tagType == TagInt) return asInts();
			else if constexpr (tagType == TagLong) return asLongs();
			else if constexpr (tagType == TagFloat) return asFloats();
			else if constexpr (tagType == TagDouble) return asDoubles();
			else if constexpr (tagType == TagString) return asStrings();
            else if constexpr (tagType == TagList) return asLists();
			else if constexpr (tagType == TagCompound) return asCompounds();
		}

        [[nodiscard]] vector<nbt_byte> getBytes() const;
        [[nodiscard]] vector<nbt_short> getShorts() const;
		[[nodiscard]] vector<nbt_int> getInts() const;
        [[nodiscard]] vector<nbt_long> getLongs() const;
        [[nodiscard]] vector<nbt_float> getFloats() const;
        [[nodiscard]] vector<nbt_double> getDoubles() const;
        [[nodiscard]] vector<nbt_string> getStrings() const;
        [[nodiscard]] vector<nbt_list> getLists() const;
        [[nodiscard]] vector<nbt_compound> getCompounds() const;
		template<TagType tagType, typename = nbt_type<tagType>>
		[[nodiscard]] inline vector<nbt_type<tagType>> get() const {
			if constexpr (tagType == TagByte) return getBytes();
			else if constexpr (tagType == TagShort) return getShorts();
			else if constexpr (tagType == TagInt) return getInts();
			else if constexpr (tagType == TagLong) return getLongs();
			else if constexpr (tagType == TagFloat) return getFloats();
			else if constexpr (tagType == TagDouble) return getDoubles();
			else if constexpr (tagType == TagString) return getStrings();
            else if constexpr (tagType == TagList) return getLists();
			else if constexpr (tagType == TagCompound) return getCompounds();
		}
		template<typename T>
		[[nodiscard]] std::enable_if_t<is_convertible_from_nbt_v<T> || is_nbt_type_v<T>, vector<T>> get() const;

        template<typename T, std::enable_if_t<(((is_testable_nbt_type_v<T> || is_testable_nbt_list_type_v<T>) && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>), int> = 0>
		[[nodiscard]] inline vector<T> getOrDefault(const vector<T>& dflt) const noexcept {
			return is<T>() ? get<T>() : dflt;
		}
		template<typename T, std::enable_if_t<(((is_testable_nbt_type_v<T> || is_testable_nbt_list_type_v<T>) && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>), int> = 0>
		[[nodiscard]] inline vector<T> getOrDefault() const noexcept(std::is_nothrow_default_constructible_v<T>) {
			return is<T>() ? get<T>() : T();
		}
		template<typename T, std::enable_if_t<(((is_testable_nbt_type_v<T> || is_testable_nbt_list_type_v<T>) && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>), int> = 0>
		[[nodiscard]] inline std::optional<vector<T>> getOptional() const noexcept {
			return is<T>() ? std::make_optional<vector<T>>(get<T>()) : std::nullopt;
		}


        void emplaceListOfTagType(TagType tagType);
        template<typename T, typename... Args>
		std::enable_if_t<is_nbt_type_v<T>, vector<T>&> emplaceList(Args&&... args);
		template<typename T, typename U, typename... Args>
		std::enable_if_t<is_nbt_type_v<T>, vector<T>&> emplaceList(std::initializer_list<U> initList, Args&&... args);
		template<TagType tagType, typename... Args>
		vector<nbt_type<tagType>>& emplaceList(Args&&... args);
		template<TagType tagType, typename U, typename... Args>
		vector<nbt_type<tagType>>& emplaceList(std::initializer_list<U> initList, Args&&... args);

        [[nodiscard]] inline bool empty() const noexcept {
            return std::visit<bool>([](const auto& list) -> bool { return list.empty(); }, data);
        }
		inline size_t size() const noexcept {
            return std::visit<size_t>([](const auto& list) -> size_t { return list.size(); }, data);
        }
        inline size_t max_size() const noexcept {
            return std::visit<size_t>([](const auto& list) -> size_t { return list.max_size(); }, data);
        }
        inline void reserve(size_t newCapacity) {
            std::visit([&newCapacity](auto& list) { return list.reserve(newCapacity); }, data);
        }
        inline size_t capacity() const noexcept {
            return std::visit<size_t>([](const auto& list) -> size_t { return list.capacity(); }, data);
        }
        inline void shrink_to_fit() {
            std::visit([](auto& list) { return list.shrink_to_fit(); }, data);
        }

        inline void clear() noexcept {
            std::visit([](auto& list) { return list.clear(); }, data);
        }

        void resize(size_t count);

		[[nodiscard]] bool operator ==(const NBTList& b) const noexcept;
		[[nodiscard]] bool operator !=(const NBTList& b) const noexcept;

        [[nodiscard]] NBT getElement(size_t i) const;


        [[nodiscard]] explicit inline operator vector<nbt_byte>() const { return getBytes(); }
        [[nodiscard]] explicit inline operator vector<nbt_short>() const { return getShorts(); }
		[[nodiscard]] explicit inline operator vector<nbt_int>() const { return getInts(); }
        [[nodiscard]] explicit inline operator vector<nbt_long>() const { return getLongs(); }
        [[nodiscard]] explicit inline operator vector<nbt_float>() const { return getFloats(); }
        [[nodiscard]] explicit inline operator vector<nbt_double>() const { return getDoubles(); }
        [[nodiscard]] explicit inline operator vector<nbt_string>() const { return getStrings(); }
        [[nodiscard]] explicit inline operator vector<nbt_list>() const { return getLists(); }
        [[nodiscard]] explicit inline operator vector<nbt_compound>() const { return getCompounds(); }


        std::string toSNBT(const size_t indentAmount = 0, const char indentChar = ' ', const size_t indentLevel = 0) const;

    };



}

#endif

// Some templated member functions of NBTList are defined in NBT.h, so it must
// be included incase this header is included in a file that doesn't include NBT.h
#include "NBT.h"