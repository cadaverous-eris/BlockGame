#ifndef NBT_COMPOUND_H
#define NBT_COMPOUND_H

#include <map>
#include <iostream>
#include <optional>

#include "nbt_types.h"
#include "util/type_traits.h"
#include "NBTCompat.h"

namespace nbt {

    class NBTCompound;
    std::ostream& operator <<(std::ostream&, const NBTCompound&);

    class NBT;
    class NBTList;

    class NBTCompound {
    public:
        struct KeyCompare {
			using is_transparent = void;
			inline bool operator ()(const nbt_string& lhs, const nbt_string& rhs) const noexcept { return lhs < rhs; }
			inline bool operator ()(const nbt_string_view lhs, const nbt_string& rhs) const noexcept { return lhs < rhs; }
			inline bool operator ()(const nbt_string& lhs, const nbt_string_view rhs) const noexcept { return lhs < rhs; }
			constexpr bool operator ()(const nbt_string_view lhs, const nbt_string_view rhs) const noexcept { return lhs < rhs; }
		};
        using map_type = std::map<nbt_string, NBT, KeyCompare>;
		using key_type = typename map_type::key_type;
		using value_type = typename map_type::mapped_type;
		using entry_type = typename map_type::value_type;
		using size_type = typename map_type::size_type;
		using difference_type = typename map_type::difference_type;
		using allocator_type = typename map_type::allocator_type;
		using reference = typename map_type::reference;
		using const_reference = typename map_type::const_reference;
		using pointer = typename map_type::pointer;
		using const_pointer = typename map_type::const_pointer;
		using iterator = typename map_type::iterator;
		using const_iterator = typename map_type::const_iterator;
		using reverse_iterator = typename map_type::reverse_iterator;
		using const_reverse_iterator = typename map_type::const_reverse_iterator;
		using node_type = typename map_type::node_type;
		using insert_return_type = typename map_type::insert_return_type;

    private:
		map_type data;

    public:

        NBTCompound() = default;

		~NBTCompound() = default;

		NBTCompound(const NBTCompound&) = default;
		NBTCompound(NBTCompound&&) noexcept = default;

		NBTCompound(std::initializer_list<entry_type>);
		template<class InputIt, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<InputIt>::value_type, entry_type>, int> = 0>
		NBTCompound(InputIt first, InputIt last) : data(first, last) {}
		NBTCompound(const map_type&);
		NBTCompound(map_type&&);

		NBTCompound& operator =(const NBTCompound&) = default;
		NBTCompound& operator =(NBTCompound&&) = default;

		NBTCompound& operator =(std::initializer_list<entry_type>);

        bool operator ==(const NBTCompound& b) const;
		bool operator !=(const NBTCompound& b) const;

		explicit inline operator map_type() const { return data; }

        [[nodiscard]] inline bool empty() const noexcept { return data.empty(); }
		inline size_type size() const noexcept { return data.size(); }
		inline size_type max_size() const noexcept { return data.max_size(); }

        bool hasKey(const nbt_string_view key) const;
		bool hasKey(const nbt_string_view key, const TagType tagType) const;
		template<TagType tagType>
		inline bool hasKey(const nbt_string_view key) const {
			return hasKey(key, tagType);
		}
		template<typename T>
		std::enable_if_t<(is_testable_nbt_type_v<T> || is_nbt_type_v<T>), bool>
		hasKey(const nbt_string_view key) const;

		NBT& operator[](const key_type& key);

		NBT& at(const key_type& key);
		const NBT& at(const key_type& key) const;
		template<TagType tagType>
		nbt_type<tagType>& at(const key_type& key);
		template<TagType tagType>
		const nbt_type<tagType>& at(const key_type& key) const;

		const NBT& get(const key_type& key) const;
		template<TagType tagType>
		const nbt_type<tagType>& get(const key_type& key) const;
		template<typename T, std::enable_if_t<is_convertible_from_nbt_v<T> || is_nbt_type_v<T>, int> = 0>
		T get(const key_type& key) const;

		template<typename T>
		std::enable_if_t<((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>), T>
		getOrDefault(const nbt_string_view key, const T& dflt) const noexcept;
		template<typename T>
		std::enable_if_t<((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>) && std::is_default_constructible_v<T>, T>
		getOrDefault(const nbt_string_view key) const noexcept(std::is_nothrow_default_constructible_v<T>);

		template<typename T>
		std::enable_if_t<(((is_testable_nbt_type_v<T> && is_convertible_from_nbt_v<T>) || is_nbt_type_v<T>) && !eng::is_optional_v<T> && !std::is_same_v<T, std::nullptr_t> && !std::is_same_v<T, std::nullopt_t>), std::optional<T>>
		getOptional(const nbt_string_view key) const noexcept;

		iterator find(const key_type& key);
		const_iterator find(const key_type& key) const;
		template<typename K>
		iterator find(const K& x) { return data.find(x); }
		template<typename K>
		const_iterator find(const K& x) const { return data.find(x); }

		void clear() noexcept;

		iterator erase(iterator pos) noexcept;
		iterator erase(const_iterator first, const_iterator last) noexcept;
		size_type erase(const key_type& key);

		template<typename... Args>
		inline std::pair<iterator, bool> emplace(const key_type& key, Args&&... args) {
			return data.try_emplace(key, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline std::pair<iterator, bool> emplace(key_type&& key, Args&&... args) {
			return data.try_emplace(std::move(key), std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline iterator emplace(const_iterator hint, const key_type& key, Args&&... args) {
			return data.try_emplace(hint, key, std::forward<Args>(args)...);
		}
		template<typename... Args>
		inline iterator emplace(const_iterator hint, key_type&& key, Args&&... args) {
			return data.try_emplace(hint, std::move(key), std::forward<Args>(args)...);
		}

		void swap(NBTCompound& other) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value);

		iterator begin() noexcept;
		const_iterator begin() const noexcept;
		const_iterator cbegin() const noexcept;
		reverse_iterator rbegin() noexcept;
		const_reverse_iterator rbegin() const noexcept;
		const_reverse_iterator crbegin() const noexcept;
		iterator end() noexcept;
		const_iterator end() const noexcept;
		const_iterator cend() const noexcept;
		reverse_iterator rend();
		const_reverse_iterator rend() const noexcept;
		const_reverse_iterator crend() const noexcept;

        std::string toSNBT(const size_t indentAmount = 0, const char indentChar = ' ', const size_t indentLevel = 0) const;

    };

}

#endif

// Some templated member functions of NBTCompound are defined in NBT.h, so it must
// be included incase this header is included in a file that doesn't include NBT.h
#include "NBT.h"