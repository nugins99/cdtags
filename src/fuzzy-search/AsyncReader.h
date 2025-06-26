#pragma once
#include "Reader.h"
#include <istream>
#include <stdexcept>
#include <thread>
#include <iosfwd>
#include <iostream>


namespace fzf
{

class AsyncReader : public Reader
{
   public:
    AsyncReader();
    ~AsyncReader();

    // Disable copy and move semantics
    AsyncReader(const AsyncReader&) = delete;
    AsyncReader& operator=(const AsyncReader&) = delete;
    AsyncReader(AsyncReader&&) = delete;

    virtual void start() override;

   private:
    void read();

    std::thread m_thread;  ///< Thread for reading input asynchronously

};

}  // namespace fzf
