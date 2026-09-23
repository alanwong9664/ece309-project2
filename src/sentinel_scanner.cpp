#include "core/sentinel_scanner.h"

SentinelScanner::SentinelScanner(std::string sentinel)
    : sentinel_(std::move(sentinel)), pending_() {}

std::size_t SentinelScanner::longest_sentinel_prefix_suffix(const std::string& text) const {
    std::size_t max_len = sentinel_.size() > 0 ? sentinel_.size() - 1 : 0;
    if (max_len > text.size()) {
        max_len = text.size();
    }

    for (std::size_t len = max_len; len > 0; --len) {
        if (text.compare(text.size() - len, len, sentinel_, 0, len) == 0) {
            return len;
        }
    }
    return 0;
}

SentinelScanner::Out SentinelScanner::feed(std::string_view chunk) {
    pending_.append(chunk.data(), chunk.size());

    auto pos = pending_.find(sentinel_);
    if (pos != std::string::npos) {
        std::string safe = pending_.substr(0, pos);
        std::string trailing = pending_.substr(pos + sentinel_.size());
        pending_.clear();
        return { safe + trailing, true };
    }

    std::size_t hold_len = longest_sentinel_prefix_suffix(pending_);
    std::string safe = pending_.substr(0, pending_.size() - hold_len);
    pending_ = pending_.substr(pending_.size() - hold_len);

    return { safe, false };
}

SentinelScanner::Out SentinelScanner::flush() {
    std::string safe = pending_;
    pending_.clear();
    return { safe, false };
}