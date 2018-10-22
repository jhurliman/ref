#include <messages/Recording.h>
#include <sensors/Image_generated.h>

#include <cstdlib>
#include <flatbuffers/flatbuffers.h>
#include <gtest/gtest.h>
#include <memory>

namespace ref {

std::string GetTempDir() {
    const char* tmpDir = std::getenv("TEST_TMPDIR");
    return tmpDir ? tmpDir : "/tmp";
}

uint32_t readUInt(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint32_t));
    return value;
}

uint64_t readULong(std::ifstream& file) {
    uint64_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(uint64_t));
    return value;
}

TEST(Recording, ReadEmpty) {
    Recording rec{"lib/messages/test/data/empty.rec"};

    auto expectedVersion = std::make_pair<uint32_t, uint32_t>(1, 0);

    EXPECT_EQ(expectedVersion, rec.version());
    EXPECT_EQ(Time::FromNanoseconds(0), rec.startTime());
    EXPECT_EQ(Time::FromNanoseconds(0), rec.endTime());
    EXPECT_EQ(0, rec.size());
    EXPECT_EQ(rec.begin(), rec.end());
}

TEST(Recording, ReadSimple) {
    // FIXME
}

TEST(Recording, ReadBroken) {
    EXPECT_THROW({ Recording("lib/messages/test/data/missing.rec"); }, std::runtime_error);

    EXPECT_THROW({ Recording("lib/messages/test/data/zerobytes.rec"); }, std::runtime_error);
}

TEST(Recording, WriteEmpty) {
    std::string filename = GetTempDir() + "/out.rec";

    Recording::TypesList types;
    Recording::TopicsList topics;
    Recording rec(filename, types, topics);
    rec.close();

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    std::array<char, 4> magic;
    file.read(magic.data(), magic.size());
    EXPECT_EQ(MAGIC, magic);

    uint32_t version = readUInt(file);
    EXPECT_EQ(RECORDING_MINOR, version);

    uint64_t startNS = readULong(file);
    EXPECT_EQ(0, startNS);

    uint64_t endNS = readULong(file);
    EXPECT_EQ(0, endNS);

    uint64_t messageCount = readULong(file);
    EXPECT_EQ(0, messageCount);

    // Variable header length
    uint32_t headerLength = readUInt(file);
    ASSERT_EQ(56, headerLength);

    // Variable header
    std::array<char, 56> headerData;
    file.read(headerData.data(), headerLength);
    auto header = messages::recording::UnPackHeader(headerData.data());
    ASSERT_TRUE(header.get());
    ASSERT_TRUE(header->types.get());
    EXPECT_EQ(0, header->types->definitions.size());
    ASSERT_TRUE(header->topics.get());
    EXPECT_EQ(0, header->topics->definitions.size());

    // Total recording length
    file.seekg(0, file.end);
    size_t fileLength = file.tellg();
    ASSERT_EQ(36 + headerLength, fileLength);
}

TEST(Recording, WriteSimple) {
    std::string filename = GetTempDir() + "/out.rec";

    messages::recording::TypeDefinitionT typeA;
    typeA.name = "TypeA";

    messages::recording::TypeDefinitionT typeB;
    typeB.name = "TypeB";

    messages::recording::TopicDefinitionT topicA;
    topicA.topic_name = "/a";
    topicA.type_index = 0;

    messages::recording::TopicDefinitionT topicB;
    topicB.topic_name = "/b";
    topicB.type_index = 1;

    Recording::TypesList types{typeA, typeB};
    Recording::TopicsList topics{topicA, topicB};
    Recording rec(filename, types, topics);

    messages::HeaderT imgHeader;
    imgHeader.sensor_stamp = 1;
    imgHeader.publish_stamp = 2;
    imgHeader.frame_id = "frame_id";

    messages::sensors::ImageT image;
    image.header = std::make_unique<messages::HeaderT>(imgHeader);
    image.height = 3;
    image.width = 4;
    image.encoding = "encoding";
    image.is_bigendian = true;
    image.step = 5;
    image.data.insert(image.data.end(), {255, 254, 253});

    rec.write<messages::sensors::Image, messages::sensors::ImageT>("/a", image);

    rec.close();

    std::ifstream file(filename, std::ios::in | std::ios::binary);

    std::array<char, 4> magic;
    file.read(magic.data(), magic.size());
    EXPECT_EQ(MAGIC, magic);

    uint32_t version = readUInt(file);
    EXPECT_EQ(RECORDING_MINOR, version);

    uint64_t startNS = readULong(file);
    EXPECT_EQ(0, startNS);

    uint64_t endNS = readULong(file);
    EXPECT_EQ(0, endNS);

    uint64_t messageCount = readULong(file);
    EXPECT_EQ(0, messageCount);

    // Variable header length
    uint32_t headerLength = readUInt(file);
    ASSERT_LT(64, headerLength);
    ASSERT_GT(192, headerLength);

    // Variable header
    std::vector<char> headerData(headerLength);
    file.read(headerData.data(), headerLength);
    auto header = messages::recording::UnPackHeader(headerData.data());
    ASSERT_TRUE(header.get());
    ASSERT_TRUE(header->types.get());
    EXPECT_EQ(2, header->types->definitions.size());
    EXPECT_EQ("TypeA", header->types->definitions[0]->name);
    EXPECT_EQ("", header->types->definitions[0]->hash);
    EXPECT_EQ(0, header->types->definitions[0]->schema.size());
    EXPECT_EQ("TypeB", header->types->definitions[1]->name);
    EXPECT_EQ("", header->types->definitions[1]->hash);
    EXPECT_EQ(0, header->types->definitions[1]->schema.size());
    ASSERT_TRUE(header->topics.get());
    EXPECT_EQ(2, header->topics->definitions.size());
    EXPECT_EQ("/a", header->topics->definitions[0]->topic_name);
    EXPECT_EQ(0, header->topics->definitions[0]->type_index);
    EXPECT_EQ("/b", header->topics->definitions[1]->topic_name);
    EXPECT_EQ(1, header->topics->definitions[1]->type_index);

    // Deserialize the message we wrote

    // Recording timestamp
    uint64_t timestamp = readULong(file);
    EXPECT_EQ(2, timestamp);

    // Topic index
    uint32_t index = readUInt(file);
    EXPECT_EQ(0, index);

    // Length
    uint32_t length = readUInt(file);
    EXPECT_EQ(128, length);

    // Message payload
    std::vector<char> payload(length);
    file.read(payload.data(), length);
    auto msg = messages::sensors::UnPackImage(payload.data());
    ASSERT_TRUE(msg.get());
    ASSERT_TRUE(msg->header.get());
    EXPECT_EQ(1, msg->header->sensor_stamp);
    EXPECT_EQ(2, msg->header->publish_stamp);
    EXPECT_EQ("frame_id", msg->header->frame_id);
    EXPECT_EQ(3, msg->height);
    EXPECT_EQ(4, msg->width);
    EXPECT_EQ("encoding", msg->encoding);
    EXPECT_EQ(true, msg->is_bigendian);
    EXPECT_EQ(5, msg->step);
    std::vector<uint8_t> validateImageData{255, 254, 253};
    EXPECT_EQ(validateImageData, msg->data);
}

TEST(Recording, RoundTrip) {}

}  // namespace ref
