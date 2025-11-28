#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "Controller.h"
#include "ModelInterface.h"

// Mock Model for testing
class MockModel : public fzf::ModelInterface
{
   public:
    const std::string & result() const override { return selected; }
    std::string searchString() const override { return search; }
    void setSearchString(std::string s) override { search = std::move(s); }
    int getSelectedIndex() const override { return index; }
    void setSelectedIndex(int i) override { index = i; selectedIndices.push_back(i); }
    std::size_t size() const override { return resultsSize; }
    std::string selected;
    std::string search;
    int index = -1;
    std::size_t resultsSize = 0;
    std::vector<int> selectedIndices;  ///< Store selected indices for testing
};

class MockInput : public fzf::InputInterface
{
   public:

    virtual fzf::InputEvent getNextEvent() override
    {
        fzf::InputEvent event;
        event.type = fzf::InputType::PrintableChar;
        event.character = inputs.front();
        inputs.erase(0, 1);  // Remove the first character
        return event;
    }
   /// Write results to the output.
    /// @param results The results to write.
    virtual void writeResults(const fzf::Results& ) override {};

    /// @brief Update progress indicator.
    /// @param count Number of lines processed or spinner step.
    virtual void updateProgress(size_t ) override {};   

    void writeFinalResult(const std::string& ) override {}

    std::string inputs;  ///< Character to return on next getch call
};

TEST(ControllerTest, HandlesUpAndDownArrow)
{
    MockInput input;
    input.inputs = "\033[A\033[B\n";  // Simulate up and down arrow keys
    MockModel model;
    fzf::Controller controller(input, model);
    model.resultsSize = 3;
    model.setSelectedIndex(1);
    controller.run();
    // TODO - add test to verify the inital down arrow bypasses "0"

    EXPECT_EQ(model.getSelectedIndex(), 1);
    EXPECT_EQ(model.selectedIndices.size(), 3);  // Check if both up and down were processed
    EXPECT_EQ(model.selectedIndices[0], 1);  // Check if indices are as expected
    EXPECT_EQ(model.selectedIndices[1], 0);  // Check if indices are as expected
    EXPECT_EQ(model.selectedIndices[2], 1);  // Check if indices are as expected
    EXPECT_EQ(controller.stopped(), true);  // Check if controller stopped after newline
}

TEST(ControllerTest, HandlesSearchString)
{
    MockInput input;
    input.inputs = "ab\n";  // Simulate input characters followed by newline
    MockModel model;
    fzf::Controller controller(input, model);
    controller.run();
    EXPECT_EQ(model.searchString(), "ab");
    EXPECT_EQ(controller.stopped(), true);  // Check if controller stopped after newline
}

TEST(ControllerTest, HandlesBackspace)
{
    MockInput input;
    input.inputs = "abc\177\n";  // Simulate input characters followed by backspace and newline
    MockModel model;
    fzf::Controller controller(input, model);
    controller.run();
    EXPECT_EQ(model.searchString(), "ab");
}
