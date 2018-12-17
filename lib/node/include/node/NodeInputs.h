#pragma once

#include "NodeDefinition.h"
#include "PublishedMessage.h"
#include "ReceivedMessage.h"

#include <core/Assert.h>
#include <core/Optional.h>
#include <core/Time.h>

namespace ref {

class NodeInputs {
public:
    NodeInputs(const NodeDefinition::IDToTopicMap& idToTopicMap);
    virtual ~NodeInputs();

    const Time::TimePoint currentTime() const;
    const ReceivedMessageMap& allMessages() const;

    void setCurrentTime(const Time::TimePoint now);

    void copyFromOutputs(const PublishedMessageMap& outputs);
    void copyFromOutput(const PublishedMessageBase* message);
    void clear();
    virtual void deserialize();

    template<class Message, class MessageT>
    void deserializeMessage(const std::string& id, std::unique_ptr<MessageT>* messagePtrPtr);

    const ReceivedMessage* operator[](const std::string& topicName) const;

protected:
    Time::TimePoint _currentTime;
    const NodeDefinition::IDToTopicMap& _idToTopicMap;
    ReceivedMessageMap _received;
};

////////////////////////////////////////////////////////////////////////////////

template<class Message, class MessageT>
void NodeInputs::deserializeMessage(
        const std::string& id,
        std::unique_ptr<MessageT>* messagePtrPtr) {

    const auto idTopicIt = _idToTopicMap.find(id);
    if (idTopicIt == _idToTopicMap.end()) {
        return;
    }

    const auto recvIt = _received.find(idTopicIt->second.name);
    if (recvIt == _received.end()) {
        return;
    }

    ReceivedMessage& recv = recvIt->second;
    if (!recv.data.size()) {
        *messagePtrPtr = nullptr;
        return;
    }

    *messagePtrPtr = std::make_unique<MessageT>();
    const Message* rawMessage = flatbuffers::GetRoot<Message>(recv.data.data());
    rawMessage->UnPackTo(messagePtrPtr->get());
}

}  // namespace ref
