#pragma once

namespace fzf
{

    /// @class InputInterface
    /// @brief An interface for reading input from the user.
class InputInterface
{
   public:
    /// @brief Virtual destructor for InputInterface.
    virtual ~InputInterface() = default;

    /// @brief Read a single character from the input.
    /// @return The character read.
    virtual char getch() = 0;
};
}  // namespace fzf