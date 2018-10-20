#pragma once

#include <core/Time.h>
#include <memory>
#include <messages/Recording.h>
#include <node/NodeDefinition.h>

namespace ref {

class NodeBase;

struct NodeMessages {
    const Time::TimePoint currentTime;
    const NodeDefinition::IDToTopicMap& idToTopicMap;

    NodeMessages(
            const Time::TimePoint currentTime,
            const NodeDefinition::IDToTopicMap& idToTopicMap);

    virtual void record(Recording& recording) const;

    template<class Message, class MessageT>
    void recordMessage(const std::string& id, const MessageT* msg, Recording& recording) const {
        if (!msg) {
            return;
        }

        const auto& it = idToTopicMap.find(id);
        if (it != idToTopicMap.end()) {
            const std::string& topicName = it->second.first;
            recording.write<Message, MessageT>(topicName, *msg);
        }
    };
};

}  // namespace ref
