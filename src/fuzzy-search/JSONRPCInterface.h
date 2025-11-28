/// @file JSONRPCInterface.h
/// @brief JSON-RPC based input/output implementation of InputInterface.

#pragma once

#include "InputInterface.h"
#include <istream>
#include <ostream>
#include <optional>
#include <string>
#include <sstream>
#include <vector>

namespace fzf
{
/// @brief A simple JSON-RPC interface that speaks JSON over arbitrary
/// `std::istream`/`std::ostream` pairs. This class expects incoming JSON-RPC
/// requests (one JSON object per line) describing input events and emits
/// JSON-RPC notifications for results and progress.
///
/// NOTE: This implementation intentionally keeps JSON handling lightweight and
/// tailored to the project's needs: it does minimal, robust-enough string
/// parsing for the expected messages (method=="input" with params). It is
/// not a full JSON parser — if you require full JSON-RPC compliance, replace
/// the parsing with a proper JSON library (e.g. nlohmann::json).
class JSONRPCInterface : public InputInterface
{
  public:
    /// Construct with input and output streams (not owned).
    explicit JSONRPCInterface(std::istream& in, std::ostream& out);
    ~JSONRPCInterface() override = default;

    JSONRPCInterface(const JSONRPCInterface&) = delete;
    JSONRPCInterface& operator=(const JSONRPCInterface&) = delete;

    InputEvent getNextEvent() override;
    void writeResults(const Results& results) override;
    void updateProgress(size_t count) override;
    void writeFinalResult(const std::string& result) override;

  private:
    std::istream& m_in;
    std::ostream& m_out;
    std::string m_searchString{};
};

}  // namespace fzf
