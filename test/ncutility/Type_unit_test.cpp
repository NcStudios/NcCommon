#include "gtest/gtest.h"
#include "ncutility/Type.h"

#include <string>

struct EmptyType{};
struct TrivialType { int m1; float m2; };
struct NonTrivialType { ~NonTrivialType(){} };
struct NonAggregateType { NonAggregateType() {} };

TEST(TypeTests, Concept_TriviallyCopyable)
{
    static_assert(nc::type::TriviallyCopyable<int>);
    static_assert(nc::type::TriviallyCopyable<EmptyType>);
    static_assert(nc::type::TriviallyCopyable<TrivialType>);
    static_assert(!nc::type::TriviallyCopyable<NonTrivialType>);
    static_assert(nc::type::TriviallyCopyable<NonAggregateType>);
}

TEST(TypeTests, Concept_Aggregate)
{
    static_assert(nc::type::Aggregate<char[1]>);
    static_assert(nc::type::Aggregate<EmptyType>);
    static_assert(nc::type::Aggregate<TrivialType>);
    static_assert(nc::type::Aggregate<NonTrivialType>);
    static_assert(!nc::type::Aggregate<NonAggregateType>);
}

TEST(TypeTests, Concept_NonTrivialAggregate)
{
    static_assert(!nc::type::NonTrivialAggregate<EmptyType>);
    static_assert(!nc::type::NonTrivialAggregate<TrivialType>);
    static_assert(nc::type::NonTrivialAggregate<NonTrivialType>);
    static_assert(!nc::type::NonTrivialAggregate<NonAggregateType>);
}

TEST(TypeTests, MemberCount)
{
    static_assert(nc::type::MemberCount<EmptyType>() == 0ull);
    static_assert(nc::type::MemberCount<TrivialType>() == 2ull);
    static_assert(nc::type::MemberCount<NonTrivialType>() == 0ull);
}
