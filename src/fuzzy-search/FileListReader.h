#include <thread>
#include <atomic>
#include "Reader.h"

namespace fzf {
class FileListReader : public Reader {
public:

    enum class SearchType
    {
        Files,
        Directories
    };

    FileListReader(const std::string& rootPath, SearchType searchType)
        : m_rootPath(rootPath), m_searchType(searchType), m_stopFlag(false) {}

    void start() override {
        m_stopFlag = false;
        m_thread = std::thread([this]() { this->run(); });
    }

    void stop() override {
        m_stopFlag = true;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    ~FileListReader() { stop(); }

private:
    void run() {
        std::unordered_set<std::string> skipDirs = {".git", ".svn", ".hg", ".bzr"};
        for (auto it = std::filesystem::recursive_directory_iterator(m_rootPath, std::filesystem::directory_options::skip_permission_denied);
             it != std::filesystem::recursive_directory_iterator(); ++it) {
            if (m_stopFlag) break;
            if (it->is_directory()) {
                std::string name = it->path().filename().string();
                if (skipDirs.count(name)) {
                    it.disable_recursion_pending();
                    continue;
                }
                if (m_searchType == SearchType::Directories) {
                    addLine(it->path().string());
                }
            } else if (m_searchType == SearchType::Files && it->is_regular_file()) {
                addLine(it->path().string());
            }
        }
        setEndOfFile();
    }

    std::string m_rootPath;
    SearchType m_searchType;
    std::thread m_thread;
    std::atomic<bool> m_stopFlag;
};
}