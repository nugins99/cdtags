#pragma once
#include <boost/property_tree/ptree.hpp>
#include <cstdint>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

namespace fzf
{

struct Result
{
    Result(std::size_t idx, const std::string& ln, bool sel, double sc)
        : index(idx), line(ln), selected(sel), score(sc)
    {}
    std::size_t index{0};
    std::string line;
    bool selected{false};
    double score{0.0};
    boost::property_tree::ptree toPropertyTree() const
    {
        boost::property_tree::ptree pt;
        pt.put("index", static_cast<long long>(index));
        pt.put("line", line);
        pt.put("selected", selected);
        pt.put("score", score);
        return pt;
    }
};

struct Results
{
    std::string searchString;
    std::vector<Result> results;
    std::size_t totalResults{0};
    std::pair<std::size_t, std::size_t> resultRange;

    boost::property_tree::ptree toPropertyTree(bool reverse) const
    {
        boost::property_tree::ptree pt;
        pt.put("searchString", searchString);
        pt.put("totalResults", static_cast<long long>(totalResults));

        // range as an array [first, second]
        boost::property_tree::ptree rangeArray;
        {
            boost::property_tree::ptree item;
            item.put("", static_cast<long long>(resultRange.first));
            rangeArray.push_back(std::make_pair("", item));
        }
        {
            boost::property_tree::ptree item;
            item.put("", static_cast<long long>(resultRange.second));
            rangeArray.push_back(std::make_pair("", item));
        }
        pt.add_child("range", rangeArray);

        // results array
        boost::property_tree::ptree resultsArray;

        if (reverse)
        {
            for (const auto& r : std::ranges::reverse_view(results))
            {
                resultsArray.push_back(std::make_pair(std::string(), r.toPropertyTree()));
            }
        }
        else
        {
            for (const auto& r : results)
            {
                resultsArray.push_back(std::make_pair(std::string(), r.toPropertyTree()));
            }
        }
        pt.add_child("results", resultsArray);

        return pt;
    }
};

enum class InputType
{
    UpArrow,
    DownArrow,
    Backspace,
    PrintableChar,
    Newline,
    SearchString,
    Unknown
};

struct InputEvent
{
    InputType type{InputType::Unknown};
    std::optional<char> character;  // Valid only if type is PrintableChar
    std::string searchString;       // Current search string after input
};

/// @class InputInterface
/// @brief An interface for reading input from the user.
class InputInterface
{
   public:
    /// @brief Virtual destructor for InputInterface.
    virtual ~InputInterface() = default;

    /// @brief Get next event
    virtual InputEvent getNextEvent() = 0;

    /// Write results to the output.
    /// @param results The results to write.
    virtual void writeResults(const Results& results) = 0;

    /// @brief Update progress indicator.
    /// @param count Number of lines processed or spinner step.
    virtual void updateProgress(size_t count) = 0;

    /// @brief Write final result
    /// @param result The final result to write.
    virtual void writeFinalResult(const std::string& result) = 0;
};
}  // namespace fzf