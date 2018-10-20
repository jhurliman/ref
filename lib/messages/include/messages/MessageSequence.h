#include "RecordedMessage.h"

#include <fstream>
#include <iterator>

namespace ref {

class MessageSequence : public std::iterator<std::input_iterator_tag, RecordedMessage> {
public:
    MessageSequence(
            const Recording& recording,
            std::ifstream& file,
            size_t messageOffset,
            size_t fileLength);

    explicit operator bool() const;

    RecordedMessage const& operator*() const;
    RecordedMessage const* operator->() const;

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
