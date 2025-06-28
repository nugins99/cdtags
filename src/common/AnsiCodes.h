#ifndef ANSI_CODES_H
#define ANSI_CODES_H

#include <string_view>

/// @name ANSI Escape Codes
namespace ansi {
    namespace text {
        inline const std::string_view bold = "\033[1m";
        inline const std::string_view underline = "\033[4m";
        inline const std::string_view strikethrough = "\033[9m";
        inline const std::string_view blink = "\033[5m";
        inline const std::string_view hidden = "\033[8m";
        inline const std::string_view inverse = "\033[7m";
        inline const std::string_view normal = "\033[0m"; // Reset all attributes
    }
    namespace fg {
        inline const std::string_view red = "\033[31m";
        inline const std::string_view green = "\033[32m";
        inline const std::string_view yellow = "\033[33m";
        inline const std::string_view blue = "\033[34m";
        inline const std::string_view magenta = "\033[35m";
        inline const std::string_view cyan = "\033[36m";
        inline const std::string_view white = "\033[37m";
        inline const std::string_view gray = "\033[90m";
        inline const std::string_view reset = "\033[39m"; // Reset text color to default
    }
    namespace bg {
        inline const std::string_view black = "\033[40m";
        inline const std::string_view red = "\033[41m";
        inline const std::string_view green = "\033[42m";
        inline const std::string_view yellow = "\033[43m";
        inline const std::string_view blue = "\033[44m";
        inline const std::string_view magenta = "\033[45m";
        inline const std::string_view cyan = "\033[46m";
        inline const std::string_view white = "\033[47m";
        inline const std::string_view gray = "\033[100m";
        inline const std::string_view reset = "\033[49m"; // Reset background color
    }
    namespace reset {
        inline const std::string_view all = "\033[0m";
        inline const std::string_view bold = "\033[22m";
        inline const std::string_view underline = "\033[24m";
        inline const std::string_view fg = "\033[39m";
        inline const std::string_view bg = "\033[49m";
    }
    namespace cursor {
        inline const std::string_view save = "\033[s";
        inline const std::string_view restore = "\033[u";
        inline const std::string_view home = "\033[H";
        inline const std::string_view clearLine = "\033[2K";
        inline const std::string_view clearScreen = "\033[2J\033[H";
        // To move cursor to (row, col): use std::format("\033[{};{}H", row, col)
    }
}

#endif  // ANSI_CODES_H
