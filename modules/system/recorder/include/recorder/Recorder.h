#pragma once

#include <messages/Recording.h>
#include <node/NodeBase.h>
#include <node/NodeMessages.h>
#include <node/Parameters.h>

#include <memory>

namespace ref {

class Recorder : public NodeBase {
public:
    Recorder(const NodeDefinition& def, const Graph& graph);

    void tick(const NodeMessages& input, NodeMessages* output);

private:
    std::unique_ptr<Recording> _curRecording;
};

}  // namespace ref
