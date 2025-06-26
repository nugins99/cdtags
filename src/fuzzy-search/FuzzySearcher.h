/// @file FuzzySearcher.h
/// @brief Header file for fuzzy searching utilities, including Levenshtein
/// distance and Smith-Waterman similarity score calculation.

#ifndef FUZZYSEARCHER_H
#define FUZZYSEARCHER_H

#include <string>

/// @namespace fzf
/// @brief Namespace for fuzzy searching utilities.
namespace fzf
{

/// @brief Calculates the Levenshtein distance between two strings.
///
/// The Levenshtein distance is a measure of the difference between two strings.
/// It is calculated as the minimum number of single-character edits (insertions,
/// deletions, or substitutions) required to change one string into the other.
///
/// @param s1 The first string.
/// @param s2 The second string.
/// @return The Levenshtein distance.
int levenshteinDistance(const std::string& s1, const std::string& s2);

/// @brief Calculates the Smith-Waterman similarity score between two strings.
///
/// The Smith-Waterman algorithm is used for local sequence alignment. It finds
/// the optimal alignment between substrings of the input strings.
///
/// @param s1 The first string.
/// @param s2 The second string.
/// @return The Smith-Waterman similarity score.
int smithWaterman(const std::string& s1, const std::string& s2);

int score(const std::string& s1, const std::string& s2);

}  // namespace fzf

#endif  // FUZZYSEARCHER_H