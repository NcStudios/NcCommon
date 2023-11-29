/**
 * @file BinarySerialization.h
 * @copyright Copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#if defined(__APPLE__)
    /** @note While this is likely fine on apple/clang, tests can't build
     * in CI. Disabling consumption on macOS until there is test coverage. */
    #error "BinarySerialization.h is currently unsupported on macOS."
#endif

#include "ncutility/NcError.h"
#include "ncutility/Type.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

namespace nc::serialize
{
class BinaryBuffer;

/**
 * @brief Serialize an object to a BinaryBuffer.
 * @note Additional overloads of Serialize() may be provided. The following are supported out-of-the-box:
 *   - Trivially copyable types
 *   - Stl types: array, vector, string, pair, and unordered_map
 *   - Aggregates with <= g_maxMemberCount members, each satisfying at least one of these requirements
 */
template<class T>
void Serialize(BinaryBuffer& buffer, const T& in);

/**
 * @brief Deserialize an object from a BinaryBuffer.
 * @note Additional overloads of Deserialize() may be provided. The following are supported out-of-the-box:
 *   - Trivially copyable types
 *   - Stl types: array, vector, string, pair, and unordered_map
 *   - Aggregates with <= g_maxMemberCount members, each satisfying at least one of these requirements
 */
template<class T>
void Deserialize(BinaryBuffer& buffer, T& out);

/** @brief The maximum number of members an aggregate may have for default serialization. */
inline constexpr auto g_maxMemberCount = 10ull;

/** @brief A simple interface for reading and writing binary data. */
class BinaryBuffer
{
    public:
        /** @brief Construct a new BinaryBuffer. */
        explicit BinaryBuffer(size_t sizeHint = 1024)
        {
            m_data.reserve(sizeHint);
        }

        /** @brief Construct a BinaryBuffer from a vector of bytes. */
        explicit BinaryBuffer(std::vector<uint8_t> data)
            : m_data{std::move(data)}, m_writePos{m_data.size()}
        {
        }

        /** @brief Read bytes from the buffer. */
        void Read(void* out, size_t nBytes)
        {
            AssertRead(nBytes);
            std::memcpy(out, m_data.data() + m_readPos, nBytes);
            m_readPos += nBytes;
        }

        /** @brief Read a trivially copyable object from the buffer. */
        template<type::TriviallyCopyable T>
        void Read(T& out)
        {
            Read(static_cast<void*>(&out), sizeof(T));
        }

        /** @brief Write bytes to the buffer. */
        void Write(const void* in, size_t nBytes)
        {
            EnsureCapacity(nBytes);
            std::memcpy(m_data.data() + m_writePos, in, nBytes);
            m_writePos += nBytes;
        }

        /** @brief Write a trivially copyable object to the buffer. */
        template<type::TriviallyCopyable T>
        void Write(const T& in)
        {
            Write(static_cast<const void*>(&in), sizeof(T));
        }

        /** @brief Get the number of bytes left to read from the buffer. */
        auto AvailableReadBytes() const noexcept -> size_t
        {
            return m_writePos - m_readPos;
        }

        /** @brief Get the number of bytes available for writing without requiring a buffer resize. */
        auto AvailableWriteCapacity() const noexcept -> size_t
        {
            return m_data.capacity() - m_writePos;
        }

        /** @brief Change the capacity of the buffer.
         *  @note Read and write positions will be updated if nBytes is less than Size(). */
        void Resize(size_t nBytes)
        {
            m_data.resize(nBytes, 0);
            m_readPos = std::min(m_readPos, nBytes);
            m_writePos = std::min(m_writePos, nBytes);
        }

        /** @brief Clear the buffer and reset the read and write positions. */
        void Clear() noexcept
        {
            m_data.clear();
            m_readPos = 0;
            m_writePos = 0;
        }

        /** @brief Release ownership of the underlying binary data. */
        auto ReleaseBuffer() -> std::vector<uint8_t>
        {
            auto buffer = std::move(m_data);
            Clear();
            return buffer;
        }

