#include "node/NodeOutputs.h"

namespace ref {

NodeOutputs::NodeOutputs(const NodeDefinition::IDToTopicMap& idToTopicMap)
        : _idToTopicMap(idToTopicMap) {}

NodeOutputs::~NodeOutputs() {}

void NodeOutputs::clear() {
    _published.clear();
}

const PublishedMessageMap& NodeOutputs::allMessages() const {
    return _published;
}

void NodeOutputs::serialize() {}

}  // namespace ref
