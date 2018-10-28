#pragma once

#include "NodeDefinition.h"

#include <vector>

namespace ref {

struct ReceivedMessage {
    const NodeDefinition::Topic& topic;
    std::vector<uint8_t> data;

    ReceivedMessage(const NodeDefinition::Topic& topic_);
};

using ReceivedMessageMap = std::unordered_map<NodeDefinition::TopicName, ReceivedMessage>;

}  // namespace ref
