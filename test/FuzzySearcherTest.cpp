/// @file FuzzySearcherTest.cpp
/// @brief Unit tests for the FuzzySearcher utility functions.

#define BOOST_TEST_MODULE FuzzySearcherTest
#include <boost/test/included/unit_test.hpp>
#include "FuzzySearcher.h"
using namespace fzf;

BOOST_AUTO_TEST_CASE(LevenshteinDistanceTest) {
    // Test identical strings
    BOOST_CHECK_EQUAL(levenshteinDistance("test", "test"), 0);

    // Test completely different strings
    BOOST_CHECK_EQUAL(levenshteinDistance("abc", "xyz"), 3);

    // Test single-character difference
    BOOST_CHECK_EQUAL(levenshteinDistance("cat", "cut"), 1);

    // Test insertion
    BOOST_CHECK_EQUAL(levenshteinDistance("cat", "cats"), 1);

    // Test deletion
    BOOST_CHECK_EQUAL(levenshteinDistance("cats", "cat"), 1);

    // Test substitution
    BOOST_CHECK_EQUAL(levenshteinDistance("kitten", "sitting"), 3);

    // Test empty strings
    BOOST_CHECK_EQUAL(levenshteinDistance("", ""), 0);
    BOOST_CHECK_EQUAL(levenshteinDistance("abc", ""), 3);
    BOOST_CHECK_EQUAL(levenshteinDistance("", "abc"), 3);
}

BOOST_AUTO_TEST_CASE(SmithWatermanTest) {
    // Test identical strings
    BOOST_CHECK_EQUAL(smithWaterman("test", "test"), 8);

    // Test partial match
    BOOST_CHECK_EQUAL(smithWaterman("abc", "xbc"), 4);

    // Test no match
    BOOST_CHECK_EQUAL(smithWaterman("abc", "xyz"), 0);

    // Test substring match
    BOOST_CHECK_EQUAL(smithWaterman("abcdef", "cde"), 6);

    // Test empty strings
    BOOST_CHECK_EQUAL(smithWaterman("", ""), 0);
    BOOST_CHECK_EQUAL(smithWaterman("abc", ""), 0);
    BOOST_CHECK_EQUAL(smithWaterman("", "abc"), 0);
}
