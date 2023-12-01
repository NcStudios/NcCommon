/**
 * @file Type.h
 * @copyright Copyright Jaremie Romer and McCallister Romer 2023
 */
#pragma once

#include <type_traits>

namespace nc::type
{
/** @brief Specifies that a type is trivially copyable. */
template<class T>
concept TriviallyCopyable = requires
{
    requires std::is_trivially_copyable_v<T>;
};

/** @brief Specifies a type is an aggregate. */
template<class T>
concept Aggregate = requires
{
    requires std::is_aggregate_v<T>;
};

/** @brief Specifies an aggregate type is constructible with the given list of arguments. */
template<class Aggregate, class... Args>
concept ConstructibleWith = requires
{
    { Aggregate{ {Args{}}... } };
};

/** @brief A type which is implicitly convertable to all other types. */
struct UniversalType
{
    template<class T>
    operator T() {}
};

/** @brief Count the number of members in an aggregate. */
template<Aggregate T, class... Members>
consteval auto MemberCount()
{
    if constexpr (!ConstructibleWith<T, Members..., UniversalType>)
    {
        return sizeof...(Members);
    }
    else
    {
        return MemberCount<T, Members..., UniversalType>();
    }
}
} // namespace nc::type
