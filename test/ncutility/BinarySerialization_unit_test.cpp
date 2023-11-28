#include "gtest/gtest.h"
#include "ncutility/BinarySerialization.h"

#include <algorithm>

namespace test
{
// Type expected to be automatically serializable via aggregate unpacking
struct Aggregate
{
    struct SingleMember
    {
        int m1;
        auto operator<=>(const SingleMember&) const = default;
    };

    struct NonTrivialMember
    {
        SingleMember m1;
        std::string m2;
        auto operator<=>(const NonTrivialMember&) const = default;
    };

    struct CollectionMember
    {
        std::vector<SingleMember> m1;
        auto operator<=>(const CollectionMember&) const = default;
    };

    SingleMember m1;
    NonTrivialMember m2;
    CollectionMember m3;
    int m4, m5, m6, m7, m8, m9, m10; // give it max members
    auto operator<=>(const Aggregate&) const = default;
};

static_assert(nc::type::Aggregate<Aggregate>);
static_assert(!nc::type::TriviallyCopyable<Aggregate>);
static_assert(nc::type::MemberCount<Aggregate>() <= nc::serialize::g_maxMemberCount);

// Type with too many members to be unpacked - requires user-provided an overload for serialization.
struct BigAggregate
{
    int m1, m2, m3, m4, m5, m6, m7, m8, m9, m10;
    std::string m11;

    auto operator<=>(const BigAggregate&) const = default;
};

static_assert(nc::type::Aggregate<BigAggregate>);
static_assert(!nc::type::TriviallyCopyable<BigAggregate>);
static_assert(nc::type::MemberCount<BigAggregate>() > nc::serialize::g_maxMemberCount);

// Type which can't be memcpy'd or unpacked - requies user-provided overload for serialization.
class NonAggregate
{
    public:
        NonAggregate(std::string x_, int y_)
            : x{x_}, y{y_} {}

        auto X() -> std::string&{ return x; }
        auto X() const -> const std::string& { return x;}
        auto Y() -> int& { return y; }
        auto Y() const -> const int& { return y; }

    private:
        std::string x;
        int y;
};

static_assert(!nc::type::Aggregate<NonAggregate>);
static_assert(!nc::type::TriviallyCopyable<NonAggregate>);

// BigAggregate overloads are in 'test' namesapce for name lookup sanity checking
void Serialize(nc::serialize::BinaryBuffer& buffer, const BigAggregate& in)
{
    Serialize(buffer, in.m1);
    Serialize(buffer, in.m2);
    Serialize(buffer, in.m3);
    Serialize(buffer, in.m4);
    Serialize(buffer, in.m5);
    Serialize(buffer, in.m6);
    Serialize(buffer, in.m7);
    Serialize(buffer, in.m8);
    Serialize(buffer, in.m9);
    Serialize(buffer, in.m10);
    Serialize(buffer, in.m11);
}

void Deserialize(nc::serialize::BinaryBuffer& buffer, BigAggregate& in)
{
    Deserialize(buffer, in.m1);
    Deserialize(buffer, in.m2);
    Deserialize(buffer, in.m3);
    Deserialize(buffer, in.m4);
    Deserialize(buffer, in.m5);
    Deserialize(buffer, in.m6);
    Deserialize(buffer, in.m7);
    Deserialize(buffer, in.m8);
    Deserialize(buffer, in.m9);
    Deserialize(buffer, in.m10);
    Deserialize(buffer, in.m11);
}
} // namespace test

namespace nc::serialize
{
// NonAggregate has overloads in 'nc::serialize' for name lookup sanity checking
void Serialize(nc::serialize::BinaryBuffer& buffer, const test::NonAggregate& in)
{
    Serialize(buffer, in.X());
    Serialize(buffer, in.Y());
}

void Deserialize(nc::serialize::BinaryBuffer& buffer, test::NonAggregate& in)
{
    Deserialize(buffer, in.X());
    Deserialize(buffer, in.Y());
}
} // namespace nc::serialize

