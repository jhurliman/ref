#include "node/NodeMessages.h"

namespace ref {

NodeMessages::NodeMessages(const Time::TimePoint currentTime, const NodeDefinition::IDToTopicMap& idToTopicMap)
        : currentTime(currentTime), idToTopicMap(idToTopicMap) {}

}  // namespace ref
