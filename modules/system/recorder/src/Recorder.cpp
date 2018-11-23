#include "recorder/Recorder.h"

#include <core/Format.h>

namespace ref {

Recorder::Recorder(const NodeDefinition& def, const Graph& graph)
        : NodeBase(def, graph), _input(def.inputs()), _output(def.outputs()) {
    std::string folder = def.parameters().readString(STR(folder), "/tmp");
    std::string file_pattern = def.parameters().readString(STR(file_pattern), "recording_%s.rec");
    // int32_t max_bytes = def.parameters().readInt(STR(max_bytes));

    std::string filename = folder + "/" + Format(file_pattern.c_str(), "001");

    std::unordered_map<std::string, size_t> typeMap;
    Recording::TypesList types;
    Recording::TopicsList topics;

    for (auto&& entry : graph.types()) {
        messages::recording::TypeDefinitionT type;
        type.name = entry.name;
        type.hash = entry.hash;
        type.schema = entry.schema;
        types.push_back(std::move(type));
        typeMap[type.name] = types.size() - 1;
    }

    for (auto&& entry : graph.topics()) {
        messages::recording::TopicDefinitionT topic;
        topic.topic_name = entry.name;
        topic.type_index = uint32_t(typeMap[entry.type.name]);
    }

    _curRecording = std::make_unique<Recording>(filename, std::move(types), std::move(topics));
}

NodeInputs* Recorder::inputs() {
    return &_input;
}

NodeOutputs* Recorder::outputs() {
    return &_output;
}

void Recorder::tick() {
    // FIXME:
    // _input.record(*_curRecording);
}

}  // namespace ref
