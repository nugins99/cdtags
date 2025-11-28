/// @file JSONRPCInterface.cpp
/// @brief Implementation of the JSON-RPC InputInterface.

#include "JSONRPCInterface.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iomanip>
#include <sstream>

namespace fzf
{
JSONRPCInterface::JSONRPCInterface(std::istream& in, std::ostream& out) : m_in(in), m_out(out) {}

InputEvent JSONRPCInterface::getNextEvent()
{
    InputEvent event;
    std::string line;
    if (!std::getline(m_in, line))
    {
        // EOF or error — return Unknown
        event.type = InputType::Unknown;
        return event;
    }
    // Parse JSON using Boost.PropertyTree
    boost::property_tree::ptree pt;
    try
    {
        std::istringstream iss(line);
        boost::property_tree::read_json(iss, pt);
    }
    catch (...)  // parsing failed
    {
        event.type = InputType::Unknown;
        return event;
    }

    auto getString = [&](const std::string& key) -> std::string
    {
        boost::optional<std::string> val = pt.get_optional<std::string>(key);
        if (val) return *val;
        std::string paramsKey = std::string("params.") + key;
        val = pt.get_optional<std::string>(paramsKey);
        if (val) return *val;
        return {};
    };

    // Minimal JSON parsing: check method
    const std::string method = getString("method");
    if (method != "input")
    {
        event.type = InputType::Unknown;
        return event;
    }

    const std::string type = getString("type");
    if (type == "UpArrow")
    {
        event.type = InputType::UpArrow;
    }
    else if (type == "DownArrow")
    {
        event.type = InputType::DownArrow;
    }
    else if (type == "Backspace")
    {
        // update search string
        if (!m_searchString.empty()) m_searchString.pop_back();
        event.type = InputType::Backspace;
        event.searchString = m_searchString;
    }
    else if (type == "Newline")
    {
        event.type = InputType::Newline;
    }
    else if (type == "PrintableChar")
    {
        std::string ch = getString("character");
        if (ch.empty()) ch = getString("char");
        if (!ch.empty())
        {
            event.type = InputType::PrintableChar;
            event.character = ch[0];
            m_searchString.push_back(ch[0]);
            event.searchString = m_searchString;
        }
        else
        {
            event.type = InputType::Unknown;
        }
    }
    else
    {
        event.type = InputType::Unknown;
    }

    // for other cases where the search string was not set above, capture it
    if (event.searchString.empty())
    {
        event.searchString = m_searchString;
    }
    return event;
}

void JSONRPCInterface::writeResults(const Results& results)
{
    boost::property_tree::ptree root;
    root.put("jsonrpc", "2.0");
    root.put("method", "results");
    root.add_child("params", results.toPropertyTree());
    // write_json will pretty-print by default; pass false to avoid extra indentation
    boost::property_tree::write_json(m_out, root, false);
    m_out.flush();
}

void JSONRPCInterface::updateProgress(size_t count)
{
    std::ostringstream ss;
    ss << "{\"jsonrpc\":\"2.0\",\"method\":\"progress\",\"params\":{";
    ss << "\"count\":" << count << "}}\n";
    m_out << ss.str();
    m_out.flush();
}

// @brief Write final result
void JSONRPCInterface::writeFinalResult(const std::string& result)
{
    std::ostringstream ss;
    ss << "{\"jsonrpc\":\"2.0\",\"method\":\"finalResult\",\"params\":{";
    ss << "\"result\":\"" << result << "\"}}\n";
    m_out << ss.str();
    m_out.flush();
}

}  // namespace fzf
