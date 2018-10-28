#pragma once

#include "NodeDefinition.h"

#include <flatbuffers/flatbuffers.h>
#include <unordered_map>

namespace ref {

struct PublishedMessageBase {
    const NodeDefinition::Topic& topic;
    flatbuffers::FlatBufferBuilder builder;

    PublishedMessageBase(const NodeDefinition::Topic& topic_);
};

template<class MessageT>
struct PublishedMessage : PublishedMessageBase {
    using PublishedMessageBase::PublishedMessageBase;
    std::unique_ptr<MessageT> message;
};

using PublishedMessageMap = std::unordered_map<NodeDefinition::TopicName, PublishedMessageBase*>;

}  // namespace ref
