#pragma once
#include <iosfwd>
#include <iostream>
#include <istream>
#include <stdexcept>
#include <thread>

#include "Reader.h"

namespace fzf
{

class StdinReader : public Reader
{
   public:
    StdinReader();
    ~StdinReader();

    // Disable copy and move semantics
    StdinReader(const StdinReader&) = delete;
    StdinReader& operator=(const StdinReader&) = delete;
    StdinReader(StdinReader&&) = delete;

    virtual void start() override;

   private:
    void read();

    std::thread m_thread;  ///< Thread for reading input asynchronously
};

}  // namespace fzf
