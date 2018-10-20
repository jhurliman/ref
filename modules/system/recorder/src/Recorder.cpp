#include "recorder/Recorder.h"

#include <core/Format.h>

namespace ref {

Recorder::Recorder(const NodeDefinition& def) : NodeBase(def) {
    std::string folder = def.parameters().readString(STRING(folder), "/tmp");
    std::string file_pattern = def.parameters().readString(STRING(file_pattern), "recording_%s.rec");
    int32_t max_bytes = def.parameters().readInt(STRING(max_bytes));

    std::string filename = folder + "/" + Format(file_pattern.c_str(), "001");

    // FIXME: Recorder needs to be able to fetch the entire set of types and
    // topics used in a launch graph
    Recording::TypesList types;
    Recording::TopicsList topics;

    _curRecording = std::make_unique<Recording>(filename, types, topics);
}

void Recorder::tick(const NodeMessages& input, NodeMessages* output) {
    input.record(*_curRecording);
}

}  // namespace ref
