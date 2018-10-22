#include "node/NodeMessages.h"

namespace ref {

NodeMessages::NodeMessages(const Time::TimePoint currentTime, const NodeDefinition::IDToTopicMap& idToTopicMap)
        : currentTime(currentTime), idToTopicMap(idToTopicMap) {}

void NodeMessages::record(Recording&) const {
    // Default implementation is a no-op because there are no messages to record
}

}  // namespace ref