    private:
        std::vector<uint8_t> m_data;
        size_t m_readPos = 0;
        size_t m_writePos = 0;

        void AssertRead(size_t bytes)
        {
            if (m_readPos + bytes > m_writePos)
            {
                throw NcError{"Out of bounds read."};
            }
        }

        void EnsureCapacity(size_t nBytes)
        {
            if (m_writePos + nBytes >= m_data.size())
            {
                Resize(m_data.size() + nBytes);
            }
        }
};

/** @cond internal */
namespace detail
{
template<class... Args>
void SerializeMultiple(BinaryBuffer& buffer, Args&&... args)
{
    (Serialize(buffer, args), ...);
}

template<class... Args>
void DeserializeMultiple(BinaryBuffer& buffer, Args&&... args)
{
    (Deserialize(buffer, args), ...);
}

template<class C>
void SerializeTrivialContainer(BinaryBuffer& buffer, const C& container)
{
    buffer.Write(container.size());
    buffer.Write(container.data(), sizeof(typename C::value_type) * container.size());
}

template<class C>
void DeserializeTrivialContainer(BinaryBuffer& buffer, C& container)
{
    auto size = size_t{};
    buffer.Read(size);
    container.resize(size);
    buffer.Read(container.data(), sizeof(typename C::value_type) * size);
}

template<class C>
void SerializeNonTrivialContainer(BinaryBuffer& buffer, const C& container)
{
    buffer.Write(container.size());
    for (const auto& obj : container) Serialize(buffer, obj);
}

template<class C>
void DeserializeNonTrivialContainer(BinaryBuffer& buffer, C& container)
{
    auto count = size_t{};
    buffer.Read(count);
    container.reserve(count);
    std::generate_n(std::back_inserter(container), count, [&buffer]()
    {
        auto out = typename C::value_type{};
        Deserialize(buffer, out);
        return out;
    });
}
} // namespace detail

template<type::TriviallyCopyable T>
void Serialize(BinaryBuffer& buffer, const T& in)
{
    buffer.Write(in);
}

template<type::TriviallyCopyable T>
void Deserialize(BinaryBuffer& buffer, T& out)
{
    buffer.Read(out);
}

template<>
inline void Serialize(BinaryBuffer& buffer, const std::string& in)
{
    detail::SerializeTrivialContainer(buffer, in);
}

template<>
inline void Deserialize(BinaryBuffer& buffer, std::string& out)
{
    detail::DeserializeTrivialContainer(buffer, out);
}

template<type::TriviallyCopyable T>
void Serialize(BinaryBuffer& buffer, const std::vector<T>& in)
{
    detail::SerializeTrivialContainer(buffer, in);
}

template<type::TriviallyCopyable T>
void Deserialize(BinaryBuffer& buffer, std::vector<T>& out)
{
    detail::DeserializeTrivialContainer(buffer, out);
}

template<class T>
void Serialize(BinaryBuffer& buffer, const std::vector<T>& in)
{
    detail::SerializeNonTrivialContainer(buffer, in);
}

template<class T>
void Deserialize(BinaryBuffer& buffer, std::vector<T>& out)
{
    detail::DeserializeNonTrivialContainer(buffer, out);
}

template<type::TriviallyCopyable T, size_t I>
void Serialize(BinaryBuffer& buffer, const std::array<T, I>& in)
{
    detail::SerializeTrivialContainer(buffer, in);
}

template<type::TriviallyCopyable T, size_t I>
void Deserialize(BinaryBuffer& buffer, std::array<T, I>& out)
{
    auto size = size_t{};
    buffer.Read(size);
    buffer.Read(out.data(), sizeof(T) * out.size());
}

template<class T, size_t I>
void Serialize(BinaryBuffer& buffer, const std::array<T, I>& in)
{
    detail::SerializeNonTrivialContainer(buffer, in);
}

template<class T, size_t I>
void Deserialize(BinaryBuffer& buffer, std::array<T, I>& out)
{
    auto count = size_t{}; // throwaway value
    Deserialize(buffer, count);
    for (auto& obj : out) Deserialize(buffer, obj);
}

template<class T, class U>
void Serialize(BinaryBuffer& buffer, const std::pair<T, U>& in)
{
    detail::SerializeMultiple(buffer, in.first, in.second);
}

template<class T, class U>
void Deserialize(BinaryBuffer& buffer, std::pair<T, U>& out)
{
    detail::DeserializeMultiple(buffer, out.first, out.second);
}

template<class K, class V>
void Serialize(BinaryBuffer& buffer, const std::unordered_map<K, V>& in)
{
    detail::SerializeNonTrivialContainer(buffer, in);
}

template<class K, class V>
void Deserialize(BinaryBuffer& buffer, std::unordered_map<K, V>& out)
{
    auto count = size_t{};
    buffer.Read(count);
    out.reserve(count);
    std::generate_n(std::inserter(out, std::end(out)), count, [&buffer]()
    {
        auto pair = std::pair<K, V>{};
        Deserialize(buffer, pair);
        return pair;
    });
}

template<type::NonTrivialAggregate T>
void Serialize(BinaryBuffer& buffer, const T& in)
{
    constexpr auto memberCount = type::MemberCount<T>();
    static_assert(memberCount <= g_maxMemberCount);

    if constexpr (memberCount == 10)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
    }
    else if constexpr (memberCount == 9)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9);
    }
    else if constexpr (memberCount == 8)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7, m8] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8);
    }
    else if constexpr (memberCount == 7)
    {
        const auto& [m1, m2, m3, m4, m5, m6, m7] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7);
    }
    else if constexpr (memberCount == 6)
    {
        const auto& [m1, m2, m3, m4, m5, m6] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4, m5, m6);
    }
    else if constexpr (memberCount == 5)
    {
        const auto& [m1, m2, m3, m4, m5] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4, m5);
    }
    else if constexpr (memberCount == 4)
    {
        const auto& [m1, m2, m3, m4] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3, m4);
    }
    else if constexpr (memberCount == 3)
    {
        const auto& [m1, m2, m3] = in;
        detail::SerializeMultiple(buffer, m1, m2, m3);
    }
    else if constexpr (memberCount == 2)
    {
        const auto& [m1, m2] = in;
        detail::SerializeMultiple(buffer, m1, m2);
    }
    else if constexpr (memberCount == 1)
    {
        const auto& [m1] = in;
        detail::SerializeMultiple(buffer, m1);
    }
}

