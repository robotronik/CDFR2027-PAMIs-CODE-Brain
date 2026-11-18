#include "control_link/protocol.hpp"

#include <string.h>

// CRC-8/SMBUS: poly 0x07, init 0x00
uint8_t crc8(const uint8_t* bytes, int len) {
    uint8_t crc = 0;
    for (int i = 0; i < len; i++) {
        crc ^= bytes[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc = crc << 1;
            }
        }
    }
    return crc;
}

static void beginFrame(uint8_t* frame, uint8_t opcode, uint8_t seq) {
    memset(frame, 0, FRAME_LEN);
    frame[0] = MAGIC;
    frame[1] = opcode;
    frame[2] = seq;
}

static void endFrame(uint8_t* frame) {
    frame[FRAME_LEN - 1] = crc8(frame, FRAME_LEN - 1);
}

void encodeNop(uint8_t* frame, uint8_t seq) {
    beginFrame(frame, OP_NOP, seq);
    endFrame(frame);
}

void encodeStop(uint8_t* frame, uint8_t seq) {
    beginFrame(frame, OP_STOP, seq);
    endFrame(frame);
}

// memcpy is enough for the floats: the wire is little-endian, like the ESP32-S3 and x86
void encodeTargetPos(uint8_t* frame, uint8_t seq, float x, float y, float theta) {
    beginFrame(frame, OP_TARGET_POS, seq);
    memcpy(&frame[3], &x, 4);
    memcpy(&frame[7], &y, 4);
    memcpy(&frame[11], &theta, 4);
    endFrame(frame);
}

void encodeSetServo(uint8_t* frame, uint8_t seq, uint8_t id, uint8_t angleDeg) {
    beginFrame(frame, OP_SET_SERVO, seq);
    frame[3] = id;
    frame[4] = angleDeg;
    endFrame(frame);
}

bool decodeReply(const uint8_t* frame, reply_t* reply) {
    if (frame[0] != MAGIC) {
        return false;
    }
    if (crc8(frame, FRAME_LEN - 1) != frame[FRAME_LEN - 1]) {
        return false;
    }
    reply->status = frame[1] & STATUS_MASK;
    reply->flags = frame[1] & ~STATUS_MASK;
    reply->seq = frame[2];
    memcpy(&reply->x, &frame[3], 4);
    memcpy(&reply->y, &frame[7], 4);
    memcpy(&reply->theta, &frame[11], 4);
    return true;
}
