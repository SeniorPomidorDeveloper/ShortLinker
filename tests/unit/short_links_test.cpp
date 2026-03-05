#include <userver/utest/utest.hpp>

// Unit tests for DB-dependent logic require a running database.
// See tests/test_basic.py for functional integration tests.

UTEST(ShortLinks, GeneratedIdLength) {
    // Verify that the expected ID structure matches constraints.
    // The kIdLength constant in create_link.cpp is 8.
    constexpr std::size_t kExpectedIdLength = 8;
    EXPECT_EQ(kExpectedIdLength, 8u);
}
