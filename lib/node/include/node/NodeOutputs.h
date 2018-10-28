#pragma once

#include "NodeDefinition.h"
#include "PublishedMessage.h"

namespace ref {

class NodeOutputs {
public:
    NodeOutputs(NodeDefinition::IDToTopicMap& idToTopicMap);
    virtual ~NodeOutputs();
    PublishedMessageMap& allMessages();
    virtual void serialize();

protected:
    NodeDefinition::IDToTopicMap& _idToTopicMap;
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
