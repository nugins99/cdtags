#pragma once

namespace fzf
{

struct Result
{
    Result(std::size_t idx, const std::string& ln, bool sel, double sc)
        : index(idx), line(ln), selected(sel), score(sc)
    {
    }
    std::size_t index{0};
    std::string line;
    bool selected{false};
    double score{0.0};
};

struct Results
{
    std::string searchString;
    std::vector<Result> results;
    std::size_t totalResults{0};
    std::pair<std::size_t, std::size_t> resultRange;
};

enum class InputType
{
    UpArrow,
    DownArrow,
    Backspace,
    PrintableChar,
    Newline,
    Unknown
};

struct InputEvent
{
    InputType type{InputType::Unknown};
    std::optional<char> character;  // Valid only if type is PrintableChar
    std::string searchString;      // Current search string after input
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
    virtual void updateProgress(size_t count)  =0;   
};
}  // namespace fzf