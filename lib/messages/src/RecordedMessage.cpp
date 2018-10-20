#include "messages/RecordedMessage.h"
#include "messages/Recording.h"

#include <recording/TopicDatabase_generated.h>
#include <recording/TypeDatabase_generated.h>
#include <stdexcept>

namespace ref {

RecordedMessage::RecordedMessage(const Recording& recording, uint32_t topicIndex, uint32_t length)
        : _recording(recording), _topicIndex(topicIndex), _length(length) {}

size_t RecordedMessage::size() const {
    return _length;
}

const char* RecordedMessage::data() const {
    return _data.data();
}

const std::string& RecordedMessage::topic() const {
    return getTopic().topic_name;
}

const std::string& RecordedMessage::typeName() const {
    return getType().name;
}

const std::string& RecordedMessage::typeHash() const {
    return getType().hash;
}

const std::vector<uint8_t>& RecordedMessage::typeSchema() const {
    return getType().schema;
}

const messages::recording::TopicDefinitionT& RecordedMessage::getTopic() const {
    auto& topics = _recording.topics();
    if (_topicIndex >= topics.size()) {
        throw std::runtime_error("Topic index is out of bounds");
    }
    return topics[_topicIndex];
}

const messages::recording::TypeDefinitionT& RecordedMessage::getType() const {
    auto& topic = getTopic();
    auto& types = _recording.types();
    if (topic.type_index >= types.size()) {
        throw std::runtime_error("Type index is out of bounds");
    }
    return types[topic.type_index];
}

}  // namespace ref
