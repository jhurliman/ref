#pragma once

#include "RecordedMessage.h"

#include <fstream>
#include <iterator>

namespace ref {

class MessageSequence {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = RecordedMessage;
    using difference_type = void;
    using pointer = RecordedMessage*;
    using reference = RecordedMessage&;

    MessageSequence(
            const Recording& recording,
            std::ifstream& file,
            std::streampos messageOffset,
            size_t fileLength);

    explicit operator bool() const;

    RecordedMessage const& operator*() const;
    RecordedMessage const* operator->() const;

    bool operator==(MessageSequence const& other) const;
    bool operator!=(MessageSequence const& other) const;

    MessageSequence& operator++();
    MessageSequence operator++(int);

private:
    std::ifstream& _file;
    size_t _curOffset = 0;
    size_t _fileLength = 0;
    bool _done = false;
    RecordedMessage _curValue;
};

}  // namespace ref
