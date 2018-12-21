#include "node/NodeOutputs.h"

namespace ref {

NodeOutputs::NodeOutputs(const NodeDefinition::IDToTopicMap& idToTopicMap)
        : _idToTopicMap(idToTopicMap) {
    // Initialize the map of published messages with empty entries for each published topic
    for (auto&& entry : _idToTopicMap) {
        const std::string& topicName = entry.second.name;
        _published[topicName] = nullptr;
    }
}

NodeOutputs::~NodeOutputs() {}

void NodeOutputs::clear() {
    for (auto&& entry : _published) {
        _published[entry.first] = nullptr;
    }
}

PublishedMessageMap& NodeOutputs::allMessages() {
    return _published;
}

void NodeOutputs::serialize() {}

}  // namespace ref
