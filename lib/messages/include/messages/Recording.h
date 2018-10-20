#pragma once

#include <array>
#include <core/Format.h>
#include <core/Optional.h>
#include <core/Time.h>
#include <cstddef>
#include <fstream>
#include <mutex>
#include <recording/Header_generated.h>
#include <recording/TopicDatabase_generated.h>
#include <recording/TypeDatabase_generated.h>
#include <unordered_map>
#include <vector>

namespace ref {

class MessageSequence;

constexpr uint32_t RECORDING_MAJOR = 1;
constexpr uint32_t RECORDING_MINOR = 0;
constexpr std::array<char, 4> MAGIC = {{'R', 'E', 'F', RECORDING_MAJOR}};

class Recording {
public:
    enum class Mode { Read, Write };

    using FileVersion = std::pair<uint32_t, uint32_t>;
    using TypesList = std::vector<messages::recording::TypeDefinitionT>;
    using TopicsList = std::vector<messages::recording::TopicDefinitionT>;

    Recording(const std::string& filename);
    Recording(const std::string& filename, const TypesList& types, const TopicsList& topics);

    ~Recording();

    template<class Message, class MessageT>
    void write(const std::string& topic, const MessageT& message);
    void flush();
    void close();

    const FileVersion version() const;
    const Time::TimePoint startTime();
    const Time::TimePoint endTime();
    uint64_t messageCount();
    const TypesList& types() const;
    const TopicsList& topics() const;

    const MessageSequence readMessages();

protected:
    std::string _filename;
    Mode _mode;
    std::ifstream _infile;
    std::ofstream _outfile;
    uint32_t _minorVersion = 0;
    uint64_t _startNS = 0;
    uint64_t _endNS = 0;
    uint64_t _messageCount = 0;
    size_t _messageOffset = 0;
    size_t _fileLength = 0;
    TypesList _types;
    TopicsList _topics;
    std::unordered_map<std::string, uint32_t> _topicIndexMap;
    std::mutex _writeMutex;

    uint32_t readUInt();
    uint64_t readULong();
    void writeUInt(uint32_t value);
    void writeULong(uint64_t value);
    void buildTopicLookup();
    void serializeVariableHeader(
            flatbuffers::FlatBufferBuilder& builder,
            const TypesList& types,
            const TopicsList& topics);
};

template<class Message, class MessageT>
void Recording::write(const std::string& topic, const MessageT& message) {
    // Resolve the topic to a topic list index
    auto&& it = _topicIndexMap.find(topic);
    if (it == _topicIndexMap.end()) {
        throw std::runtime_error(Format("'%s' not found in the topic database", topic));
    }
    uint32_t topicIndex = it->second;

    // Extract the publish timestamp from the header
    uint64_t t = message.header->publish_stamp;

    // Serialize the message
    flatbuffers::FlatBufferBuilder builder;
    builder.Finish(Message::Pack(builder, &message));
    uint32_t length = builder.GetSize();

    {
        std::lock_guard<std::mutex> lock(_writeMutex);
        writeULong(t);
        writeUInt(topicIndex);
        writeUInt(length);
        _outfile.write(reinterpret_cast<const char*>(builder.GetBufferPointer()), length);
    }
}

}  // namespace ref
