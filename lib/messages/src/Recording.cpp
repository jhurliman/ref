#include "messages/Recording.h"
#include "messages/MessageSequence.h"

#include <core/Log.h>
#include <recording/TopicDatabase_generated.h>
#include <recording/TypeDatabase_generated.h>
#include <stdexcept>

namespace ref {

Recording::Recording(const std::string& filename)
        : _filename(filename), _mode(Recording::Mode::Read) {
    _infile.open(_filename, std::ios::binary | std::ios::in);
    if (!_infile.is_open()) {
        throw std::runtime_error(Format("Cannot open recording '%s' for reading", filename));
    }

    // Retrieve the total file size
    _infile.seekg(0, std::ios::end);
    _fileLength = _infile.tellg();
    if (_fileLength < 36) {
        throw std::runtime_error("Recording file is too small");
    }
    _infile.seekg(0, std::ios::beg);

    // Check the magic bytes at the beginning of the file
    std::array<char, MAGIC.size()> magic;
    _infile.read(magic.data(), magic.size());
    if (magic != MAGIC) {
        throw std::runtime_error("Unrecognized recording format");
    }

    _minorVersion = readUInt();
    _startNS = readULong();
    _endNS = readULong();
    _messageCount = readULong();

    uint32_t varHeaderLength = readUInt();
    if (varHeaderLength > _fileLength - _infile.tellg()) {
        throw std::runtime_error("Invalid variable header length");
    }

    // Read the raw variable length header data
    std::vector<char> headerData(varHeaderLength);
    _infile.read(headerData.data(), varHeaderLength);

    // Parse the variable length header
    messages::recording::HeaderT variableHeader;
    auto rawHeader = messages::recording::GetHeader(headerData.data());
    rawHeader->UnPackTo(&variableHeader);

    auto&& types = variableHeader.types.get()->definitions;
    _types.reserve(types.size());
    for (auto&& type : types) {
        _types.push_back(*type);
    }

    auto&& topics = variableHeader.topics.get()->definitions;
    _topics.reserve(topics.size());
    for (auto&& topic : topics) {
        _topics.push_back(*topic);
    }

    buildTopicLookup();

    // Save the file offset to where messages start
    _messageOffset = _infile.tellg();
}

Recording::Recording(
        const std::string& filename,
        const Recording::TypesList& types,
        const Recording::TopicsList& topics)
        : _filename(filename), _mode(Recording::Mode::Write) {
    std::lock_guard<std::mutex> lock(_writeMutex);

    _outfile.open(_filename, std::ios::binary | std::ios::out);
    if (!_outfile.is_open()) {
        throw std::runtime_error(Format("Cannot open recording '%s' for writing", filename));
    }

    _types = types;
    _topics = topics;
    buildTopicLookup();

    // Serialize the variable length header data
    flatbuffers::FlatBufferBuilder builder;
    serializeVariableHeader(builder, types, topics);
    uint32_t varHeaderLength = builder.GetSize();

    // Write the fixed header fields. Most of these are placeholders that will
    // be updated with real values when the recording is finalized
    _outfile.write(MAGIC.data(), MAGIC.size());
    writeUInt(RECORDING_MINOR);
    writeULong(0);  // start_ns
    writeULong(0);  // end_ns
    writeULong(0);  // message_count
    writeUInt(varHeaderLength);

    // Write the variable length header fields
    _outfile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), varHeaderLength);

    // Set the file offset to where messages start
    _messageOffset = _outfile.tellp();
}

Recording::~Recording() {
    close();
}

void Recording::flush() {
    std::lock_guard<std::mutex> lock(_writeMutex);
    _outfile.flush();
}

void Recording::close() {
    if (_infile.is_open()) {
        _infile.close();
    } else if (_outfile.is_open()) {
        std::lock_guard<std::mutex> lock(_writeMutex);

        // Write final values to the fixed header fields
        _outfile.seekp(8);
        writeULong(_startNS);
        writeULong(_endNS);
        writeULong(_messageCount);

        _outfile.close();
    }
}

const Recording::FileVersion Recording::version() const {
    return {RECORDING_MAJOR, _minorVersion};
}

const Time::TimePoint Recording::startTime() {
    return Time::FromNanoseconds(_startNS);
}

const Time::TimePoint Recording::endTime() {
    return Time::FromNanoseconds(_endNS);
}

uint64_t Recording::messageCount() {
    return _messageCount;
}

const Recording::TypesList& Recording::types() const {
    return _types;
}

const Recording::TopicsList& Recording::topics() const {
    return _topics;
}

const MessageSequence Recording::readMessages() {
    return MessageSequence(*this, _infile, _messageOffset, _fileLength);
}

uint32_t Recording::readUInt() {
    uint32_t value = 0;
    _infile.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return value;
}

uint64_t Recording::readULong() {
    uint64_t value = 0;
    _infile.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
    return value;
}

void Recording::writeUInt(uint32_t value) {
    _outfile.write(reinterpret_cast<const char*>(&value), sizeof(uint32_t));
}

void Recording::writeULong(uint64_t value) {
    _outfile.write(reinterpret_cast<const char*>(&value), sizeof(uint64_t));
}

void Recording::buildTopicLookup() {
    for (size_t i = 0; i < _topics.size(); ++i) {
        _topicIndexMap[_topics[i].topic_name] = i;
    }
}

void Recording::serializeVariableHeader(
        flatbuffers::FlatBufferBuilder& builder,
        const TypesList& types,
        const TopicsList& topics) {
    // Type database
    std::vector<flatbuffers::Offset<messages::recording::TypeDefinition>> typeEntriesOffsets;
    for (auto&& type : types) {
        typeEntriesOffsets.push_back(messages::recording::CreateTypeDefinition(builder, &type));
    }
    auto typesOffset = messages::recording::CreateTypeDatabase(
            builder, builder.CreateVector(typeEntriesOffsets));

    // Topic database
    std::vector<flatbuffers::Offset<messages::recording::TopicDefinition>> topicEntriesOffsets;
    for (auto&& topic : topics) {
        topicEntriesOffsets.push_back(messages::recording::CreateTopicDefinition(builder, &topic));
    }
    auto topicsOffset = messages::recording::CreateTopicDatabase(
            builder, builder.CreateVector(topicEntriesOffsets));

    builder.Finish(messages::recording::CreateHeader(builder, typesOffset, topicsOffset));
}

}  // namespace ref
