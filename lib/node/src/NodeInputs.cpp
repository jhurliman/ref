#include "node/NodeInputs.h"

#include <core/Assert.h>

namespace ref {

NodeInputs::NodeInputs(const NodeDefinition::IDToTopicMap& idToTopicMap)
        : _currentTime(Time::FromNanoseconds(0)), _idToTopicMap(idToTopicMap) {
    for (auto&& entry : idToTopicMap) {
        const NodeDefinition::Topic& topic = entry.second;
        _received.try_emplace(topic.name, topic);
    }
}

NodeInputs::~NodeInputs() {}

const Time::TimePoint NodeInputs::currentTime() const {
    return _currentTime;
}

const ReceivedMessageMap& NodeInputs::allMessages() const {
    return _received;
}

void NodeInputs::setCurrentTime(const Time::TimePoint now) {
    _currentTime = now;
}

void NodeInputs::copyFromOutputs(const PublishedMessageMap& outputs) {
    for (auto&& entry : outputs) {
        copyFromOutput(entry.second);
    }
}

void NodeInputs::copyFromOutput(const PublishedMessageBase* message) {
    if (!message) {
        return;
    }

    const auto it = _received.find(message->topic.name);
    if (it == _received.end()) {
        return;
    }

    ReceivedMessage& input = it->second;
    uint8_t* srcData = message->builder.GetBufferPointer();
    input.data.assign(srcData, srcData + message->builder.GetSize());
}

void NodeInputs::clear() {
    for (auto&& entry : _received) {
        entry.second.data.clear();
    }
}

void NodeInputs::deserialize() {}

const ReceivedMessage* NodeInputs::operator[](const std::string& topicName) const {
    auto it = _received.find(topicName);
    if (it == _received.end()) {
        return nullptr;
    }
    return &it->second;
}

}  // namespace ref
