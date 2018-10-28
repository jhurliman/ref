#pragma once

#include <messages/Recording.h>
#include <node/NodeBase.h>
#include <node/NodeInputs.h>
#include <node/NodeOutputs.h>
#include <node/Parameters.h>

#include <memory>

namespace ref {

class Recorder : public NodeBase {
public:
    Recorder(const NodeDefinition& def, const Graph& graph);

    void tick(const NodeInputs& input, NodeOutputs* output);

private:
    std::unique_ptr<Recording> _curRecording;
};

}  // namespace ref
