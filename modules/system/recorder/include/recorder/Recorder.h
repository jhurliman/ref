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

    NodeInputs* inputs() override;
    NodeOutputs* outputs() override;
    void tick() override;

private:
    std::unique_ptr<Recording> _curRecording;
    NodeInputs _input;
    NodeOutputs _output;
};

}  // namespace ref
