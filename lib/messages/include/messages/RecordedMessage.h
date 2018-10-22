#pragma once

#include <string>
#include <vector>

namespace ref {

namespace messages {
namespace recording {
class TopicDefinitionT;
class TypeDefinitionT;
}  // namespace recording
}  // namespace messages

class Recording;

class RecordedMessage {
public:
    RecordedMessage(const Recording& recording, uint32_t topicIndex, uint32_t length);

    size_t size() const;
    const char* data() const;

    const std::string& topic() const;
    const std::string& typeName() const;
    const std::string& typeHash() const;
    const std::vector<uint8_t>& typeSchema() const;

private:
    const Recording& _recording;
    uint32_t _topicIndex = 0;
    uint32_t _length = 0;
    std::vector<char> _data;

    const messages::recording::TopicDefinitionT& getTopic() const;
    const messages::recording::TypeDefinitionT& getType() const;

    friend class MessageSequence;
};

}  // namespace ref
