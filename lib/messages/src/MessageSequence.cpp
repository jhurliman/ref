#include "messages/MessageSequence.h"

namespace ref {

constexpr size_t INDEX_SIZE = sizeof(uint32_t);
constexpr size_t LENGTH_SIZE = sizeof(uint32_t);

MessageSequence::MessageSequence(
        const Recording& recording,
        std::ifstream& file,
        size_t messageOffset,
        size_t fileLength)
        : _file(file)
        , _curOffset(messageOffset)
        , _fileLength(fileLength)
        , _curValue(recording, 0, 0) {}

MessageSequence::operator bool() const {
    size_t filePos = _curOffset + INDEX_SIZE + LENGTH_SIZE;
    return !_done && filePos < _fileLength;
}

RecordedMessage const& MessageSequence::operator*() const {
    return _curValue;
}

RecordedMessage const* MessageSequence::operator->() const {
    return &_curValue;
}

bool MessageSequence::operator==(MessageSequence const& other) const {
    return bool(this) == bool(other);
}

bool MessageSequence::operator!=(MessageSequence const& other) const {
    return bool(this) != bool(other);
}

MessageSequence& MessageSequence::operator++() {
    assert(!_done);

    size_t filePos = _curOffset + INDEX_SIZE + LENGTH_SIZE;
    if (filePos >= _fileLength) {
        _done = true;
        return *this;
    }

    // Topic index
    _file.read(reinterpret_cast<char*>(&_curValue._topicIndex), INDEX_SIZE);

    // Message length
    _file.read(reinterpret_cast<char*>(&_curValue._length), LENGTH_SIZE);

    if (filePos + _curValue._length > _fileLength) {
        _done = true;
        return *this;
    }

    // Message value
    _curValue._data.reserve(_curValue._length);
    _file.read(_curValue._data.data(), _curValue._length);

    _curOffset += INDEX_SIZE + LENGTH_SIZE + _curValue._length;

    return *this;
}

MessageSequence MessageSequence::operator++(int) {
    MessageSequence const tmp(*this);
    ++*this;
    return tmp;
}

}  // namespace ref
