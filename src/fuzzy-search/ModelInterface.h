#pragma once
#include <string>
namespace fzf
{
/// @class ModelInterface
/// @brief An interface for the fuzzy search model.
class ModelInterface
{
   public:
    virtual ~ModelInterface() = default;

    /// @brief Get the number of results in the model.
    /// @return The number of results.
    virtual std::size_t size() const = 0;

    /// @brief Get the result at the specified index.
    /// @param index The index of the result to retrieve.
    /// @return The result string at the specified index.
    virtual const std::string & result() const = 0;

    /// @brief Get the current search string.
    /// @return The current search string.
    virtual std::string searchString() const = 0;

    /// @brief Set the search string and update the model.
    /// @param searchString The new search string to set.
    virtual void setSearchString(std::string searchString) = 0;

    /// @brief Get the currently selected result index.
    /// @return The index of the currently selected result.
    virtual int getSelectedIndex() const = 0;

    /// @brief Set the currently selected result index.
    /// @param index The index to set as selected.
    virtual void setSelectedIndex(int index) = 0;
};

}  // namespace fzf