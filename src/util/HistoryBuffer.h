#pragma once

#include <memory>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <cassert>


namespace eng {

    template<typename T, size_t C = size_t(-1)>
    class HistoryBuffer {
        static_assert(C != 0);
    public:
        using value_type = T;

    private:
        std::unique_ptr<T[]> storage;
        size_t beginIndex = 0;
        size_t endIndex = 0;
        size_t currentIndex = 0;

    public:
        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        HistoryBuffer(const T& initialValue) : storage(std::make_unique<T[]>(C)), beginIndex(0), endIndex(1), currentIndex(0) {
            storage[0] = initialValue;
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        HistoryBuffer(T&& initialValue) : storage(std::make_unique<T[]>(C)), beginIndex(0), endIndex(1), currentIndex(0) {
            storage[0] = std::move(initialValue);
        }

        HistoryBuffer(const HistoryBuffer&) = delete;
        HistoryBuffer& operator =(const HistoryBuffer&) = delete;

        HistoryBuffer(HistoryBuffer&& b) noexcept :
                storage(std::move(b.storage)),
                beginIndex(b.beginIndex),
                endIndex(b.endIndex),
                currentIndex(b.currentIndex) {}
        HistoryBuffer& operator =(HistoryBuffer&& b) noexcept {
            if (&b != this) {
                storage = std::move(b.storage);
                beginIndex = b.beginIndex;
                endIndex = b.endIndex;
                currentIndex = b.currentIndex;
            }
            return this;
        }

        ~HistoryBuffer() = default;

        static constexpr size_t capacity() noexcept { return C; }
        inline size_t size() const noexcept {
            return (endIndex < beginIndex) ? ((capacity() - beginIndex) + endIndex) : (endIndex - beginIndex);
        }

        inline T& current() noexcept { return storage[currentIndex]; }
        const T& current() const noexcept { return storage[currentIndex]; }


        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        void reset(const T& value)  noexcept(std::is_nothrow_copy_assignable_v<U>) {
            currentIndex = beginIndex = 0;
            endIndex = 1;
            storage[currentIndex] = value;
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        void reset(T&& value) noexcept(std::is_nothrow_move_assignable_v<U>) {
            currentIndex = beginIndex = 0;
            endIndex = 1;
            storage[currentIndex] = std::move(value);
        }


        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        T& push(const T& value) noexcept(std::is_nothrow_copy_assignable_v<U>) {
            currentIndex = (currentIndex + 1) % capacity();
            if (currentIndex == beginIndex) {
                beginIndex = (beginIndex + 1) % capacity();
            }
            endIndex = currentIndex + 1;
            storage[currentIndex] = value;
            return storage[currentIndex];
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        T& push(T&& value) noexcept(std::is_nothrow_move_assignable_v<U>) {
            currentIndex = (currentIndex + 1) % capacity();
            if (currentIndex == beginIndex) {
                beginIndex = (beginIndex + 1) % capacity();
            }
            endIndex = currentIndex + 1;
            storage[currentIndex] = std::move(value);
            return storage[currentIndex];
        }

        bool undo() noexcept {
            if (currentIndex == beginIndex)
                return false;
            currentIndex = (currentIndex + capacity() - 1) % capacity();
            return true;
        }
        bool redo() noexcept {
            if (((currentIndex + 1) == endIndex) || (((currentIndex + 1) % capacity()) == beginIndex))
                return false;
            currentIndex = (currentIndex + 1) % capacity();
            return true;
        }
    };

    template<typename T>
    class HistoryBuffer<T, 0>;

    template<typename T>
    class HistoryBuffer<T, 1> {
    public:
        using value_type = T;

    private:
        std::unique_ptr<T> storage;

    public:
        template<typename U = T, std::enable_if_t<std::is_copy_constructible_v<U>, int> = 0>
        HistoryBuffer(const T& initialValue) : storage(std::make_unique<T>(initialValue)) {}
        template<typename U = T, std::enable_if_t<std::is_move_constructible_v<U>, int> = 0>
        HistoryBuffer(T&& initialValue) : storage(std::make_unique<T>(std::move(initialValue))) {}

        HistoryBuffer(const HistoryBuffer&) = delete;
        HistoryBuffer& operator =(const HistoryBuffer&) = delete;

        HistoryBuffer(HistoryBuffer&& b) noexcept : storage(std::move(b.storage))  {}
        HistoryBuffer& operator =(HistoryBuffer&& b) noexcept {
            if (&b != this) {
                storage = std::move(b.storage);
            }
            return this;
        }

        ~HistoryBuffer() = default;

        static constexpr size_t capacity() noexcept { return 1; }
        inline size_t size() const noexcept { return 1; }

        inline T& current() noexcept { return *storage; }
        const T& current() const noexcept { return *storage; }

        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        void reset(const T& value)  noexcept(std::is_nothrow_copy_assignable_v<U>) {
            *storage = value;
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        void reset(T&& value) noexcept(std::is_nothrow_move_assignable_v<U>) {
            *storage = std::move(value);
        }

        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        T& push(const T& value) noexcept(std::is_nothrow_copy_assignable_v<U>) {
            *storage = value;
            return *storage;
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        T& push(T&& value) noexcept(std::is_nothrow_move_assignable_v<U>) {
            *storage = std::move(value);
            return *storage;
        }

        bool undo() noexcept { return false; }
        bool redo() noexcept { return false; }
    };

    template<typename T>
    class HistoryBuffer<T, size_t(-1)> {
    public:
        using value_type = T;

    private:
        std::unique_ptr<T[]> storage;
        size_t storageCapacity;
        size_t beginIndex = 0;
        size_t endIndex = 0;
        size_t currentIndex = 0;

    public:
        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        HistoryBuffer(size_t capacity, const T& initialValue) :
                storage(std::make_unique<T[]>(capacity)),
                storageCapacity(capacity),
                beginIndex(0), endIndex(1), currentIndex(0) {
            assert(capacity > 0);
            storage[0] = initialValue;
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        HistoryBuffer(size_t capacity, T&& initialValue) :
                storage(std::make_unique<T[]>(capacity)),
                storageCapacity(capacity),
                beginIndex(0), endIndex(1), currentIndex(0) {
            assert(capacity > 0);
            storage[0] = std::move(initialValue);
        }

        HistoryBuffer(const HistoryBuffer&) = delete;
        HistoryBuffer& operator =(const HistoryBuffer&) = delete;

        HistoryBuffer(HistoryBuffer&& b) noexcept :
                storage(std::move(b.storage)),
                beginIndex(b.beginIndex),
                endIndex(b.endIndex),
                currentIndex(b.currentIndex) {}
        HistoryBuffer& operator =(HistoryBuffer&& b) noexcept {
            if (&b != this) {
                storage = std::move(b.storage);
                beginIndex = b.beginIndex;
                endIndex = b.endIndex;
                currentIndex = b.currentIndex;
            }
            return this;
        }

        ~HistoryBuffer() = default;

        inline size_t capacity() const noexcept { return storageCapacity; }
        inline size_t size() const noexcept {
            return (endIndex < beginIndex) ? ((capacity() - beginIndex) + endIndex) : (endIndex - beginIndex);
        }

        inline T& current() noexcept { return storage[currentIndex]; }
        const T& current() const noexcept { return storage[currentIndex]; }


        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        void reset(const T& value)  noexcept(std::is_nothrow_copy_assignable_v<U>) {
            currentIndex = beginIndex = 0;
            endIndex = 1;
            storage[currentIndex] = value;
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        void reset(T&& value) noexcept(std::is_nothrow_move_assignable_v<U>) {
            currentIndex = beginIndex = 0;
            endIndex = 1;
            storage[currentIndex] = std::move(value);
        }


        template<typename U = T, std::enable_if_t<std::is_copy_assignable_v<U>, int> = 0>
        T& push(const T& value) noexcept(std::is_nothrow_copy_assignable_v<U>) {
            currentIndex = (currentIndex + 1) % capacity();
            if (currentIndex == beginIndex) {
                beginIndex = (beginIndex + 1) % capacity();
            }
            endIndex = currentIndex + 1;
            storage[currentIndex] = value;
            return storage[currentIndex];
        }
        template<typename U = T, std::enable_if_t<std::is_move_assignable_v<U>, int> = 0>
        T& push(T&& value) noexcept(std::is_nothrow_move_assignable_v<U>) {
            currentIndex = (currentIndex + 1) % capacity();
            if (currentIndex == beginIndex) {
                beginIndex = (beginIndex + 1) % capacity();
            }
            endIndex = currentIndex + 1;
            storage[currentIndex] = std::move(value);
            return storage[currentIndex];
        }

        bool undo() noexcept {
            if (currentIndex == beginIndex)
                return false;
            currentIndex = (currentIndex + capacity() - 1) % capacity();
            return true;
        }
        bool redo() noexcept {
            if (((currentIndex + 1) == endIndex) || (((currentIndex + 1) % capacity()) == beginIndex))
                return false;
            currentIndex = (currentIndex + 1) % capacity();
            return true;
        }
    };

}