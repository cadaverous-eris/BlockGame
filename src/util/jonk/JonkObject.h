#pragma once

#include <iostream>
#include <optional>

#include "jonk_types.h"
#include "util/type_traits.h"
#include "JonkCompat.h"

namespace jonk {

	class JonkObject;
	std::ostream& operator <<(std::ostream&, const JonkObject&);

	class Jonk;

	class JonkObject {
	public:
		struct KeyCompare {
			using is_transparent = void;
			bool operator ()(const std::string& lhs, const std::string& rhs) const noexcept;
			bool operator ()(const std::string_view lhs, const std::string& rhs) const noexcept;
			bool operator ()(const std::string& lhs, const std::string_view rhs) const noexcept;
			constexpr bool operator ()(const std::string_view lhs, const std::string_view rhs) const noexcept { return lhs < rhs; }
		};
		using map_type = std::map<std::string, Jonk, KeyCompare>;
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

		JonkObject() = default;

		~JonkObject() = default;

		JonkObject(const JonkObject&) = default;
		JonkObject(JonkObject&&) noexcept = default;

		JonkObject(std::initializer_list<entry_type>);
		template<class InputIt, std::enable_if_t<!std::is_same_v<typename std::iterator_traits<InputIt>::value_type, entry_type>, int> = 0>
		JonkObject(InputIt first, InputIt last) : data(first, last) {}
		JonkObject(const map_type&);
		JonkObject(map_type&&);

		JonkObject& operator =(const JonkObject&) = default;
		JonkObject& operator =(JonkObject&&) = default;

		JonkObject& operator =(std::initializer_list<entry_type>);

		// TODO: insert, set (map::insert_or_assign), erase_if, extract, and merge functions

		bool operator ==(const JonkObject& b) const;
		bool operator !=(const JonkObject& b) const;

		explicit operator map_type() const { return data; }

		[[nodiscard]] bool empty() const noexcept;
		size_type size() const noexcept;
		size_type max_size() const noexcept;
		size_type maxSize() const noexcept;

		bool hasKey(const std::string_view key) const;
		bool hasKey(const std::string_view key, const JonkType jonkType) const;
		template<JonkType jonkType>
		inline bool hasKey(const std::string_view key) const {
			return hasKey(key, jonkType);
		}
		template<typename T>
		std::enable_if_t<(is_testable_jonk_type_v<T> || is_jonk_type_v<T>), bool>
		hasKey(const std::string_view key) const;

		Jonk& operator[](const key_type& key);

		Jonk& at(const key_type& key);
		const Jonk& at(const key_type& key) const;
		template<JonkType jonkType>
		jonk_type_ref<jonkType> at(const key_type& key);
		template<JonkType jonkType>
		jonk_type_const_ref<jonkType> at(const key_type& key) const;

		const Jonk& get(const key_type& key) const;
		template<JonkType jonkType>
		jonk_type_const_ref<jonkType> get(const key_type& key) const;
		template<typename T, std::enable_if_t<is_convertible_from_jonk_v<T> || is_jonk_type_v<T>, int> = 0>
		T get(const key_type& key) const;

		template<typename T>
		std::enable_if_t<((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>), T>
		getOrDefault(const std::string_view key, const T& dflt) const noexcept;
		template<typename T>
		std::enable_if_t<((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>) && std::is_default_constructible_v<T>, T>
		getOrDefault(const std::string_view key) const noexcept(std::is_nothrow_default_constructible_v<T>);

		template<typename T>
		std::enable_if_t<(((is_testable_jonk_type_v<T> && is_convertible_from_jonk_v<T>) || is_jonk_type_v<T>) && !eng::is_optional_v<T> && !std::is_same_v<T, nullptr_t> && !std::is_same_v<T, std::nullopt_t>), std::optional<T>>
		getOptional(const std::string_view key) const noexcept;

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

		void swap(JonkObject& other) noexcept(std::allocator_traits<allocator_type>::is_always_equal::value);

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

		std::string toJonkString(const size_t indentAmount = 2, const char indentChar = ' ', const size_t indentLevel = 0) const;

		friend std::ostream& operator <<(std::ostream&, const JonkObject&);

	};

}

// Some templated member functions of JonkObject are defined in Jonk.h, so it must
// be included incase this header is included in a file that doesn't include Jonk.h
#include "Jonk.h"