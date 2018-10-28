#include "node/NodeInputs.h"

#include <core/Assert.h>

namespace ref {

NodeInputs::NodeInputs(Time::TimePoint currentTime, NodeDefinition::IDToTopicMap& idToTopicMap)
        : _currentTime(currentTime), _idToTopicMap(idToTopicMap) {
    for (auto&& entry : idToTopicMap) {
        const std::string& topicName = entry.second.name;
        _received.try_emplace(topicName, entry.second);
    }
}

NodeInputs::~NodeInputs() {}

const Time::TimePoint NodeInputs::currentTime() const {
    return _currentTime;
}

const ReceivedMessageMap& NodeInputs::allMessages() const {
    return _received;
}

void NodeInputs::copyFromOutputs(const PublishedMessageMap& outputs) {
    for (auto&& entry : outputs) {
        if (!entry.second) {
            continue;
        }

        const auto it = _received.find(entry.first);
        if (it == _received.end()) {
            continue;
        }

        const PublishedMessageBase* output = entry.second;
        ReceivedMessage& input = it->second;

        uint8_t* srcData = output->builder.GetBufferPointer();
        input.data.assign(srcData, srcData + output->builder.GetSize());
    }
}

void NodeInputs::deserialize() {}

const ReceivedMessage* NodeInputs::operator[](const std::string& topicName) const {
    auto it = _received.find(topicName);
    if (it == _received.end()) {
        return {};
    }
    return &it->second;
}

}  // namespace ref
