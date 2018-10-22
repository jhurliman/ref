#include "node/NodeMessages.h"

namespace ref {

NodeMessages::NodeMessages(const Time::TimePoint currentTime_, const NodeDefinition::IDToTopicMap& idToTopicMap_)
        : currentTime(currentTime_), idToTopicMap(idToTopicMap_) {}

void NodeMessages::record(Recording&) const {
    // Default implementation is a no-op because there are no messages to record
}

}  // namespace ref
