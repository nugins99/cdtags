/// @file FuzzySearcher.cpp
/// @brief Implementation of utility functions for fuzzy searching, including
/// Levenshtein distance calculation.

#include "FuzzySearcher.h"

#include <vector>

/// @namespace fzf
/// @brief Namespace for fuzzy searching utilities.
namespace fzf
{

int levenshteinDistance(const std::string& s1, const std::string& s2)
{
    size_t len1 = s1.size();
    size_t len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i)
    {
        for (size_t j = 0; j <= len2; ++j)
        {
            if (i == 0)
            {
                dp[i][j] = j;
            }
            else if (j == 0)
            {
                dp[i][j] = i;
            }
            else if (s1[i - 1] == s2[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1];
            }
            else
            {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }

    return dp[len1][len2];
}

/// @brief Calculates the Smith-Waterman similarity score between two strings.
///
/// The Smith-Waterman algorithm is used for local sequence alignment. It finds
/// the optimal alignment between substrings of the input strings.
///
/// @param s1 The first string.
/// @param s2 The second string.
/// @return The Smith-Waterman similarity score.
int smithWaterman(const std::string& s1, const std::string& s2)
{
    size_t len1 = s1.size();
    size_t len2 = s2.size();
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));

    int maxScore = 0;

    for (size_t i = 1; i <= len1; ++i)
    {
        for (size_t j = 1; j <= len2; ++j)
        {
            int match = (s1[i - 1] == s2[j - 1]) ? 2 : -1;
            dp[i][j] = std::max({0, dp[i - 1][j - 1] + match, dp[i - 1][j] - 1, dp[i][j - 1] - 1});
            maxScore = std::max(maxScore, dp[i][j]);
        }
    }

    return maxScore;
}

int scoreSmithWatermanAndLevenshtein(const std::string& search, const std::string& line)
{
    // Calculate the Smith-Waterman score
    int swScore = smithWaterman(search, line);

    // Calculate the Levenshtein distance
    int levDistance = levenshteinDistance(search, line);

    // Combine the scores
    return swScore - levDistance;  // Higher is better
}

int scoreModifiedSmithWaterman(const std::string& search, const std::string& line)
{
    // This function can be used to calculate a score based on the similarity
    // between two strings. For now, we will use the Smith-Waterman algorithm.
    int score = smithWaterman(search, line);

    if (line.find(search) != std::string::npos)
    {
        // If the search string is found in the line, we can boost the score
        score += 10;  // Boost score by 10 for exact matches
    }
    else
    {
        // Look to see if all the characters in the search string are present in the line in the
        // correct order.
        size_t oldPos = -1;
        size_t pos = 0;
        for (char c : search)
        {
            pos = line.find(c, pos);
            if (pos == std::string::npos)
            {
                // If any character is not found, we can reduce the score
                score -= 5;  // Reduce score by 5 for missing characters
                break;
            }
            else if (oldPos +1 == pos)
            {
                // If the character is found adjacent to the previous character, we can boost the score
                score += 1;  // Boost score by 1 for characters found in order
            }
            oldPos = pos;
        }
    }
    return score;
}

int score(const std::string& search, const std::string& line)
{
    // Use the modified Smith-Waterman score for fuzzy searching
    return scoreModifiedSmithWaterman(search, line);
    //return scoreSmithWatermanAndLevenshtein(search, line);
}
}  // namespace fzf