TEST(BinarySerializationTest, BinaryBuffer_readWrite_preservesRoundTrip)
{
    const auto expected = std::array<int, 3>{10, 11, 12};
    auto uut = nc::serialize::BinaryBuffer{};
    uut.Write(&expected[0], 4);
    uut.Write(&expected[1], 4);
    uut.Write(&expected[2], 4);

    ASSERT_EQ(12, uut.AvailableReadBytes());
    auto actual = std::array<int, 3>{0, 0, 0};
    uut.Read(&actual[0], 4);
    uut.Read(&actual[1], 4);
    uut.Read(&actual[2], 4);
    EXPECT_EQ(0, uut.AvailableReadBytes());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(BinarySerializationTest, BinaryBuffer_readWrite_interleavedCallsSucceed)
{
    const auto expected = std::array<int, 3>{10, 11, 12};
    auto actual = std::array<int, 3>{0, 0, 0};
    auto uut = nc::serialize::BinaryBuffer{};
    uut.Write(&expected[0], 4);
    uut.Read(&actual[0], 4);
    uut.Write(&expected[1], 4);
    uut.Read(&actual[1], 4);
    uut.Write(&expected[2], 4);
    uut.Read(&actual[2], 4);
    EXPECT_EQ(0, uut.AvailableReadBytes());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(BinarySerializationTest, BinaryBuffer_resize_growthPreservesPositions)
{
    const auto expected = std::array<int, 3>{10, 11, 12};
    auto uut = nc::serialize::BinaryBuffer{};
    uut.Write(&expected[0], 4);
    uut.Resize(128ull);
    EXPECT_EQ(4, uut.AvailableReadBytes());
    EXPECT_LE(124ull, uut.AvailableWriteCapacity());
    uut.Write(&expected[1], 4);
    uut.Write(&expected[2], 4);

    ASSERT_EQ(12, uut.AvailableReadBytes());
    auto actual = std::array<int, 3>{0, 0, 0};
    uut.Read(&actual[0], 4);
    uut.Read(&actual[1], 4);
    uut.Read(&actual[2], 4);
    EXPECT_EQ(0, uut.AvailableReadBytes());
    EXPECT_TRUE(std::ranges::equal(expected, actual));
}

TEST(BinarySerializationTest, BinaryBuffer_resize_shrinkageMovesPositions)
{
    const auto expected = std::array<int, 3>{10, 11, 12};
    auto uut = nc::serialize::BinaryBuffer{};
    uut.Write(&expected[0], 4);
    uut.Write(&expected[1], 4);
    uut.Resize(4ull); // discard last value
    uut.Write(&expected[2], 4);

    ASSERT_EQ(8, uut.AvailableReadBytes());
    auto actual = std::array<int, 2>{0, 0};
    uut.Read(&actual[0], 4);
    uut.Read(&actual[1], 4);
    EXPECT_EQ(0, uut.AvailableReadBytes());
    EXPECT_EQ(expected[0], actual[0]);
    EXPECT_EQ(expected[2], actual[1]);
}

TEST(BinarySerializationTest, BinaryBuffer_clear_resetsState)
{
    auto uut = nc::serialize::BinaryBuffer{};
    auto dummy = 1;
    uut.Write(&dummy, 4);
    uut.Clear();
    EXPECT_EQ(0, uut.AvailableReadBytes());
    EXPECT_TRUE(uut.ReleaseBuffer().empty());
}

TEST(BinarySerializationTest, BinaryBuffer_releaseBuffer_resetsState)
{
    auto uut = nc::serialize::BinaryBuffer{};
    auto dummy = 1;
    uut.Write(&dummy, 4);
    const auto buffer = uut.ReleaseBuffer();
    EXPECT_EQ(4, buffer.size());
    EXPECT_EQ(0, uut.AvailableReadBytes());
    EXPECT_TRUE(uut.ReleaseBuffer().empty());
}

TEST(BinarySerializationTest, Serialize_primitives_preservedRoundTrip)
{
    auto buffer = nc::serialize::BinaryBuffer{};

    const auto expectedInt = 42;
    const auto expectedFloat = 3.14f;
    Serialize(buffer, expectedInt);
    Serialize(buffer, expectedFloat);

    auto actualInt = 0;
    auto actualFloat = 0.0f;
    Deserialize(buffer, actualInt);
    Deserialize(buffer, actualFloat);

    EXPECT_EQ(expectedInt, actualInt);
    EXPECT_EQ(expectedFloat, actualFloat);
}

TEST(BinarySerializationTest, Serialize_containers_preservedRoundTrip)
{
    auto buffer = nc::serialize::BinaryBuffer{};

    const auto expectedString = std::string{"a test string"};
    const auto expectedTrivialVector = std::vector<int>{1, 2, 3};
    const auto expectedNonTrivialVector = std::vector<std::string>{"one", "two", "three"};
    const auto expectedTrivialArray = std::array<size_t, 2>{5, 9};
    const auto expectedNonTrivialArray = std::array<test::Aggregate::NonTrivialMember, 1>{{50, "test"}};
    const auto expectedMap = std::unordered_map<std::string, size_t>{{std::string{"test"}, 32}};
    Serialize(buffer, expectedString);
    Serialize(buffer, expectedTrivialVector);
    Serialize(buffer, expectedNonTrivialVector);
    Serialize(buffer, expectedTrivialArray);
    Serialize(buffer, expectedNonTrivialArray);
    Serialize(buffer, expectedMap);

    auto actualString = std::string{};
    auto actualTrivialVector = std::vector<int>{};
    auto actualNonTrivialVector = std::vector<std::string>{};
    auto actualTrivialArray = std::array<size_t, 2>{};
    auto actualNonTrivialArray = std::array<test::Aggregate::NonTrivialMember, 1>{};
    auto actualMap = std::unordered_map<std::string, size_t>{};
    Deserialize(buffer, actualString);
    Deserialize(buffer, actualTrivialVector);
    Deserialize(buffer, actualNonTrivialVector);
    Deserialize(buffer, actualTrivialArray);
    Deserialize(buffer, actualNonTrivialArray);
    Deserialize(buffer, actualMap);

    EXPECT_EQ(expectedString, actualString);
    EXPECT_TRUE(std::ranges::equal(expectedTrivialVector, actualTrivialVector));
    EXPECT_TRUE(std::ranges::equal(expectedNonTrivialVector, actualNonTrivialVector));
    EXPECT_TRUE(std::ranges::equal(expectedTrivialArray, actualTrivialArray));
    EXPECT_TRUE(std::ranges::equal(expectedNonTrivialArray, actualNonTrivialArray));
    EXPECT_TRUE(std::ranges::equal(expectedMap, actualMap));
}

TEST(BinarySerializationTest, Serialize_emptyContainers_triviallySucceed)
{
    auto buffer = nc::serialize::BinaryBuffer{};

    const auto expectedString = std::string{};
    const auto expectedTrivialVector = std::vector<int>{};
    const auto expectedNonTrivialVector = std::vector<std::string>{};
    const auto expectedMap = std::unordered_map<std::string, size_t>{};
    Serialize(buffer, expectedString);
    Serialize(buffer, expectedTrivialVector);
    Serialize(buffer, expectedNonTrivialVector);
    Serialize(buffer, expectedMap);

    auto actualString = std::string{};
    auto actualTrivialVector = std::vector<int>{};
    auto actualNonTrivialVector = std::vector<std::string>{};
    auto actualTrivialArray = std::array<size_t, 2>{};
    auto actualNonTrivialArray = std::array<test::Aggregate::NonTrivialMember, 1>{};
    auto actualMap = std::unordered_map<std::string, size_t>{};
    Deserialize(buffer, actualString);
    Deserialize(buffer, actualTrivialVector);
    Deserialize(buffer, actualNonTrivialVector);
    Deserialize(buffer, actualMap);

    EXPECT_EQ(expectedString, actualString);
    EXPECT_TRUE(std::ranges::equal(expectedTrivialVector, actualTrivialVector));
    EXPECT_TRUE(std::ranges::equal(expectedNonTrivialVector, actualNonTrivialVector));
    EXPECT_TRUE(std::ranges::equal(expectedMap, actualMap));
}

TEST(BinarySerializationTest, Serialize_aggregate_preservedRoundTrip)
{
    auto buffer = nc::serialize::BinaryBuffer{};
    auto actual = test::Aggregate{};
    const auto expected = test::Aggregate
    {
        42,
        {59, "sample"},
        { {{1}, {2}, {3}} },
        4, 5, 6, 7, 8, 9, 10
    };

    Serialize(buffer, expected);
    Deserialize(buffer, actual);
    EXPECT_EQ(expected, actual);
}

TEST(BinarySerializationTest, Serialize_aggregateWithCustomOverloads_preservedRoundTrip)
{
    auto buffer = nc::serialize::BinaryBuffer{};
    const auto expected = test::BigAggregate{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "test"};
    auto actual = test::BigAggregate{};
    Serialize(buffer, expected);
    Deserialize(buffer, actual);
    EXPECT_EQ(expected, actual);
}

TEST(BinarySerializationTest, Serialize_nonAggregate_preservedRoundTrip)
{
    auto buffer = nc::serialize::BinaryBuffer{};
    auto expected = test::NonAggregate{"1", 2};
    auto actual = test::NonAggregate{"", 0};
    Serialize(buffer, expected);
    Deserialize(buffer, actual);
    EXPECT_EQ(expected.X(), actual.X());
    EXPECT_EQ(expected.Y(), actual.Y());
}
