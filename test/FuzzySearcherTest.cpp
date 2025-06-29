// @file FuzzySearcherTest.cpp
// @brief Unit tests for the FuzzySearcher utility functions using Google Test.

#include <gtest/gtest.h>
#include "FuzzySearcher.h"
using namespace fzf;

TEST(FuzzySearcherTest, LevenshteinDistance) {
    // Test identical strings
    EXPECT_EQ(levenshteinDistance("test", "test"), 0);

    // Test completely different strings
    EXPECT_EQ(levenshteinDistance("abc", "xyz"), 3);

    // Test single-character difference
    EXPECT_EQ(levenshteinDistance("cat", "cut"), 1);

    // Test insertion
    EXPECT_EQ(levenshteinDistance("cat", "cats"), 1);

    // Test deletion
    EXPECT_EQ(levenshteinDistance("cats", "cat"), 1);

    // Test substitution
    EXPECT_EQ(levenshteinDistance("kitten", "sitting"), 3);

    // Test empty strings
    EXPECT_EQ(levenshteinDistance("", ""), 0);
    EXPECT_EQ(levenshteinDistance("abc", ""), 3);
    EXPECT_EQ(levenshteinDistance("", "abc"), 3);
}

TEST(FuzzySearcherTest, SmithWaterman) {
    // Test identical strings
    EXPECT_EQ(smithWaterman("test", "test"), 8);

    // Test partial match
    EXPECT_EQ(smithWaterman("abc", "xbc"), 4);

    // Test no match
    EXPECT_EQ(smithWaterman("abc", "xyz"), 0);

    // Test substring match
    EXPECT_EQ(smithWaterman("abcdef", "cde"), 6);

    // Test empty strings
    EXPECT_EQ(smithWaterman("", ""), 0);
    EXPECT_EQ(smithWaterman("abc", ""), 0);
    EXPECT_EQ(smithWaterman("", "abc"), 0);
}
