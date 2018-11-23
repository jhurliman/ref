#pragma once

#include "NodeDefinition.h"
#include "PublishedMessage.h"

namespace ref {

class NodeOutputs {
public:
    NodeOutputs(const NodeDefinition::IDToTopicMap& idToTopicMap);
    virtual ~NodeOutputs();
    const PublishedMessageMap& allMessages() const;
    void clear();
    virtual void serialize();

protected:
    const NodeDefinition::IDToTopicMap& _idToTopicMap;
    PublishedMessageMap _published;

    template<class Message, class MessageT>
    void serializeMessage(PublishedMessage<MessageT>& output);
};

template<class Message, class MessageT>
void NodeOutputs::serializeMessage(PublishedMessage<MessageT>& output) {
    output.builder.Clear();
    const std::string& topicName = output.topic.name;
    if (!topicName.empty()) {
        if (output.message) {
            output.builder.Finish(Message::Pack(output.builder, output.message.get()));
            _published[topicName] = &output;
        } else {
            _published[topicName] = nullptr;
        }
    }
}

}  // namespace ref
