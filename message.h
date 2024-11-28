#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class MessageType : uint8_t {
    DATA = 1,
    ACK = 2
};

struct MessageHeader {
    MessageType type;
    uint32_t message_id;
    uint32_t data_size;

    MessageHeader(MessageType type, uint32_t id, uint32_t data_size)
        : type(type), message_id(id), data_size(data_size) {}
    MessageHeader() : type(MessageType::DATA), message_id(0), data_size(0) {}
} __attribute__((packed));

class Message {
public:
    static const size_t MAX_DATA_SIZE = 4096 - sizeof(MessageHeader);

    Message(MessageType type, uint32_t id, const std::string& data = "")
        : header{type, id, static_cast<uint32_t>(data.size())}, data(data) {}

    Message() : header(), data() {}

    // Serialize message to binary format
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer(sizeof(MessageHeader) + header.data_size);
        memcpy(buffer.data(), &header, sizeof(MessageHeader));
        if (!data.empty()) {
            memcpy(buffer.data() + sizeof(MessageHeader), data.c_str(), data.size());
        }
        return buffer;
    }

    void deserializeHeader(const uint8_t* buffer, size_t size) {
        if (size != sizeof(MessageHeader)) {
            throw std::runtime_error("Invalid header size");
        }
        const MessageHeader* header = reinterpret_cast<const MessageHeader*>(buffer);
        this->header = *header;
    }

    // Deserialize from binary data
    void deserializeData(const uint8_t* buffer, size_t size) {
        if (header.data_size > 0) {
            if (size < header.data_size) {
                throw std::runtime_error("Incomplete message data");
            }
            data.assign(reinterpret_cast<const char*>(buffer), header.data_size);
        }
    }

    MessageHeader header;
    std::string data;
};
