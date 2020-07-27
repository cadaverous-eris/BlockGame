#pragma once

#include <type_traits>
#include <utility>
#include <algorithm>
#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>
#include <iterator>
#include <stdexcept>
//#include <iostream>


namespace eng {

	// if a Registry can be created for type T
	template<typename T, typename = void>
	struct is_registrable : std::false_type {};
	// if a Registry can be created for type T
	template<typename T>
	struct is_registrable<T, std::void_t<
		typename T::id_t, // T::id_t is a type
		std::enable_if_t<std::is_integral_v<typename T::id_t>>, // T::id_t is an integral type
		decltype(std::declval<const T>().getName()), // T has a const member function getName()
		std::enable_if_t<std::is_convertible_v<decltype(std::declval<const T>().getName()), typename std::string_view>> // T::getName() returns a std::string_view
	>> : std::true_type {};
	// if a Registry can be created for type T
	template<typename T>
	inline constexpr bool is_registrable_v = is_registrable<T>::value;


	template<typename T, typename = void>
	struct RegistryIterator;


	template<typename T, typename = void>
	class Registry;

	template<typename T>
	class Registry<T, typename std::enable_if_t<is_registrable_v<T>>> {
	public:

		// detects T::id_t T::getId() const
		template<typename U = T, typename = void>
		struct is_id_gettable : std::false_type {};
		template<typename U>
		struct is_id_gettable<U, std::void_t<
			decltype(std::declval<const U>().getId()), // T has a const member function getId()
			std::enable_if_t<std::is_same_v<decltype(std::declval<const U>().getId()), typename U::id_t>> // T::getId() returns a T::id_t
		>> : std::true_type {};
		// detects T::setId(T::id_t)
		template<typename U = T, typename = void>
		struct is_id_settable : std::false_type {};
		template<typename U>
		struct is_id_settable<U, std::void_t<
			decltype(std::declval<const U>().setId(std::declval<const typename U::id_t>()))//, // T has a member function setId(T::id_t) const
		>> : std::true_type {};
		static constexpr bool is_id_gettable_v = is_id_gettable<>::value;
		static constexpr bool is_id_settable_v = is_id_settable<>::value;

	private:
		std::unordered_map<std::string_view, typename T::id_t> idMap;
		std::unordered_map<std::string_view, const T*> registryMap;
		std::vector<const T*> registryList;

	public:
		Registry() {}

		Registry(const Registry&) = delete;
		Registry(Registry&&) = delete;
		Registry& operator =(const Registry&) = delete;
		Registry& operator =(Registry&&) = delete;

		typename T::id_t registerObject(const T& object) {
			// error if an object with the same name already exists in this registry
			if (registryMap.find(object.getName()) != registryMap.end()) {
				throw std::runtime_error("Registry already contains an object with name " + std::string(object.getName()));
			}

			registryMap.insert(std::make_pair(object.getName(), &object));
			auto it = registryList.insert(std::end(registryList), &object);
			const typename T::id_t id = it - std::begin(registryList);
			idMap.insert(std::make_pair(object.getName(), id));

			if constexpr (is_id_settable_v) {
				object.setId(id); // call the object's setId(id_t) method if it exists and is accessible
			}

			return id; // return the id of the registered object
		}

		bool isRegistered(std::string_view name) const {
			return registryMap.find(name) != registryMap.end();
		};
		bool isRegistered(const typename T::id_t id) const {
			return id < registryList.size();
		}
		bool isRegistered(const T& value) const {
			return std::find(std::begin(registryList), std::end(registryList), &value) != std::end(registryList);
		}

		typename T::id_t getId(const T& object) const {
			if constexpr (is_id_gettable_v) {
				return object.getId();
			} else {
				return idMap.at(object.getName());
			}
		}

		const T& operator [](const typename T::id_t id) const {
			//return *registryList.at(id);
			return *registryList[id];
		}
		const T& operator [](std::string_view name) const {
			return *registryMap.at(name);
		}

		size_t size() const { return registryList.size(); }

		// removes everything from the registry
		void clear() {
			idMap.clear();
			registryMap.clear();
			registryList.clear();
		}

		RegistryIterator<T> begin() const { return std::cbegin(registryList); }
		RegistryIterator<T> end() const { return std::cend(registryList); }
		RegistryIterator<T> cbegin() const { return std::cbegin(registryList); }
		RegistryIterator<T> cend() const { return std::cend(registryList); }

		// TODO: add rbegin and rend functions

	};


	template<typename T>
	struct RegistryIterator<T, typename std::enable_if_t<is_registrable_v<T>>> {
		// iterator type of std::vector<const T*>
		using pointer_vector_iterator = typename std::vector<const T*>::const_iterator;

		using iterator_category = typename pointer_vector_iterator::iterator_category;
		using value_type = const T; // TODO: change to const T& ???
		using difference_type = typename pointer_vector_iterator::difference_type;
		using reference = const T&;
		using pointer = const T*;

		RegistryIterator() : it() {}
		RegistryIterator(const pointer_vector_iterator& it) : it(it) {}

		reference operator *() const { return **it; }

		pointer operator ->() const { return *it; }

		RegistryIterator& operator ++() {
			++it;
			return *this;
		}
		RegistryIterator operator ++(int) {
			RegistryIterator c = *this;
			++it;
			return c;
		}
		RegistryIterator& operator --() {
			--it;
			return *this;
		}
		RegistryIterator operator --(int) {
			RegistryIterator c = *this;
			--it;
			return c;
		}

		RegistryIterator& operator +=(const difference_type n) {
			it += n;
			return *this;
		}
		RegistryIterator operator +(const difference_type n) const { return it + n; }
		RegistryIterator& operator -=(const difference_type n) {
			it -= n;
			return *this;
		}
		RegistryIterator operator -(const difference_type n) const { return it - n; }

		difference_type operator -(const RegistryIterator<T>& b) const { return it - b.it; }

		reference operator [](const difference_type& n) const {
			return *it[n];
		}

		bool operator ==(const RegistryIterator<T>& b) const { return it == b.it; }
		bool operator !=(const RegistryIterator<T>& b) const { return it != b.it; }

		bool operator <(const RegistryIterator<T>& b) const { return it < b.it; }
		bool operator <=(const RegistryIterator<T>& b) const { return it <= b.it; }
		bool operator >(const RegistryIterator<T>& b) const { return it > b.it; }
		bool operator >=(const RegistryIterator<T>& b) const { return it >= b.it; }

		void swap(RegistryIterator<T>& b) {
			using std::swap;
			swap(it, b.it);
		}

	protected:
		pointer_vector_iterator it;
	};

	template<typename T>
	RegistryIterator<T> operator +(const typename RegistryIterator<T>::difference_type n, const RegistryIterator<T>& b) { return b + n; }

	template<typename T>
	void swap(RegistryIterator<T>& a, RegistryIterator<T>& b) { a.swap(b); }

}