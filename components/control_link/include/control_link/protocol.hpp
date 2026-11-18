#pragma once

#include <stdint.h>

// Mirrors the ControlBoard's src/brain_comm/protocol.rs (wiki: BrainBoard-SPI-Protocol)

constexpr int FRAME_LEN = 16;
constexpr uint8_t MAGIC = 0xA5;

enum status_t : uint8_t {
    STATUS_ACK = 0,
    STATUS_BAD_MAGIC = 1,
    STATUS_BAD_CRC = 2,
    STATUS_UNKNOWN_OPCODE = 3,
    STATUS_BAD_ARGUMENT = 4,
    STATUS_QUEUE_FULL = 5,
};

// Reply byte 1: bits 0-4 status, bits 5-7 flags
constexpr uint8_t STATUS_MASK = 0x1F;
constexpr uint8_t FLAG_RESTARTED = 0x20;
constexpr uint8_t FLAG_ODOMETRY_NOT_READY = 0x40;
constexpr uint8_t FLAG_BATTERY_LOW = 0x80;

enum opcode_t : uint8_t {
    OP_NOP = 0,
    OP_STOP = 1,
    OP_TARGET_POS = 2,
    OP_SET_SERVO = 3,
};

// x, y in mm, theta in rad (counter-clockwise), world frame of the OTOS
struct reply_t {
        uint8_t status;
        uint8_t flags;
        uint8_t seq;
        float x;
        float y;
        float theta;
};

uint8_t crc8(const uint8_t* bytes, int len);

void encodeNop(uint8_t* frame, uint8_t seq);
void encodeStop(uint8_t* frame, uint8_t seq);
void encodeTargetPos(uint8_t* frame, uint8_t seq, float x, float y, float theta);
void encodeSetServo(uint8_t* frame, uint8_t seq, uint8_t id, uint8_t angleDeg);

// false on bad magic or bad CRC
bool decodeReply(const uint8_t* frame, reply_t* reply);
