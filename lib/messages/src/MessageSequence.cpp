#include "messages/MessageSequence.h"

namespace ref {

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
    return !_done;
}

RecordedMessage const& MessageSequence::operator*() const {
    return _curValue;
}

RecordedMessage const* MessageSequence::operator->() const {
    return &_curValue;
}

MessageSequence& MessageSequence::operator++() {
    assert(!_done);

    constexpr size_t INDEX_SIZE = sizeof(uint32_t);
    constexpr size_t LENGTH_SIZE = sizeof(uint32_t);

    if (_curOffset + INDEX_SIZE + LENGTH_SIZE > _fileLength) {
        _done = true;
        return *this;
    }

    // Topic index
    _file.read(reinterpret_cast<char*>(&_curValue._topicIndex), INDEX_SIZE);

    // Message length
    _file.read(reinterpret_cast<char*>(&_curValue._length), LENGTH_SIZE);

    if (_curOffset + INDEX_SIZE + LENGTH_SIZE + _curValue._length > _fileLength) {
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
