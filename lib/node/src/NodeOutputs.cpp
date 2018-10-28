#include "node/NodeOutputs.h"

namespace ref {

NodeOutputs::NodeOutputs(NodeDefinition::IDToTopicMap& idToTopicMap)
        : _idToTopicMap(idToTopicMap) {}

NodeOutputs::~NodeOutputs() {}

PublishedMessageMap& NodeOutputs::allMessages() {
    return _published;
}

void NodeOutputs::serialize() {}

}  // namespace ref