template<type::NonTrivialAggregate T>
void Deserialize(BinaryBuffer& buffer, T& out)
{
    constexpr auto memberCount = type::MemberCount<T>();
    static_assert(memberCount <= g_maxMemberCount);

    if constexpr (memberCount == 10)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9, m10] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10);
    }
    else if constexpr (memberCount == 9)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8, m9] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8, m9);
    }
    else if constexpr (memberCount == 8)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7, m8] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7, m8);
    }
    else if constexpr (memberCount == 7)
    {
        auto& [m1, m2, m3, m4, m5, m6, m7] = out;
       detail:: DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6, m7);
    }
    else if constexpr (memberCount == 6)
    {
        auto& [m1, m2, m3, m4, m5, m6] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3, m4, m5, m6);
    }
    else if constexpr (memberCount == 5)
    {
        auto& [m1, m2, m3, m4, m5] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3, m4, m5);
    }
    else if constexpr (memberCount == 4)
    {
        auto& [m1, m2, m3, m4] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3, m4);
    }
    else if constexpr (memberCount == 3)
    {
        auto& [m1, m2, m3] = out;
        detail::DeserializeMultiple(buffer, m1, m2, m3);
    }
    else if constexpr (memberCount == 2)
    {
        auto& [m1, m2] = out;
        detail::DeserializeMultiple(buffer, m1, m2);
    }
    else if constexpr (memberCount == 1)
    {
        auto& [m1] = out;
        detail::DeserializeMultiple(buffer, m1);
    }
}
/** @endcond internal */
} // namespace nc::serialize
